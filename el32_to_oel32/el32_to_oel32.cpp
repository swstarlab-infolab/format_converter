#include "el32_to_oel32.h"
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/sort/sort.hpp>
#include <ash/utility/prompt.h>
#define GSTREAM_USE_LIBSTDCXX_PARALLEL_MODE
#include <parallel/algorithm>

namespace fs = ::boost::filesystem;

el32_to_oel32::el32_to_oel32(config_t const& cfg) {
    _cfg = cfg;
    _input_path = _cfg.el32_input_path;
    _output_path = _cfg.el32_output_path;
    _row = _col = (1 << std::max(0, _cfg.input_graph_size - GRID_SIZE));
    _row_grid_ID = 0;
    _el_ofs = new el32_ofstream[_col];
    for (uint32_t i = 0; i < _col; i++)
        _el_ofs[i].init_buf(_cfg.output_buf_size);
    _unique_edges_buf = static_cast<gstream::grid_format::e32_t*>(calloc((_cfg.input_buf_size >> 3), sizeof(gstream::grid_format::e32_t)));
    _total_edge = 0;
}

el32_to_oel32::~el32_to_oel32() {
    delete[] _el_ofs;
}

bool el32_to_oel32::run() {
    if (!el32_check_output_directory(_output_path, _row)) {
        return false;
    }

    el32_ifstream* el32_is = new el32_ifstream(_cfg.input_buf_size);

    for (uint32_t r = 0; r < _row; r++) {
        _row_grid_ID = r;
        _init_output_stream();
        for (uint32_t c = 0; c < _col; c++) {
            gstream::grid_format::gbid_t gbid = {r, c};
            std::string input_file_path = el32_make_unsorted_path(_input_path, r, c);
            assert(fs::exists(input_file_path));
            printf("Input: %s\n", input_file_path.c_str());

            el32_is->init_stream(input_file_path);
            uint64_t const file_size = fs::file_size(input_file_path);
            uint64_t const max_buffer_size = _cfg.input_buf_size;
            uint64_t remain_size = file_size;
            uint64_t not_optimize_size = 0;
            
            while(remain_size){
                uint64_t const buffer_size = _cfg.input_buf_size - not_optimize_size;
                uint64_t const input_size = std::min(buffer_size, remain_size);
                assert(input_size % 8 == 0);

                el32_is->read_stream(input_size, not_optimize_size / 4);
                if (_cfg.is_big_endian)
                    el32_is->convert_little_endian(buffer_size / 4);
                
                if (remain_size <= input_size) {
                    not_optimize_size = _optimize(el32_is->get_in_buf(), (not_optimize_size + input_size) / 4, gbid, true);
                }
                else {
                    not_optimize_size = _optimize(el32_is->get_in_buf(), max_buffer_size / 4, gbid, false);
                    el32_is->memmove_in_buf(0, max_buffer_size / 4 - not_optimize_size / 4, not_optimize_size);
                }

                remain_size -= input_size;
            }
            el32_is->close_stream();
        }
        _close_output_stream();
    }

    delete el32_is;
    _close_output_stream();
    el32_remove_empty_output_directory(_output_path, _row);
    return true;
}

uint64_t el32_to_oel32::get_total_edge() {
    return _total_edge;
}

void el32_to_oel32::_init_output_stream() {
    for (uint32_t i = 0; i < _col; i++) {
        _el_ofs[i].init_stream(el32_make_optimal_path(_output_path, _row_grid_ID, i));
    }
}

void el32_to_oel32::_close_output_stream() {
    for (uint32_t i = 0; i < _col; i++) {
        _el_ofs[i].close_stream();
    }
}