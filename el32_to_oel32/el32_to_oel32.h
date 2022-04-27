#include <gfstream/el32_ifstream.h>
#include <gfstream/el32_ofstream.h>
#include <gfstream/utility.h>
#include <gfstream/constant.h>
#include <gstream/grid_format/grid_stream2.h>
#include <vector>
#include <string>

class el32_to_oel32 {
public:
    struct config_t {
        uint16_t input_graph_size;
        std::string el32_input_path;
        std::string el32_output_path;
        uint64_t input_buf_size;
        uint64_t output_buf_size;
        bool is_big_endian;
    };

    el32_to_oel32(config_t const& cfg);
    ~el32_to_oel32();
    
    bool run();
    uint64_t get_total_edge();

private:
    config_t _cfg;
    std::string _input_path, _output_path;
    uint32_t _row, _col;
    uint32_t _row_grid_ID;
    el32_ofstream* _el_ofs;
    gstream::grid_format::e32_t* _unique_edges_buf;
    uint64_t _total_edge;

    uint64_t _optimize(uint32_t* const& in_buf, uint64_t const& max_index, gstream::grid_format::gbid_t const& gbid, bool is_last_buf);

    void _init_output_stream();
    void _close_output_stream();
};
