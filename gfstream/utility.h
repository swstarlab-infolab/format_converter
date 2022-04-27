#ifndef GSTREAM_TOOL_GFSTREAM_UTILITY_H
#define GSTREAM_TOOL_GFSTREAM_UTILITY_H
#include <gfstream/constant.h>
#include <string>

bool el32_check_output_directory(std::string const& path, uint32_t row);
void el32_remove_empty_output_directory(std::string const& path, uint32_t row);

#endif // GSTREAM_TOOL_GFSTREAM_UTILITY_H
