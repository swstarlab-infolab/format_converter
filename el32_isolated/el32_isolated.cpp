#include "el32_isolated.h"
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <ash/utility/prompt.h>

namespace fs = ::boost::filesystem;

el32_isolated::el32_isolated(config_t const& cfg) {
    _cfg = cfg;
    _input_path = _cfg.el32_input_path;
    _output_path = _cfg.el32_output_path;
    _row = _col = (1 << std::max(0, _cfg.input_graph_size - GRID_SIZE));
    _row_grid_ID = 0;
    _el_ofs = new el32_ofstream*[2];
    for (uint32_t i = 0; i < 2; i++)
        _el_ofs[i] = new el32_ofstream[_col];
    for (uint32_t i = 0; i < 2; i++)
        for (uint32_t j = 0; j < _col; j++)
            _el_ofs[i][j].init_buf(_cfg.output_buf_size);
}

el32_isolated::~el32_isolated() {
    // for (uint32_t i = 0; i < col; i++) {
    //     new_ID_map_v[i]->clear();
    // }
    for (uint32_t i = 0; i < 2; i++)
        delete[] _el_ofs[i];
    delete[] _el_ofs;
}

bool el32_isolated::run() {
    if (!el32_check_output_directory(_output_path, _row)) {
        return false;
    }

    el32_ifstream* el32_is = new el32_ifstream(_cfg.input_buf_size);

    uint64_t num_vertex = ((uint64_t)1 << _cfg.input_graph_size);
    _is_not_isolated_vertex = static_cast<bool*>(malloc(num_vertex));
    memset(_is_not_isolated_vertex, 0, num_vertex);
    
    _new_ID_array = static_cast<uint64_t*>(malloc(num_vertex * 8));
    memset(_new_ID_array, 0, num_vertex * 8);
    
    _process(el32_is, CONVERT_EL32);

    delete el32_is;
    _close_output_stream();
    el32_remove_empty_output_directory(_output_path, _row);
    return true;
}

uint64_t el32_isolated::get_total_edge() {
    return _total_edge;
}

void el32_isolated::_process(el32_ifstream* const& el32_is, ProcessType const& type) {
    for (uint32_t r = 0; r < _row; r++) {
        _global_row_grid_ID = _row_grid_ID;
        for (uint32_t c = 0; c < _col; c++) {
            std::string input_file_path = el32_make_unsorted_path(_input_path, r, c);
            assert(fs::exists(input_file_path));
            printf("Input: %s\n", input_file_path.c_str());

            el32_is->init_stream(input_file_path);
            uint64_t const file_size = fs::file_size(input_file_path);
            uint64_t remain_size = file_size;
            
            while(remain_size){
                uint64_t const buffer_size = std::min(_cfg.input_buf_size, remain_size);
                assert(buffer_size % 8 == 0);

                el32_is->read_stream(buffer_size);                    
                remain_size -= buffer_size;
            }
            el32_is->close_stream();
        }
    }
}

void el32_isolated::_insert_edge(uint64_t const& src_v, uint64_t const& dest_v, bool const& el_ofs_ID) {
    _total_edge++;
    uint32_t row_v_ID = static_cast<uint32_t>(src_v & GRID_MASK);
    uint32_t col_grid_ID = static_cast<uint32_t>(dest_v >> GRID_SIZE);
    uint32_t col_v_ID = static_cast<uint32_t>(dest_v & GRID_MASK);
    _el_ofs[el_ofs_ID][col_grid_ID].write_stream(row_v_ID, col_v_ID);
}

void el32_isolated::_init_output_stream() {
    _init_output_stream(0, 0);
    if (_row > 1) _init_output_stream(1, 1);
}

void el32_isolated::_init_output_stream(uint32_t const& _el_ofs_ID, uint32_t const& _row_grid_ID) {
    if (_row_grid_ID < _row) {
        for (uint32_t i = 0; i < _col; i++) {
            _el_ofs[_el_ofs_ID][i].init_stream(el32_make_unsorted_path(_output_path, _row_grid_ID, i));
        }
    }
}

void el32_isolated::_close_output_stream() {
    _close_output_stream(0);
    if (_row > 1) _close_output_stream(1);
}

void el32_isolated::_close_output_stream(uint32_t const& _el_ofs_ID) {
    for (uint32_t i = 0; i < _col; i++) {
        _el_ofs[_el_ofs_ID][i].close_stream();
    }
}

void el32_isolated::_clear() {
    _total_edge = 0;
    _row_grid_ID = 0;
}

