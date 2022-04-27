#ifndef GSTREAM_TOOL_GFSTREAM_UTILITY_H
#define GSTREAM_TOOL_GFSTREAM_UTILITY_H
#include <gfstream/constant.h>
#include <string>

bool el32_check_output_directory(std::string const& path, uint32_t row);
void el32_remove_empty_output_directory(std::string const& path, uint32_t row);
std::string el32_make_unsorted_path(std::string const& path, uint32_t const& row_grid_ID, uint32_t const& col_grid_ID);
std::string el32_make_optimal_path(std::string const& path, uint32_t const& row_grid_ID, uint32_t const& col_grid_ID);
std::string el32_make_subdir_path(std::string const& path, uint32_t const& row_grid_ID);
std::string adj_make_path(std::string const& path, uint16_t const& file_number);

#endif // GSTREAM_TOOL_GFSTREAM_UTILITY_H
