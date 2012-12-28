#ifndef __GX_COLOR_PIXEL_SHADER_H__
#define __GX_COLOR_PIXEL_SHADER_H__

#include <cstdint>
#include <memory>

#include <d3d11/d3d11_error.h>
#include <d3d11/d3d11_pointers.h>

#include <math/math_matrix.h>

#include <gx/gx_constant_buffer_helper.h>

namespace gx
{
	class __declspec(align(16)) color_pixel_shader_constant_buffer 
	{
		public:

		explicit color_pixel_shader_constant_buffer ( ID3D11Device* device );

		math::float4 get_color() const
		{
			return m_color;
		}

		void set_color(math::float4 value)
		{
			m_color = value;
		}

		void update ( ID3D11DeviceContext* context, math::float4* value)
		{
			constant_buffer_update( context, m_buffer.get(), value);
		}

		void update ( ID3D11DeviceContext* context, math::float4 value)
		{
			constant_buffer_update( context, m_buffer.get(), value);
		}

		void flush ( ID3D11DeviceContext* context )
		{
			update(context, &m_color);
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
            return sizeof(m_color);
        }

		public:

		math::float4		m_color;
		d3d11::ibuffer_ptr	m_buffer;
	};

	class color_pixel_shader final
    {
		public:

		explicit color_pixel_shader ( ID3D11Device* device );

		operator const ID3D11PixelShader*() const
		{
			return m_shader.get();
		}

		d3d11::ipixelshader_ptr	m_shader;
		const void*					m_code;
		uint32_t					m_code_size;
    };
}

#endif