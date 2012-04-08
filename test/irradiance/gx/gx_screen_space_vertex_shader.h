#ifndef __GX_SCREEN_SPACE_VERTEX_SHADER_H__
#define __GX_SCREEN_SPACE_VERTEX_SHADER_H__

#include <cstdint>
#include <memory>

#include <dx11/dx11_error.h>
#include <dx11/dx11_pointers.h>

#include <math/math_matrix.h>

#include <gx/gx_constant_buffer_helper.h>

namespace gx
{
	class screen_space_vertex_shader_constant_buffer 
	{
		public:

		screen_space_vertex_shader_constant_buffer ( ID3D11Device* device );

		void set_wvp(math::matrix_float44 value)
		{
			m_wvp = value;
		}

		void update ( ID3D11DeviceContext* context, math::matrix_float44* value)
		{
			constant_buffer_update( context, m_buffer.get(), value);
		}

		void flush ( ID3D11DeviceContext* context )
		{
			update(context, &m_wvp);
		}

		void bind_as_vertex_constant_buffer(ID3D11DeviceContext* context)
		{
			context->VSSetConstantBuffers(0, 1, dx11::get_pointer(m_buffer));
		}

		public:

		dx11::id3d11buffer_ptr	m_buffer;

		math::matrix_float44	m_wvp;
	};

	class screen_space_vertex_shader
    {
		public:

		screen_space_vertex_shader ( ID3D11Device* device );

		void bind(ID3D11DeviceContext* context, screen_space_vertex_shader_constant_buffer* constant_buffer)
		{
			constant_buffer->bind_as_vertex_constant_buffer(context);
			context->VSSetShader(m_shader.get(), nullptr, 0 );
		}

		operator dx11::id3d11vertexshader_ptr()
		{
			return m_shader;
		}

		operator ID3D11VertexShader*()
		{
			return m_shader.get();
		}

		operator const ID3D11VertexShader*() const
		{
			return m_shader.get();
		}

		dx11::id3d11vertexshader_ptr						 m_shader;
		const void*											 m_code;
		uint32_t										 m_code_size;
    };
}

#endif