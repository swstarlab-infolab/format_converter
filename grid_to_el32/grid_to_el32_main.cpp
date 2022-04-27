#include "grid_to_el32.h"
#include <chrono>
#include <ash/utility/dbg_log.h>

grid_to_el32::config_t make_config(char* argv[]) noexcept {
    grid_to_el32::config_t cfg;
    try {
        std::string argv_s[6];
        for (uint16_t i = 0; i < 6; i++)
            argv_s[i] = argv[i];
            
        cfg.grid_name = argv_s[1];
        cfg.grid_input_path = argv_s[2];
        cfg.el32_output_path = argv_s[3];
        cfg.output_buf_size = static_cast<uint64_t>(std::stoull(argv_s[4])) * 1024 * 1024;
        if (std::tolower(argv_s[5][0]) == 'y')
            cfg.is_dense_format = true;
        else if (std::tolower(argv_s[5][0]) == 'n')
            cfg.is_dense_format = false;
        else
            ASH_FATAL("Parameter $5: Dense format flag is wrong");
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
    if (argc < 6) {
        printf("usage> grid_to_el32 <GRID_NAME> <INPUT_PATH> <OUTPUT_PATH> <OUTPUT_BUF_SIZE:MB> <IS_DENSE_FORMAT>");
        return -1;
    }

    grid_to_el32::config_t const cfg = make_config(argv);

    using std::chrono::duration_cast;
    using std::chrono::system_clock;
    using std::chrono::milliseconds;

    double duration;
    auto start = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    grid_to_el32* prog = new grid_to_el32(cfg);
    if (prog->run()) {
        printf("total edge : %lu\n", prog->get_total_edge());
    }
    else {
        printf("grid_to_el32 returns error!\n");
    }
    delete prog;

    auto end = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    duration = (double)(end - start);
    printf("total time : %lf\n", duration / 1000);
    return 0;
}