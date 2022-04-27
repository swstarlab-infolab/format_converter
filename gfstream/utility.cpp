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
