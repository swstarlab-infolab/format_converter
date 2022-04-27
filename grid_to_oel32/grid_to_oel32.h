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
};
