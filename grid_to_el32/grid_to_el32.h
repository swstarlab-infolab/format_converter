#include <gfstream/el32_ofstream.h>
#include <gfstream/utility.h>
#include <gfstream/constant.h>
#include <gstream/grid_format/grid_stream2.h>
#include <string>

class grid_to_el32 {
public:
    struct config_t {
        std::string grid_name;
        std::string grid_input_path;
        std::string el32_output_path;
        uint64_t output_buf_size;
        bool is_dense_format;
    };

    grid_to_el32(config_t const& cfg);
    ~grid_to_el32();

    bool run();
    uint64_t get_total_edge();
    
private:
    config_t _cfg;
    std::string _input_path, _output_path;
    uint32_t _row, _col;
    uint32_t _row_grid_ID;
    uint64_t _base_shard_size;
    void* input_buf;
    void* sparse_buf;
    el32_ofstream* _el_ofs;
    uint64_t _total_edge;

    gstream::grid_format::grid_stream2 _gs;
    bool _open_grid_stream();
    
    void _init_output_stream();
    void _close_output_stream();
};
