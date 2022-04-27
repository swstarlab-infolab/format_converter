#include <string>
#include <vector>
#include <fstream>

class el32_cmp {
public:
    struct config_t {
        std::string el32_path1;
        std::string el32_path2;
    };
    
    el32_cmp(config_t const& cfg);
    ~el32_cmp();

    void run();

private:
    config_t _cfg;
    uint8_t* _in_buf1;
    uint8_t* _in_buf2;
    std::ifstream _ifs1, _ifs2;
};