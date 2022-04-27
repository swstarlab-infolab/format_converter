cmake_minimum_required(VERSION 3.10)
project(GStream)

# C++
set(CMAKE_CXX_COMPILER g++-7)
set(CMAKE_CXX_FLAGS "-Wall -O3 -lstdc++ -std=c++17 -fopenmp -D_FILE_OFFSET_BITS=64 -DASH_IGNORE_DMESG")

# CUDA
find_package(CUDA REQUIRED)

set(CUDA_HOST_COMPILER g++-7)
set(CUDA_SEPARABLE_COMPILATION ON)
set(CUDA_PROPAGATE_HOST_FLAGS OFF)
list(APPEND CUDA_NVCC_FLAGS 
	#-gencode arch=compute_50,code=sm_50
#-gencode arch=compute_52,code=sm_52
	#-gencode arch=compute_60,code=sm_60
	#-gencode arch=compute_61,code=sm_61
	-gencode arch=compute_80,code=sm_80
	-O3 -std=c++14 -Xcompiler -fopenmp)

# Boost
set(BOOST_ROOT "/usr/local/boost_1_77_0")
set(BOOST_INCLUDEDIR "~/local/include")
set(BOOST_LIBRARYDIR "~/local/lib")
find_package(Boost 1.77.0 REQUIRED)


# TOOLS/CONVERTER
set(_GFSTREAM gfstream)
include_directories(${CMAKE_SOURCE_DIR}/source/tool/converter)
file(GLOB_RECURSE _GFSTREAM_SRC ${CMAKE_SOURCE_DIR}/source/tool/converter/${_GFSTREAM}/*)
add_library(${_GFSTREAM} ${_GFSTREAM_SRC})

foreach(_ITEM el32_isolated el32_to_oel32 grid_to_el32 grid_to_oel32)
	file(GLOB_RECURSE _SOURCE ${CMAKE_SOURCE_DIR}/source/tool/converter/${_ITEM}/*)
	add_executable(${_ITEM} ${_SOURCE})
	add_dependencies(${_ITEM} ${_ASH} ${_GSTREAM} ${_GFSTREAM} ${_KMALLOC})
endforeach()
