#include "el32_to_oel32.h"
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/sort/sort.hpp>
#include <ash/utility/prompt.h>
#define GSTREAM_USE_LIBSTDCXX_PARALLEL_MODE
#include <parallel/algorithm>

namespace fs = ::boost::filesystem;

el32_to_oel32::el32_to_oel32(config_t const& cfg) {
    _cfg = cfg;
    _input_path = _cfg.el32_input_path;
    _output_path = _cfg.el32_output_path;
    _row = _col = (1 << std::max(0, _cfg.input_graph_size - GRID_SIZE));
    _row_grid_ID = 0;
    _el_ofs = new el32_ofstream[_col];
    for (uint32_t i = 0; i < _col; i++)
        _el_ofs[i].init_buf(_cfg.output_buf_size);
    _unique_edges_buf = static_cast<gstream::grid_format::e32_t*>(calloc((_cfg.input_buf_size >> 3), sizeof(gstream::grid_format::e32_t)));
    _total_edge = 0;
}

el32_to_oel32::~el32_to_oel32() {
    delete[] _el_ofs;
}

bool el32_to_oel32::run() {
    if (!el32_check_output_directory(_output_path, _row)) {
        return false;
    }

    el32_ifstream* el32_is = new el32_ifstream(_cfg.input_buf_size);

    for (uint32_t r = 0; r < _row; r++) {
        _row_grid_ID = r;
        _init_output_stream();
        for (uint32_t c = 0; c < _col; c++) {
            gstream::grid_format::gbid_t gbid = {r, c};
            std::string input_file_path = el32_make_unsorted_path(_input_path, r, c);
            assert(fs::exists(input_file_path));
            printf("Input: %s\n", input_file_path.c_str());

            el32_is->init_stream(input_file_path);
            uint64_t const file_size = fs::file_size(input_file_path);
            uint64_t const max_buffer_size = _cfg.input_buf_size;
            uint64_t remain_size = file_size;
            uint64_t not_optimize_size = 0;
            
            while(remain_size){
                uint64_t const buffer_size = _cfg.input_buf_size - not_optimize_size;
                uint64_t const input_size = std::min(buffer_size, remain_size);
                assert(input_size % 8 == 0);

                el32_is->read_stream(input_size, not_optimize_size / 4);
                if (_cfg.is_big_endian)
                    el32_is->convert_little_endian(buffer_size / 4);
                
                if (remain_size <= input_size) {
                    not_optimize_size = _optimize(el32_is->get_in_buf(), (not_optimize_size + input_size) / 4, gbid, true);
                }
                else {
                    not_optimize_size = _optimize(el32_is->get_in_buf(), max_buffer_size / 4, gbid, false);
                    el32_is->memmove_in_buf(0, max_buffer_size / 4 - not_optimize_size / 4, not_optimize_size);
                }

                remain_size -= input_size;
            }
            el32_is->close_stream();
        }
        _close_output_stream();
    }

    delete el32_is;
    _close_output_stream();
    el32_remove_empty_output_directory(_output_path, _row);
    return true;
}

uint64_t el32_to_oel32::get_total_edge() {
    return _total_edge;
}

void el32_to_oel32::_insert_edge(gstream::grid_format::gbid_t const& gbid, uint32_t const& src_v, uint32_t const& dest_v) {
    _total_edge++;
    _el_ofs[gbid.col].write_stream(src_v, dest_v);
}

uint64_t el32_to_oel32::_optimize(uint32_t* const& in_buf, uint64_t const& max_index, gstream::grid_format::gbid_t const& gbid, bool is_last_buf) {
    using namespace gstream;
    using namespace grid_format;
    
    e32_t* input_edges = reinterpret_cast<e32_t*>(in_buf);
    uint64_t num_input_edges = max_index >> 1;
    uint64_t num_process_edges = num_input_edges;

    if (!is_last_buf) {
        e32_t last_edge = input_edges[num_input_edges - 1];
        for (uint64_t i = num_input_edges - 1; i >= 0; i--) {
            if (input_edges[i].u == last_edge.u){
                num_process_edges--;
            }
            else break;
        }
        // output buffer is small
        assert(num_process_edges > 0);
    }

    uint64_t num_no_loops;
    if (gbid.col == gbid.row) {
    #if defined(GSTREAM_USE_CXX17_EXECUTION_POLICY)
        e32_t* last = std::partition(std::execution::par_unseq, input_edges, input_edges + num_process_edges, e32_is_not_diagonal);
    #elif defined(GSTREAM_USE_LIBSTDCXX_PARALLEL_MODE)
        e32_t* last = __gnu_parallel::partition(input_edges, input_edges + num_process_edges, e32_is_not_diagonal);
    #else
        e32_t* last = std::partition(input_edges, input_edges + num_process_edges, e32_is_not_diagonal);
    #endif
        num_no_loops = static_cast<uint64_t>(reinterpret_cast<char const*>(last) - reinterpret_cast<char const*>(input_edges)) / sizeof(e32_t*);
    }
    else {
        num_no_loops = num_process_edges;
    }

    /* Sort */
    boost::sort::block_indirect_sort(input_edges, input_edges + num_no_loops, e32_compare, std::thread::hardware_concurrency());

    /* Deduplication */
    uint64_t num_duplicates, num_uniques;
    {
    #if defined(GSTREAM_USE_CXX17_EXECUTION_POLICY)
        e32_t* last = std::unique_copy(std::execution::par_unseq, input_edges, input_edges + num_no_loops, _unique_edges_buf, e32_equal);
    #elif defined(GSTREAM_USE_LIBSTDCXX_PARALLEL_MODE)
        e32_t* last = __gnu_parallel::unique_copy(input_edges, input_edges + num_no_loops, _unique_edges_buf, e32_equal);
    #else
        e32_t* last = std::unique_copy(input_edges, input_edges + num_no_loops, _unique_edges_buf.unique_edges, e32_equal);
    #endif
        num_uniques = static_cast<uint64_t>(reinterpret_cast<char const*>(last) - reinterpret_cast<char const*>(_unique_edges_buf)) / sizeof(e32_t*);
        assert(num_uniques <= num_no_loops);
        num_duplicates = num_no_loops - num_uniques;
        assert(num_duplicates < num_no_loops);
    }

    for (uint64_t i = 0; i < num_uniques; i++) {
        _insert_edge(gbid, _unique_edges_buf[i].u, _unique_edges_buf[i].v);
    }

    return (num_input_edges - num_process_edges) << 3;
}

void el32_to_oel32::_init_output_stream() {
    for (uint32_t i = 0; i < _col; i++) {
        _el_ofs[i].init_stream(el32_make_optimal_path(_output_path, _row_grid_ID, i));
    }
}

void el32_to_oel32::_close_output_stream() {
    for (uint32_t i = 0; i < _col; i++) {
        _el_ofs[i].close_stream();
    }
}