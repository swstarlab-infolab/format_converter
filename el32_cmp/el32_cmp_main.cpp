#include "el32_cmp.h"
#include <cassert>
#include <string>

el32_cmp::config_t make_config(char* argv[]) noexcept {
    el32_cmp::config_t cfg;
    try {
        std::string argv_s[3];
        for (uint16_t i = 0; i < 3; i++)
            argv_s[i] = argv[i];
          
        cfg.el32_path1 = argv_s[1];
        cfg.el32_path2 = argv_s[2];
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
    if (argc < 3) {
        printf("usage> el32_cmp <EL32_PATH1> <EL32_PATH2>\n");
        return -1;
    }
    
    el32_cmp::config_t cfg = make_config(argv);
    el32_cmp* prog = new el32_cmp(cfg);
    prog->run();
    delete prog;
}
