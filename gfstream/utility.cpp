#include <gfstream/utility.h>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <ash/utility/prompt.h>
#include <ash/detail/malloc.h>

namespace fs = ::boost::filesystem;

bool el32_check_output_directory(std::string const& path, uint32_t row) {
    std::string sub_dir_path = el32_make_subdir_path(path, 0);
    if (fs::exists(sub_dir_path)) {
        ash::yesno_prompt prompt;
        prompt.text  = "Binary edge list file is already exists. Do you want replace it?";
        if (!prompt()) {
            printf("Binary edge list convertion aborted by user.\n");
            return false;
        }
        {
            for (uint32_t i = 0; i < row; i++) {
                sub_dir_path = el32_make_subdir_path(path, i);
                if (fs::exists(sub_dir_path)) {
                    bool const r = fs::remove_all(sub_dir_path);
                    assert(r && "Failed to the delete existing Binary edge list file");
                }
            }
        }
    }

    for (uint32_t i = 0; i < row; i++) {
        sub_dir_path = el32_make_subdir_path(path, i);
        fs::create_directory(sub_dir_path);
    }
    return true;
}

void el32_remove_empty_output_directory(std::string const& path, uint32_t row) {
    namespace fs = ::boost::filesystem;
    for (uint32_t i = 0; i < row; i++) {
        std::string sub_dir_path = el32_make_subdir_path(path, i);
        if (fs::is_empty(sub_dir_path)) {
            bool const r = fs::remove(sub_dir_path);
            assert(r && "Failed to the delete empty Binary edge list file");
        }
    }
}

std::string el32_make_unsorted_path(std::string const& path, uint32_t const& row_grid_ID, uint32_t const& col_grid_ID) {
    namespace fs = ::boost::filesystem;
    boost::format fmt(EL32_SUBDIR_FORMAT + "/" + EL32_UNSORTED_FILE_FORMAT);
    fmt % row_grid_ID % row_grid_ID % col_grid_ID;
    fs::path file_name(fmt.str());
    fs::path fs_file_path = path / file_name;
    return fs_file_path.string();
}

std::string el32_make_optimal_path(std::string const& path, uint32_t const& row_grid_ID, uint32_t const& col_grid_ID) {
    namespace fs = ::boost::filesystem;
    boost::format fmt(EL32_SUBDIR_FORMAT + "/" + EL32_OPTIMAL_FILE_FORMAT + "/" + EL32_OPTIMAL_FILE_FORMAT);
    fmt % row_grid_ID % row_grid_ID % col_grid_ID;
    fs::path file_name(fmt.str());
    fs::path fs_file_path = path / file_name;
    return fs_file_path.string();
}

std::string el32_make_subdir_path(std::string const& path, uint32_t const& row_grid_ID) {
    namespace fs = ::boost::filesystem;
    boost::format fmt(EL32_SUBDIR_FORMAT + "/" + EL32_OPTIMAL_FILE_FORMAT);
    fmt % row_grid_ID;
    fs::path subdir_name(fmt.str());
    fs::path fs_subdir_path = path / subdir_name;
    return fs_subdir_path.string();
}

std::string adj_make_path(std::string const& path, uint16_t const& file_number) {
    boost::format fmt(ADJ_FORMAT + "/" + EL32_OPTIMAL_FILE_FORMAT);
    fmt % file_number;
    fs::path file_name(fmt.str());
    fs::path fs_file_path = path / file_name;
    return fs_file_path.string();
}

