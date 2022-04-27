#include "el32_to_oel32.h"
#include <ash/utility/dbg_log.h>
#include <chrono>
#include <cassert>
#include <string>

el32_to_oel32::config_t make_config(char* argv[]) noexcept {
    el32_to_oel32::config_t cfg;
    try {
        std::string argv_s[7];
        for (uint16_t i = 0; i < 7; i++)
            argv_s[i] = argv[i];

        cfg.input_graph_size = static_cast<uint16_t>(std::stoul(argv_s[1]));
        cfg.el32_input_path = argv_s[2];
        cfg.el32_output_path = argv_s[3];
        cfg.input_buf_size = static_cast<uint64_t>(std::stoull(argv_s[4])) * 1024 * 1024;
        cfg.output_buf_size = static_cast<uint64_t>(std::stoull(argv_s[5])) * 1024 * 1024;
        if (std::tolower(argv_s[6][0]) == 'y')
            cfg.is_big_endian = true;
        else if (std::tolower(argv_s[6][0]) == 'n')
            cfg.is_big_endian = false;
        else
            ASH_FATAL("Parameter $6: Big-endian flag is wrong");
            
        assert(cfg.input_buf_size % 8 == 0);
        assert(cfg.output_buf_size % 8 == 0);
    }
    catch (std::exception const& ex) {
        fprintf(stderr, "Exception occurred in %s\ndetail: %s\n", __FUNCTION__, ex.what());
    }
    catch (...) {
        fprintf(stderr, "Exception occurred in %s\n", __FUNCTION__);
    }
    return cfg;
}

int main(int argc, char* argv[]) {
    if (argc < 7) {
        printf("usage> el32_to_oel32 <RMAT_SIZE> <INPUT_PATH> <OUTPUT_PATH> <INPUT_BUF_SIZE:MB> <OUTPUT_BUF_SIZE:MB> <IS_BIG_ENDIAN>\n");
        return -1;
    }
    
    el32_to_oel32::config_t cfg = make_config(argv);

    using std::chrono::duration_cast;
    using std::chrono::system_clock;
    using std::chrono::milliseconds;

    double duration;
    auto start = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    el32_to_oel32* prog = new el32_to_oel32(cfg);
    if (prog->run()) {
        printf("total edge : %lu\n", prog->get_total_edge());
    }
    delete prog;
    
    auto end = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    duration = (double)(end - start);
    printf("total time : %lf\n", duration / 1000);
}
