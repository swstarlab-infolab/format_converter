#include <gfstream/adj_ifstream.h>
#include <cassert>
#include <cstring>

adj_ifstream::adj_ifstream(uint64_t const& input_buf_size) {
    _input_buf_size = input_buf_size;
    _in_buf = static_cast<uint8_t*>(malloc(_input_buf_size));
    _convert_in_buf = static_cast<uint64_t*>(malloc((_input_buf_size / 3) << 2));
}

adj_ifstream::~adj_ifstream() {
    free(_in_buf);
    free(_convert_in_buf);
}

void adj_ifstream::init_stream(std::string const& input_file_path) {
    _ifs.open(input_file_path, std::ios::in | std::ios::binary);
    assert(_ifs.is_open());
    // clear_buf();
}

void adj_ifstream::read_stream(uint64_t const& buffer_size) {
    _ifs.read(reinterpret_cast<char*>(_in_buf), buffer_size);
    uint64_t const bytes_transferred = _ifs.gcount();
    assert(bytes_transferred == buffer_size);
}

void adj_ifstream::close_stream() {
    _ifs.close();
}

void adj_ifstream::clear_buf() {
    memset(_in_buf, 0, _input_buf_size);
}

uint64_t* adj_ifstream::get_convert_in_buf() {
    return _convert_in_buf;
}

void adj_ifstream::convert_little_endian(uint64_t const& buffer_size) {
    uint64_t r = 0;
    uint32_t index = 0;
    for (uint64_t i = 0; i < buffer_size; i++) {
        r <<= 8;
        r |= *(_in_buf + i);
        if (i % 6 == 5) {
            _convert_in_buf[index++] = r;
            r = 0;
        }
    }
}
