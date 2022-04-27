#include <gfstream/el32_ifstream.h>
#include <gfstream/constant.h>
#include <boost/endian/conversion.hpp>
#include <cassert>
#include <cstring>

el32_ifstream::el32_ifstream(uint64_t const& input_buf_size) {
    _input_buf_size = input_buf_size;
    _in_buf = static_cast<uint32_t*>(malloc(_input_buf_size));
    _convert_in_buf = static_cast<uint64_t*>(malloc(_input_buf_size << 2));
}

el32_ifstream::~el32_ifstream() {
    free(_in_buf);
    free(_convert_in_buf);
}

void el32_ifstream::init_stream(std::string const& input_file_path) {
    _ifs.open(input_file_path, std::ios::in | std::ios::binary);
    assert(_ifs.is_open());
    // clear_buf();
}

void el32_ifstream::read_stream(uint64_t const& buffer_size) {
    _ifs.read(reinterpret_cast<char*>(_in_buf), buffer_size);
    uint64_t const bytes_transferred = _ifs.gcount();
    assert(bytes_transferred == buffer_size);
}

void el32_ifstream::read_stream(uint64_t const& buffer_size, uint64_t const& offset) {
    _ifs.read(reinterpret_cast<char*>(_in_buf + offset), buffer_size);
    uint64_t const bytes_transferred = _ifs.gcount();
    assert(bytes_transferred == buffer_size);
}

void el32_ifstream::close_stream() {
    _ifs.close();
}

void el32_ifstream::clear_buf() {
    memset(_in_buf, 0, _input_buf_size);
}

uint32_t* el32_ifstream::get_in_buf() {
    return _in_buf;
}

uint64_t* el32_ifstream::get_convert_in_buf() {
    return _convert_in_buf;
}

void el32_ifstream::convert_origin_vertex(uint64_t const& max_index, uint32_t const& src_row_ID,  uint32_t const& src_col_ID) {
#pragma omp parallel for
    for (uint64_t i = 0; i < (max_index >> 1); i++) {
        _convert_in_buf[(i << 1)] = ((uint64_t)src_row_ID << GRID_SIZE) | _in_buf[(i << 1)];
        _convert_in_buf[(i << 1) + 1] = ((uint64_t)src_col_ID << GRID_SIZE) | _in_buf[(i << 1) + 1];
    }
}

void el32_ifstream::convert_new_vertex(uint64_t const& max_index, uint64_t* const& new_ID_array, uint32_t const& src_row_ID,  uint32_t const& src_col_ID) {
#pragma omp parallel for
    for (uint64_t i = 0; i < (max_index >> 1); i++) {
        _convert_in_buf[(i << 1)] = new_ID_array[((uint64_t)src_row_ID << GRID_SIZE) | _in_buf[(i << 1)]];
        _convert_in_buf[(i << 1) + 1] = new_ID_array[((uint64_t)src_col_ID << GRID_SIZE) | _in_buf[(i << 1) + 1]];
        
        assert(_convert_in_buf[(i << 1)] != 0);
        assert(_convert_in_buf[(i << 1) + 1] != 0);

        _convert_in_buf[(i << 1)]--;
        _convert_in_buf[(i << 1) + 1]--;
    }
}

void el32_ifstream::memmove_in_buf(uint64_t const& dest, uint64_t const& src, uint64_t const& len) {
    memmove(_in_buf + dest, _in_buf + src, len);
}