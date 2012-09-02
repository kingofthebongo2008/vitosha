#ifndef __GX_MESH_DRAW_CALL_H__
#define __GX_MESH_DRAW_CALL_H__

#include <cstdint>

#include <math/math_matrix.h>
#include <d3d11/d3d11_pointers.h>

namespace gx
{
	template < uint32_t vertex_buffer_count >
    class indexed_draw_call
    {
        public:

        struct index_info
        {
            uint32_t                m_index_count;
            uint32_t                m_start_index_location;
            uint32_t                m_base_vertex_location;
            uint32_t                m_vertex_size[vertex_buffer_count];
			uint32_t                m_start_instance_location;
        };

        indexed_draw_call( index_info info, d3d11::ibuffer_ptr vertex_buffer[vertex_buffer_count], d3d11::ibuffer_ptr index_buffer ) : m_index_info(info), m_index_buffer(index_buffer)
		{
			for (auto i = 0; i < vertex_buffer_count; ++i)
			{
				m_vertex_buffer[i] = vertex_buffer[i];
			}
		}

		void indexed_draw_call::draw(ID3D11DeviceContext* device_context) const
		{
			ID3D11Buffer*	buffers[vertex_buffer_count];
			uint32_t		offset[vertex_buffer_count];

			for ( auto i = 0; i < vertex_buffer_count; ++i)
			{
				offset[i] = 0;
				buffers[i] = m_vertex_buffer[i].get();
			}

			device_context->IASetIndexBuffer( m_index_buffer.get(), DXGI_FORMAT_R16_UINT, 0);
			device_context->IASetVertexBuffers( 0, vertex_buffer_count, &buffers[0], &m_index_info.m_vertex_size[0], &offset[0] );
			device_context->DrawIndexed( m_index_info.m_index_count, m_index_info.m_start_index_location, m_index_info.m_base_vertex_location);
		}

		void indexed_draw_call::draw_instanced(ID3D11DeviceContext* device_context, uint32_t instance_count) const
		{
			ID3D11Buffer*	buffers[vertex_buffer_count];
			uint32_t		offset[vertex_buffer_count];

			for ( auto i = 0; i < vertex_buffer_count; ++i)
			{
				offset[i] = 0;
				buffers[i] = m_vertex_buffer[i].get();
			}

			device_context->IASetIndexBuffer( m_index_buffer.get(), DXGI_FORMAT_R16_UINT, 0);
			device_context->IASetVertexBuffers( 0, vertex_buffer_count, &buffers[0], &m_index_info.m_vertex_size[0], &offset[0] );
			device_context->DrawIndexedInstanced(m_index_info.m_index_count, instance_count, m_index_info.m_start_index_location, m_index_info.m_base_vertex_location, m_index_info.m_start_instance_location);
		}


		protected:

		indexed_draw_call();

        index_info                m_index_info;
        d3d11::ibuffer_ptr    m_vertex_buffer[vertex_buffer_count];
        d3d11::ibuffer_ptr    m_index_buffer;
    };

	typedef indexed_draw_call<1> indexed_draw_call_1;
	typedef indexed_draw_call<2> indexed_draw_call_2;
	typedef indexed_draw_call<3> indexed_draw_call_3;

	template < uint32_t vertex_size > inline indexed_draw_call_1 create_indexed_draw_call(uint32_t index_count, d3d11::ibuffer_ptr vertex_buffer, d3d11::ibuffer_ptr index_buffer)
	{
		indexed_draw_call_1::index_info info = {};
		info.m_index_count = index_count;
		info.m_vertex_size[0] = vertex_size;
		d3d11::ibuffer_ptr vertex_buffer_s[] = vertex_buffer;

		return indexed_draw_call_1(info, vertex_buffer_s, index_buffer);
	}

	template < uint32_t vertex_size0, uint32_t vertex_size1 >inline indexed_draw_call_2 create_indexed_draw_call(uint32_t index_count, d3d11::ibuffer_ptr vertex_buffer0, d3d11::ibuffer_ptr vertex_buffer1, d3d11::ibuffer_ptr index_buffer)
	{
		indexed_draw_call_2::index_info info = {};
		info.m_index_count = index_count;
		info.m_vertex_size[0] = vertex_size0;
		info.m_vertex_size[1] = vertex_size1;
		d3d11::ibuffer_ptr vertex_buffer_s[] = { vertex_buffer0, vertex_buffer1 } ;
		return indexed_draw_call_2(info, vertex_buffer_s, index_buffer);

	}

	template < uint32_t vertex_size0, uint32_t vertex_size1, uint32_t vertex_size2 >inline indexed_draw_call_3 create_indexed_draw_call(uint32_t index_count, d3d11::ibuffer_ptr vertex_buffer0, d3d11::ibuffer_ptr vertex_buffer1, d3d11::ibuffer_ptr vertex_buffer2, d3d11::ibuffer_ptr index_buffer)
	{
		indexed_draw_call_3::index_info info = {};
		info.m_index_count = index_count;
		info.m_vertex_size[0] = vertex_size0;
		info.m_vertex_size[1] = vertex_size1;
		info.m_vertex_size[2] = vertex_size2;
		d3d11::ibuffer_ptr vertex_buffer_s[] = { vertex_buffer0, vertex_buffer1, vertex_buffer2 } ;
		return indexed_draw_call_3(info, vertex_buffer_s, index_buffer);
	}

}

#endif