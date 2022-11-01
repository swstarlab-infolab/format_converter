#include "el32_converter.h"
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <ash/utility/prompt.h>

namespace fs = ::boost::filesystem;

el32_converter::el32_converter(config_t const& cfg) {
    _cfg = cfg;
    _input_path = _cfg.adj_input_path;
    _output_path = _cfg.el32_output_path;
    _row = _col = (1 << std::max(0, _cfg.input_graph_size - GRID_SIZE));
    _row_grid_ID = _row_v_ID = 0;
    _el_ofs = new el32_ofstream[_col];
    for (uint32_t i = 0; i < _col; i++)
        _el_ofs[i].init_buf(_cfg.output_buf_size);
    _total_edge = 0;
    
    _src_v = UINT64_INF;
    _adj_size = 0;
}

el32_converter::~el32_converter() {
    delete[] _el_ofs;
}

bool el32_converter::run() {
    if (!el32_check_output_directory(_output_path, _row)) {
        return false;
    }
    adj_ifstream* adj_is = new adj_ifstream(_cfg.input_buf_size);
    _init_output_stream();
    
    for (uint16_t i = 0; i < _cfg.input_file_number; i++){
        std::string input_file_path = adj_make_path(_input_path, i);
        printf("Input: %s\n", input_file_path.c_str());

        assert(fs::exists(input_file_path));
        adj_is->init_stream(input_file_path);
        uint64_t const file_size = fs::file_size(input_file_path);
        uint64_t remain_size = file_size;
        
        while(remain_size){
            uint64_t const buffer_size = std::min(_cfg.input_buf_size, remain_size);
            assert(buffer_size % 6 == 0);

            adj_is->read_stream(buffer_size);
            adj_is->convert_little_endian(buffer_size);
            _convert_el32(adj_is->get_convert_in_buf(), buffer_size / 6);

            remain_size -= buffer_size;
        }
        adj_is->close_stream();
    }
    delete adj_is;
    _close_output_stream();
    el32_remove_empty_output_directory(_output_path, _row);
    return true;
}

uint64_t el32_converter::get_total_edge() {
    return _total_edge;
}

void el32_converter::_insert_edge(uint64_t const& dest_v) {
    _total_edge++;
    uint32_t col_grid_ID = static_cast<uint32_t>(dest_v >> GRID_SIZE);
    uint32_t col_v_ID = static_cast<uint32_t>(dest_v & GRID_MASK);
    _el_ofs[col_grid_ID].write_stream(_row_v_ID, col_v_ID);
}

void el32_converter::_convert_el32(uint64_t* const& convert_buf, uint64_t const& max_index) {
    for (uint64_t i = 0; i < max_index; i++) {
        if (_adj_size) {
            _insert_edge(*(convert_buf + i)); 
            if (!(--_adj_size)) _src_v = UINT64_INF;
        }
        else {
            if (_src_v == UINT64_INF) {
                _src_v = *(convert_buf + i);
                uint32_t new_row_grid_ID = static_cast<uint32_t>(_src_v >> GRID_SIZE);
                while (_row_grid_ID != new_row_grid_ID) {
                    _close_output_stream();
                    _row_grid_ID++;
                    _init_output_stream();
                }
                _row_v_ID = static_cast<uint32_t>(_src_v & GRID_MASK);
            }
            else _adj_size = *(convert_buf + i);
        }
    }
}

void el32_converter::_init_output_stream() {
    for (uint32_t i = 0; i < _col; i++) {
        _el_ofs[i].init_stream(el32_make_unsorted_path(_output_path, _row_grid_ID, i));
    }
}

void el32_converter::_close_output_stream() {
    for (uint32_t i = 0; i < _col; i++) {
        _el_ofs[i].close_stream();
    }
}