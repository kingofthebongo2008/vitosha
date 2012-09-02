#ifndef __GX_SCREEN_SPACE_UV_VERTEX_SHADER_H__
#define __GX_SCREEN_SPACE_UV_VERTEX_SHADER_H__

#include <cstdint>

#include <d3d11/d3d11_pointers.h>

#include <math/math_matrix.h>

#include <gx/gx_constant_buffer_helper.h>

namespace gx
{
	class screen_space_uv_vertex_shader_constant_buffer 
	{
		public:

		explicit screen_space_uv_vertex_shader_constant_buffer ( ID3D11Device* device );

		void set_wvp(math::float4x4 value)
		{
			m_wvp = value;
		}

		void update ( ID3D11DeviceContext* context, math::float4x4* value )
		{
			constant_buffer_update( context, m_buffer.get(), value);
		}

		void flush ( ID3D11DeviceContext* context )
		{
			update(context, &m_wvp);
		}

		void bind_as_vertex_constant_buffer(ID3D11DeviceContext* context)
		{
			context->VSSetConstantBuffers(0, 1, dx::get_pointer(m_buffer));
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
            return sizeof(m_wvp);
        }

		private:

		d3d11::ibuffer_ptr	m_buffer;
		math::float4x4	m_wvp;
	};

	class screen_space_uv_vertex_shader
    {
		public:

		explicit screen_space_uv_vertex_shader ( ID3D11Device* device );

		operator ID3D11VertexShader*()
		{
			return m_shader.get();
		}

		operator const ID3D11VertexShader*() const
		{
			return m_shader.get();
		}

		d3d11::ivertexshader_ptr	m_shader;
		const void*						m_code;
		uint32_t						m_code_size;
    };

}

#endif