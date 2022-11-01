#include "el32_converter.h"
#include <ctime>
#include <cassert>
#include <string>

el32_converter::config_t make_config(char* argv[]) noexcept {
    el32_converter::config_t cfg;
    try {
        std::string argv_s[7];
        for (uint16_t i = 0; i < 7; i++)
            argv_s[i] = argv[i];

        cfg.input_graph_size = static_cast<uint16_t>(std::stoul(argv_s[1]));
        cfg.adj_input_path = argv_s[2];
        cfg.el32_output_path = argv_s[3];
        cfg.input_buf_size = static_cast<uint64_t>(std::stoull(argv_s[4])) * 1024 * 1024;
        cfg.output_buf_size = static_cast<uint64_t>(std::stoull(argv_s[5])) * 1024 * 1024;
        cfg.input_file_number = static_cast<uint16_t>(std::stoul(argv_s[6]));

        assert(cfg.input_buf_size % 6 == 0);
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
        printf("usage> el32_converter <RMAT_SIZE> <INPUT_PATH> <OUTPUT_PATH> <INPUT_BUF_SIZE:MB> <OUTPUT_BUF_SIZE:MB> <INPUT_FILE_NUMBER>\n");
        return -1;
    }
    
    el32_converter::config_t cfg = make_config(argv);

    clock_t start, end;
    double duration;
    start = clock();

    el32_converter* el32_cvt = new el32_converter(cfg);
    if (el32_cvt->run()) {
        printf("total edge : %lu\n", el32_cvt->get_total_edge());
    }
    delete el32_cvt;
    
    end = clock();
    duration = (double)(end - start) / CLOCKS_PER_SEC;
    printf("total time : %f\n", duration);
}
