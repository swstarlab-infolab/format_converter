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
