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

#find_package(ZLIB)

include_directories(${CMAKE_SOURCE_DIR}/include ${BOOST_INCLUDEDIR} ${CUDA_INCLUDE_DIRS})
link_directories(${CMAKE_SOURCE_DIR}/lib ${BOOST_LIBRARYDIR})

set(_ASH ash)
set(_GSTREAM gstream)

# ASH
file(GLOB_RECURSE _ASH_SRC ${CMAKE_SOURCE_DIR}/source/ash/*)
add_library(${_ASH} ${_ASH_SRC})

# GSTREAM
file(GLOB_RECURSE _GSTREAM_SRC ${CMAKE_SOURCE_DIR}/source/${_GSTREAM}/*)
add_library(${_GSTREAM} ${_GSTREAM_SRC})
target_link_libraries(${_GSTREAM} ${_ASH})
add_dependencies(${_GSTREAM} ${_ASH})

# MAIN APPLICATIONS
foreach(_ITEM gridgenLT-sharding gridgen-sharding gridgen-sharding2 gridinfo gridsim ash-test)
	file(GLOB_RECURSE _SOURCE ${CMAKE_SOURCE_DIR}/source/${_ITEM}/*)
	add_executable(${_ITEM} ${_SOURCE})
	add_dependencies(${_ITEM} ${_ASH} ${_GSTREAM} ${_KMALLOC})
	target_link_libraries(${_ITEM} PUBLIC ${_ASH} ${_GSTREAM} pthread boost_fiber boost_context boost_filesystem boost_system boost_exception)
endforeach()

# QUERIES
foreach(_ITEM pagerank2 degcnt tc)
	file(GLOB_RECURSE _SOURCE ${CMAKE_SOURCE_DIR}/source/query/${_ITEM}/*)
	cuda_add_executable(${_ITEM} ${_SOURCE})
	add_dependencies(${_ITEM} ${_ASH} ${_GSTREAM})
	target_link_libraries(${_ITEM} ${_GSTREAM} ${_ASH} pthread boost_fiber boost_context boost_filesystem boost_system boost_exception)
endforeach()

# TOOLS
foreach(_ITEM gridchk gridview el32_chk el32_cmp)
	file(GLOB_RECURSE _SOURCE ${CMAKE_SOURCE_DIR}/source/tool/${_ITEM}/*)
	add_executable(${_ITEM} ${_SOURCE})
	add_dependencies(${_ITEM} ${_ASH} ${_GSTREAM} ${_KMALLOC})
	target_link_libraries(${_ITEM} PUBLIC ${_ASH} ${_GSTREAM} pthread boost_fiber boost_context boost_filesystem boost_system boost_exception)
endforeach()

# TOOLS/CONVERTER
set(_GFSTREAM gfstream)
include_directories(${CMAKE_SOURCE_DIR}/source/tool/converter)
file(GLOB_RECURSE _GFSTREAM_SRC ${CMAKE_SOURCE_DIR}/source/tool/converter/${_GFSTREAM}/*)
add_library(${_GFSTREAM} ${_GFSTREAM_SRC})

foreach(_ITEM el32_converter el32_isolated el32_to_oel32 grid_to_el32 grid_to_oel32)
	file(GLOB_RECURSE _SOURCE ${CMAKE_SOURCE_DIR}/source/tool/converter/${_ITEM}/*)
	add_executable(${_ITEM} ${_SOURCE})
	add_dependencies(${_ITEM} ${_ASH} ${_GSTREAM} ${_GFSTREAM} ${_KMALLOC})
	target_link_libraries(${_ITEM} PUBLIC ${_ASH} ${_GSTREAM} ${_GFSTREAM} pthread boost_fiber boost_context boost_filesystem boost_system boost_exception)
endforeach()
