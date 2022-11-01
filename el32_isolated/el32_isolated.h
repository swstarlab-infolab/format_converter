#include <gfstream/el32_ifstream.h>
#include <gfstream/el32_ofstream.h>
#include <gfstream/utility.h>
#include <gfstream/constant.h>
#include <vector>
#include <string>

class el32_isolated {
public:
    struct config_t {
        uint16_t input_graph_size;
        std::string el32_input_path;
        std::string el32_output_path;
        uint64_t input_buf_size;
        uint64_t output_buf_size;
    };

    el32_isolated(config_t const& cfg);
    ~el32_isolated();
    
    bool run();
    uint64_t get_total_edge();

private:
    enum ProcessType {
        GET_ISOLATED,
        CONVERT_EL32
    };

    config_t _cfg;
    std::string _input_path, _output_path;
    uint32_t _row, _col;
    uint32_t _row_grid_ID, _global_row_grid_ID;
    uint32_t _el_ofs_ID;
    el32_ofstream** _el_ofs;
    uint64_t _total_edge;
    uint64_t _global_remain_size;
    bool* _is_not_isolated_vertex;
    uint64_t* _new_ID_array;

    void _process(el32_ifstream* const& el32_is, ProcessType const& type);
    void _insert_edge(uint64_t const& src_v, uint64_t const& dest_v, bool const& el_ofs_ID);

    void _init_output_stream();
    void _init_output_stream(uint32_t const& _el_ofs_ID, uint32_t const& _row_grid_ID);
    void _close_output_stream();
    void _close_output_stream(uint32_t const& _el_ofs_ID);
    void _clear();
};
