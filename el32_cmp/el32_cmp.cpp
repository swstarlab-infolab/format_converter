#include "el32_cmp.h"
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

const size_t BUF_SIZE = 3 * 1024 * 1024;

namespace fs = ::boost::filesystem;

el32_cmp::el32_cmp(config_t const& cfg) {
    _cfg = cfg;
    _in_buf1 = static_cast<uint8_t*>(malloc(BUF_SIZE));
    _in_buf2 = static_cast<uint8_t*>(malloc(BUF_SIZE));
}

el32_cmp::~el32_cmp() {}

void el32_cmp::run() {
    assert(fs::is_directory(_cfg.el32_path1));
    assert(fs::is_directory(_cfg.el32_path2));

    std::vector<std::string> sub_dir1 = _get_sub_directory(_cfg.el32_path1);
    std::vector<std::string> sub_dir2 = _get_sub_directory(_cfg.el32_path2);

    assert(sub_dir1.size() == sub_dir2.size());
    int row = sub_dir1.size();
    
    for (int i = 0; i < row; i++) {
        std::vector<std::string> sub_file1 = _get_sub_regular_file(sub_dir1[i]);
        std::vector<std::string> sub_file2 = _get_sub_regular_file(sub_dir2[i]);

        assert(sub_file1.size() == sub_file2.size());
        int col = sub_file1.size();

        for(int j = 0; j < col; j++) {
            printf("Progress: [GBID: %u, %u]\n", i, j);
            _compare_file(sub_file1[j], sub_file2[j]);
        }
    }

    printf("SUCCESS!\n");
}