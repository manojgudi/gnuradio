/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include "gras/basic_block_pimpl.h"

#include <gnuradio/hier_block2.h>

gr::hier_block2_sptr gr::make_hier_block2(
    const std::string &name,
    gr::io_signature::sptr input_signature,
    gr::io_signature::sptr output_signature
)
{
    return gr::hier_block2_sptr(new gr::hier_block2(name, input_signature, output_signature));
}

gr::hier_block2::hier_block2(
    const std::string &name,
    gr::io_signature::sptr input_signature,
    gr::io_signature::sptr output_signature
)
{
    GRASP_INIT();
    GRASP.hier_block.reset(new gras::HierBlock(name));
}

gr::hier_block2::~hier_block2(void)
{
    pimpl.reset();
}

void gr::hier_block2::lock(void)
{
    //dont tear down the flow graph
}

void gr::hier_block2::unlock(void)
{
    //thread safe commit topology changes
    GRASP.conn_block().commit();
}

gr::flat_flowgraph_sptr gr::hier_block2::flatten() const
{
    return flat_flowgraph_sptr(); //nothing to do
}

gr::hier_block2::opaque_self gr::hier_block2::self()
{
    //hide hier self in this opaque_self
    //dont need sptr magic hack, just connect w/ *this
    return boost::static_pointer_cast<gr::basic_block>(pimpl);
}

static gras::Element &get_elem_sptr(gr::basic_block_sptr block, boost::shared_ptr<void> pimpl)
{
    //check if hier is hidden in an opaque_self
    if (size_t(block.get()) == size_t(pimpl.get())) return GRASP.conn_block();

    //otherwise pick out the initialized element
    gras_basic_block_pimpl &p = (*reinterpret_cast<gras_basic_block_pimpl *>(block->pimpl.get()));
    boost::shared_ptr<gras::Element> element;
    if (p.block) element = p.block;
    if (p.hier_block) element = p.hier_block;

    //properly parent the block sptr into the element
    if (element)
    {
        element->set_container(new gras::WeakContainerSharedPtr(block));
        return *element;
    }

    throw std::runtime_error("get_elem_sptr::cannot coerce block");
}

void gr::hier_block2::connect(gr::basic_block_sptr block)
{
    GRASP.conn_block().connect(get_elem_sptr(block, pimpl));
}

void gr::hier_block2::connect(
    gr::basic_block_sptr src, int src_port,
    gr::basic_block_sptr dst, int dst_port
)
{
    GRASP.conn_block().connect(
        get_elem_sptr(src, pimpl), src_port,
        get_elem_sptr(dst, pimpl), dst_port
    );
}

void gr::hier_block2::disconnect(gr::basic_block_sptr block)
{
    GRASP.conn_block().disconnect(get_elem_sptr(block, pimpl));
}

void gr::hier_block2::disconnect(
    gr::basic_block_sptr src, int src_port,
    gr::basic_block_sptr dst, int dst_port
)
{
    GRASP.conn_block().disconnect(
        get_elem_sptr(src, pimpl), src_port,
        get_elem_sptr(dst, pimpl), dst_port
    );
}

void gr::hier_block2::disconnect_all()
{
    GRASP.conn_block().disconnect_all();
}

//TODO -- use GRAS's builtin message passing capability

void gr::hier_block2::msg_connect(basic_block_sptr src, pmt::pmt_t srcport,
                 basic_block_sptr dst, pmt::pmt_t dstport)
{
    throw std::runtime_error("msg no");
}
void gr::hier_block2::msg_connect(basic_block_sptr src, std::string srcport,
                 basic_block_sptr dst, std::string dstport)
{
    throw std::runtime_error("msg no");
}
void gr::hier_block2::msg_disconnect(basic_block_sptr src, pmt::pmt_t srcport,
                 basic_block_sptr dst, pmt::pmt_t dstport)
{
    throw std::runtime_error("msg no");
}
void gr::hier_block2::msg_disconnect(basic_block_sptr src, std::string srcport,
                 basic_block_sptr dst, std::string dstport)
{
    throw std::runtime_error("msg no");
}

gr::hier_block2_sptr gr::hier_block2::to_hier_block2()
{
    return boost::static_pointer_cast<gr::hier_block2>(shared_from_this());
}
