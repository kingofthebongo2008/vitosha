#ifndef __GX_DEBUG_VIEW_SPACE_DEPTH_PIXEL_SHADER_H__
#define __GX_DEBUG_VIEW_SPACE_DEPTH_PIXEL_SHADER_H__

#include <cstdint>

#include <d3d11/d3d11_error.h>

#include <d3d11/d3d11_pointers.h>
#include <d3d11/d3d11_helpers.h>

#include <math/math_matrix.h>

#include <gx/gx_constant_buffer_helper.h>

namespace gx
{
    class __declspec(align(16)) debug_view_space_depth_pixel_shader_constant_buffer
	{
		public:

		explicit debug_view_space_depth_pixel_shader_constant_buffer ( ID3D11Device* device );

        void set_zn_zf( float zn, float zf )
        {
            m_zn_zf = math::set( zn, zf, 0.0f, 0.0f );
        }

		void flush ( ID3D11DeviceContext* context )
		{
			constant_buffer_update(context, m_buffer.get(), (void*) &m_zn_zf, size() );
		}

		void bind_as_pixel_constant_buffer(ID3D11DeviceContext* context, uint32_t slot)
		{
			context->PSSetConstantBuffers(slot, 1, dx::get_pointer(m_buffer));
		}

		void bind_as_pixel_constant_buffer(ID3D11DeviceContext* context)
		{
			bind_as_pixel_constant_buffer(context, 0);
		}

		operator ID3D11Buffer*()
		{
			return m_buffer.get();
		}

		operator const ID3D11Buffer*() const
		{
			return m_buffer.get();
		}

        size_t size() const
        {
            return sizeof(m_zn_zf);
        }

		public:
        math::float4     m_zn_zf;
		d3d11::ibuffer_ptr	m_buffer;
	};

	class debug_view_space_depth_pixel_shader final
    {
		public:

		explicit debug_view_space_depth_pixel_shader ( ID3D11Device* device );

		operator const ID3D11PixelShader*() const
		{
			return m_shader.get();
		}

		d3d11::ipixelshader_ptr		m_shader;
		const void*						m_code;
		uint32_t						m_code_size;
    };
}

#endif