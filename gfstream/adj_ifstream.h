#ifndef GSTREAM_TOOL_GFSTREAM_ADJ_IFSTREAM_H
#define GSTREAM_TOOL_GFSTREAM_ADJ_IFSTREAM_H
#include <string>
#include <fstream>

class adj_ifstream {
public:
    adj_ifstream(uint64_t const& input_buf_size);
    ~adj_ifstream();

    void init_stream(std::string const& input_file_path);
    void read_stream(uint64_t const& buffer_size);
    void close_stream();
    void clear_buf();
    uint64_t* get_convert_in_buf();

    void convert_little_endian(uint64_t const& buffer_size);

private:
    std::ifstream _ifs;
    uint8_t* _in_buf;
    uint64_t* _convert_in_buf;
    uint64_t _input_buf_size;
};

#endif // GSTREAM_TOOL_GFSTREAM_ADJ_IFSTREAM_H