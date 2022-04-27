#ifndef GSTREAM_TOOL_GFSTREAM_EL32_OFSTREAM_H
#define GSTREAM_TOOL_GFSTREAM_EL32_OFSTREAM_H
#include <string>
#include <fstream>

class el32_ofstream {
public:
    el32_ofstream();
    ~el32_ofstream();

    void init_stream(std::string const& output_file_path);
    void write_stream(uint32_t const& row_v_ID, uint32_t const& col_v_ID);
    void close_stream();

private:
    std::string _output_path;
    std::ofstream _ofs;
    uint32_t* _out_buf;
    uint64_t _output_buf_size;
    uint64_t _offset;
};

#endif // GSTREAM_TOOL_GFSTREAM_EL32_OFSTREAM_H
