#include "grid_to_el32.h"
#include <gstream/grid_format/dense_block.h>
#include <gstream/grid_format/sparse_block.h>
#include <gstream/grid_format/extent.h>
#include <gstream/grid_format/detail/gridgen_utility.h>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <ash/utility/prompt.h>
#include <ash/detail/malloc.h>

namespace fs = ::boost::filesystem;

grid_to_el32::grid_to_el32(config_t const& cfg) {
    _cfg = cfg;
    _input_path = _cfg.grid_input_path;
    _output_path = _cfg.el32_output_path;
    assert(_open_grid_stream() == true);
    _row = _gs.grid_info().dim.y;
    _col = _gs.grid_info().dim.x;
    _base_shard_size = _gs.grid_info().base_shard_size;
    input_buf = ash::aligned_malloc(_base_shard_size, gstream::DiskIoSectorSize);
    sparse_buf = malloc(_base_shard_size);
    _el_ofs = new el32_ofstream[_col];
    for (uint32_t i = 0; i < _col; i++)
        _el_ofs[i].init_buf(_cfg.output_buf_size);
    _total_edge = 0;
}

grid_to_el32::~grid_to_el32() {
    _gs.close();
    free(sparse_buf); sparse_buf = nullptr;
    ash::aligned_free(input_buf);
    delete[] _el_ofs;
}

bool grid_to_el32::run() {
    if (!_cfg.is_dense_format) {
        printf("Not support sparse format yet.\n");
        return false;
    }

    if (!el32_check_output_directory(_output_path, _row)) {
        return false;
    }

    using namespace gstream;
    using namespace grid_format;
    
    for (uint32_t i = 0; i < _row; i++) {
        _row_grid_ID = i;
        _init_output_stream();
        for (uint32_t j = 0; j < _col; j++) {
            gbid_t gbid = {i, j};
            stree_t const* stree = _gs.stree(gbid);
            stree->dfs([&](stree_node_ptr snode) {
                if (snode->header.node_type == qtree_node_type::InternalNode)
                    return;
                _iterate_shard(snode->leaf, gbid);
            });
        }
        _close_output_stream();
    }

    return true;
}

uint64_t grid_to_el32::get_total_edge() {
    return _total_edge;
}

bool grid_to_el32::_open_grid_stream() {
    namespace fs = boost::filesystem;
    using namespace gstream;
    using namespace grid_format;
    if (!fs::exists(_input_path)) {
        printf("grid input path is not found!\n");
        return false;
    }

    grid_stream2::config_t gs_cfg;
    gs_cfg.grid_dir  = _input_path.data();
    gs_cfg.grid_name = _cfg.grid_name.data();
    gs_cfg.extent_size = 0;
    gs_cfg.opt = GSTREAM_USE_DIRECT_IO;

    if (!_gs.open(gs_cfg)) {
        printf("grid open fail!\n");
        return false;
    }
    return true;
}

void grid_to_el32::_insert_edge(gstream::grid_format::gbid_t const& gbid, uint32_t const& src_v, uint32_t const& dest_v) {
    _total_edge++;
    _el_ofs[gbid.col].write_stream(src_v, dest_v);
}

void grid_to_el32::_iterate_shard(gstream::grid_format::sleaf_t const& sleaf, gstream::grid_format::gbid_t const& gbid) {
    using namespace gstream;
    using namespace grid_format;

    extent_load_info const& ld_info = _gs.ext_ld_info(sleaf.ext_info.extent_id);

    _gs.load_extent(input_buf, ld_info);
    
    dense_block* dense_shard = static_cast<dense_block*>(ash::seek_pointer(input_buf, ld_info.data_offset));
    dense_shard->dense_to_sparse(sleaf, sparse_buf);
    sparse_block* shard = static_cast<sparse_block*>(sparse_buf);

    uint32_t const num_lists =  shard->num_lists();
    for (uint32_t i = 0; i < num_lists; ++i) {
        laddr_t const src = shard->source_vertex(i);
        sparse_block::adj_list_t adj = shard->adj_list(i);
        for (uint32_t j = 0; j < adj.length; ++j) {
            laddr_t const dst = adj[j];
            _insert_edge(gbid, src, dst);
        }
    }
}

void grid_to_el32::_init_output_stream() {
    for (uint32_t i = 0; i < _col; i++) {
        _el_ofs[i].init_stream(el32_make_unsorted_path(_output_path, _row_grid_ID, i));
    }
}

void grid_to_el32::_close_output_stream() {
    for (uint32_t i = 0; i < _col; i++) {
        _el_ofs[i].close_stream();
    }
}