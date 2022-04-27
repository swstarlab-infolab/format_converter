#include <gfstream/el32_ofstream.h>
#include <cassert>
#include <cstring>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>

el32_ofstream::el32_ofstream() {
}

el32_ofstream::~el32_ofstream() {
    free(_out_buf);
}

void el32_ofstream::init_stream(std::string const& output_file_path) {
    _offset = 0;
    _output_path = output_file_path;
    _ofs.open(output_file_path, std::ios::out | std::ios::binary);
    assert(_ofs.is_open());
    // clear_buf();
}

void el32_ofstream::write_stream(uint32_t const& row_v_ID, uint32_t const& col_v_ID) {
    memcpy(_out_buf + (_offset >> 2), &row_v_ID, 4);
    _offset += 4;
    memcpy(_out_buf + (_offset >> 2), &col_v_ID, 4);
    _offset += 4;
    if(_offset == _output_buf_size) {
        _flush_buf();
    }
}

void el32_ofstream::close_stream() {
    namespace fs = ::boost::filesystem;
    _flush_buf();
    _ofs.close();
    if (!fs::file_size(_output_path))
        fs::remove(_output_path);
    _ofs.clear();
}

void el32_ofstream::init_buf(uint64_t const& output_buf_size) {
    _output_buf_size = output_buf_size;
    _out_buf = static_cast<uint32_t*>(malloc(_output_buf_size));
}

void el32_ofstream::_clear_buf() {
    memset(_out_buf, 0, _output_buf_size);
}

void el32_ofstream::_flush_buf() {
    _ofs.write(reinterpret_cast<char*>(_out_buf), _offset);
    assert(!_ofs.bad() && "output stream I/O error.");
    _offset = 0;
    // clear_buf();
}