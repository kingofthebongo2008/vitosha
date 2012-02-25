#include "precompiled.h"

#include <gx/gx_indexed_draw_call.h>

#include <sys/sys_base.h>

#include <gx/gx_draw_call_context.h>
#include <D3D11.h>


namespace gx
{
    indexed_draw_call::indexed_draw_call( index_info info, dx11::id3d11buffer_ptr vertex_buffer, dx11::id3d11buffer_ptr index_buffer ) :
        m_index_info(info)
        , m_vertex_buffer(vertex_buffer)
        , m_index_buffer(index_buffer)
    {

    }

    indexed_draw_call::indexed_draw_call()
    {

    }

    indexed_draw_call::~indexed_draw_call()
    {

    }

    void indexed_draw_call::draw(draw_call_context* context)
    {
        UNUSED_PARAMETER(context);

        ID3D11DeviceContext* device_context = context->m_device_context.get();

        uint32_t stride = m_index_info.m_vertex_size;
        uint32_t offset = 0;
        ID3D11Buffer* buffer = m_vertex_buffer.get();

        device_context->IASetIndexBuffer( m_index_buffer.get(), DXGI_FORMAT_R16_UINT, 0);
        device_context->IASetVertexBuffers( 0, 1, &buffer, &stride, &offset );
        device_context->DrawIndexed( m_index_info.m_index_count, m_index_info.m_start_index_location, m_index_info.m_base_vertex_location);
    }
}