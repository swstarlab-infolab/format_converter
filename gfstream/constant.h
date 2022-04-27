#ifndef GSTREAM_TOOL_GFSTREAM_CONSTANT_H
#define GSTREAM_TOOL_GFSTREAM_CONSTANT_H
#include <string>

uint16_t const GRID_SIZE = 24;
uint64_t const GRID_MASK = ((uint64_t)1 << GRID_SIZE) - 1;
uint64_t const UINT64_INF = 0xffffffff;

std::string const ADJ_FORMAT = "part-%05d";
std::string const EL32_SUBDIR_FORMAT = "%d"; 
std::string const EL32_OPTIMAL_FILE_FORMAT = "%d-%d.el32.optimal";
std::string const EL32_UNSORTED_FILE_FORMAT = "%d-%d.el32.unsorted";

#endif // GSTREAM_TOOL_GFSTREAM_CONSTANT_H
