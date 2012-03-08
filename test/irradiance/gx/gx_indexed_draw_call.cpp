#include "precompiled.h"

#include <gx/gx_indexed_draw_call.h>

#include <sys/sys_base.h>

#include <gx/gx_draw_call_context.h>
#include <D3D11.h>


namespace gx
{
    indexed_draw_call::indexed_draw_call( index_info info, dx11::id3d11buffer_ptr vertex_buffer, dx11::id3d11buffer_ptr index_buffer ) :
        m_index_info(info)
        , m_index_buffer(index_buffer)
        , m_buffer_count(1)
    {
        m_vertex_buffer[0] = vertex_buffer;
    }

    indexed_draw_call::indexed_draw_call( 
                        index_info info, 
                        dx11::id3d11buffer_ptr vertex_buffer0, 
                        dx11::id3d11buffer_ptr vertex_buffer1, 
                        dx11::id3d11buffer_ptr index_buffer ) :
        m_index_info(info)
        , m_index_buffer(index_buffer)
        , m_buffer_count(2)
    {
        m_vertex_buffer[0] = vertex_buffer0;
        m_vertex_buffer[1] = vertex_buffer1;
    }

    indexed_draw_call::indexed_draw_call()
    {

    }

    indexed_draw_call::~indexed_draw_call()
    {

    }

    void indexed_draw_call::draw(ID3D11DeviceContext* device_context)
    {
		uint32_t stride = m_index_info.m_vertex_size;
        uint32_t offset = 0;
        ID3D11Buffer* buffers[2] = { m_vertex_buffer[0].get(), m_vertex_buffer[1].get() };

        device_context->IASetIndexBuffer( m_index_buffer.get(), DXGI_FORMAT_R16_UINT, 0);
        device_context->IASetVertexBuffers( 0, m_buffer_count, &buffers[0], &stride, &offset );
        device_context->DrawIndexed( m_index_info.m_index_count, m_index_info.m_start_index_location, m_index_info.m_base_vertex_location);
    }
}