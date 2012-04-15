#ifndef __GX_MESH_DRAW_CALL_H__
#define __GX_MESH_DRAW_CALL_H__

#include <cstdint>

#include <math/math_matrix.h>
#include <dx11/dx11_pointers.h>


namespace gx
{
    class indexed_draw_call
    {
        public:

        struct index_info
        {
            uint32_t                m_index_count;
            uint32_t                m_start_index_location;
            uint32_t                m_base_vertex_location;
            uint32_t                m_vertex_size_0;
			uint32_t                m_vertex_size_1;
			uint32_t                m_start_instance_location;
        };

        indexed_draw_call( index_info info, dx11::id3d11buffer_ptr vertex_buffer, dx11::id3d11buffer_ptr index_buffer );
        indexed_draw_call( index_info info, dx11::id3d11buffer_ptr vertex_buffer0, dx11::id3d11buffer_ptr vertex_buffer1, dx11::id3d11buffer_ptr index_buffer );

        ~indexed_draw_call();

		void draw(ID3D11DeviceContext* device_context);
		void draw_instanced(ID3D11DeviceContext* device_context, uint32_t instance_count);

		protected:

		indexed_draw_call();

        index_info                m_index_info;

        dx11::id3d11buffer_ptr    m_vertex_buffer[2];
        dx11::id3d11buffer_ptr    m_index_buffer;
        uint8_t                   m_buffer_count;
    };
}

#endif