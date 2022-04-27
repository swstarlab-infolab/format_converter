#ifndef GSTREAM_TOOL_GFSTREAM_EL32_IFSTREAM_H
#define GSTREAM_TOOL_GFSTREAM_EL32_IFSTREAM_H
#include <string>
#include <fstream>
#include <vector>

class el32_ifstream {
public:
    el32_ifstream(uint64_t const& input_buf_size);
    ~el32_ifstream();

    void init_stream(std::string const& input_file_path);
    void read_stream(uint64_t const& buffer_size);
    void read_stream(uint64_t const& buffer_size, uint64_t const& offset);
    void close_stream();
    void clear_buf();
    uint32_t* get_in_buf();
    uint64_t* get_convert_in_buf();

    void memmove_in_buf(uint64_t const& dest, uint64_t const& src, uint64_t const& len);

private:
    std::ifstream _ifs;
    uint32_t* _in_buf;
    uint64_t* _convert_in_buf;
    uint64_t _input_buf_size;
};

#endif // GSTREAM_TOOL_GFSTREAM_EL32_IFSTREAM_H