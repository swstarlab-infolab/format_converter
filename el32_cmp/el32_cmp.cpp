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

std::vector<std::string> el32_cmp::_get_sub_directory(std::string const& path) {    
    std::vector<std::string> sub_dir;
    for(auto& file: boost::make_iterator_range(fs::directory_iterator(path), {})) {
        if (fs::is_directory(file.path()))
            sub_dir.push_back(file.path().string());
    }
    sort(sub_dir.begin(), sub_dir.end());
    return sub_dir;
}

std::vector<std::string> el32_cmp::_get_sub_regular_file(std::string const& path) {    
    std::vector<std::string> sub_file;
    for(auto& file: boost::make_iterator_range(fs::directory_iterator(path), {})) {
        if (fs::is_regular_file(file.path()))
            sub_file.push_back(file.path().string());
    }
    sort(sub_file.begin(), sub_file.end());
    return sub_file;
}

void el32_cmp::read_stream1(uint64_t const& buffer_size) {
    _ifs1.read(reinterpret_cast<char*>(_in_buf1), buffer_size);
    uint64_t const bytes_transferred = _ifs1.gcount();
    assert(bytes_transferred == buffer_size);
}

void el32_cmp::read_stream2(uint64_t const& buffer_size) {
    _ifs2.read(reinterpret_cast<char*>(_in_buf2), buffer_size);
    uint64_t const bytes_transferred = _ifs2.gcount();
    assert(bytes_transferred == buffer_size);
}

void el32_cmp::_compare_file(std::string const& path1, std::string const& path2) {
    uint64_t file_size1 = fs::file_size(path1);
    uint64_t file_size2 = fs::file_size(path2);
    assert(file_size1 == file_size2);
    
    _ifs1.open(path1, std::ios::in | std::ios::binary);
    assert(_ifs1.is_open());

    _ifs2.open(path2, std::ios::in | std::ios::binary);
    assert(_ifs2.is_open());

    uint64_t remain_size = file_size1;

    while(remain_size){
        uint64_t const buffer_size = std::min(BUF_SIZE, remain_size);

        read_stream1(buffer_size);
        read_stream2(buffer_size);

        assert(memcmp(_in_buf1, _in_buf2, buffer_size) == 0);

        remain_size -= buffer_size;
    }

    _ifs1.close(); _ifs1.clear();
    _ifs2.close(); _ifs2.clear();

}

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