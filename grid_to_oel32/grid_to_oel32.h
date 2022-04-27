#include <gfstream/el32_ofstream.h>
#include <gfstream/utility.h>
#include <gfstream/constant.h>
#include <gstream/grid_format/grid_stream2.h>
#include <set>
#include <map>
#include <string>
#include <vector>

class grid_to_oel32 {
public:
    struct config_t {
        std::string grid_name;
        std::string grid_input_path;
        std::string el32_output_path;
        uint64_t output_buf_size;
        bool is_dense_format;
    };

    grid_to_oel32(config_t const& cfg);
    ~grid_to_oel32();

    bool run();
    uint64_t get_total_edge();
    
private:
    config_t _cfg;
    std::string _input_path, _output_path;
    uint32_t _row, _col;
    uint32_t _row_grid_ID;
    uint64_t _base_shard_size;
    void* input_buf;
    el32_ofstream* _el_ofs;
    uint64_t _total_edge;
    std::vector<shard_info> _candidate_shard;
    std::set<shard_info, shard_info_col_cmp> _inprocess_shard;
    std::map<uint64_t, uint32_t> _src_vertex_idx;

    gstream::grid_format::grid_stream2 _gs;
    bool _open_grid_stream();

    void _make_candidate_shard(gstream::grid_format::gbid_t const& gbid);
    void _progress_shard(gstream::grid_format::gbid_t const& gbid);
    
    void _init_output_stream();
    void _close_output_stream();
};
