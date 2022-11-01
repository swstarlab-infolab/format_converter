#include <gfstream/adj_ifstream.h>
#include <gfstream/el32_ofstream.h>
#include <gfstream/utility.h>
#include <gfstream/constant.h>
#include <string>

class el32_converter {
public:
    struct config_t {
        uint16_t input_graph_size;
        std::string adj_input_path;
        std::string el32_output_path;
        uint64_t input_buf_size;
        uint64_t output_buf_size;
        uint16_t input_file_number;
    };

    el32_converter(config_t const& cfg);
    ~el32_converter();
    
    bool run();
    uint64_t get_total_edge();

private:
    config_t _cfg;
    std::string _input_path, _output_path;
    uint32_t _row, _col;
    uint32_t _row_grid_ID, _row_v_ID;
    el32_ofstream* _el_ofs;
    uint64_t _total_edge;

    uint64_t _src_v, _adj_size;

    void _insert_edge(uint64_t const& dest_v);
    void _convert_el32(uint64_t* const& convert_buf, uint64_t const& max_index);

    void _init_output_stream();
    void _close_output_stream();
};
