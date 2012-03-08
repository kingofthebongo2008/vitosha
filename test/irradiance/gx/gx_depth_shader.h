#ifndef __GX_DEPTH_SHADER_H__
#define __GX_DEPTH_SHADER_H__

#include <cstdint>
#include <memory>

#include <dx11/dx11_error.h>
#include <dx11/dx11_pointers.h>

#include <math/math_matrix.h>

#include <gx/gx_constant_buffer_helper.h>

namespace gx
{
	class depth_vertex_shader_constant_buffer 
	{
		public:
		depth_vertex_shader_constant_buffer ( dx11::id3d11device_ptr device );

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

		private:

		dx11::id3d11buffer_ptr	m_buffer;
		math::matrix_float44	m_wvp;
	};

	class depth_vertex_shader
    {
		public:

		depth_vertex_shader ( dx11::id3d11device_ptr device );

		void bind(ID3D11DeviceContext* context, depth_vertex_shader_constant_buffer* constant_buffer)
		{
			constant_buffer->bind_as_vertex_constant_buffer(context);
			context->VSSetShader(m_shader.get(), nullptr, 0 );
		}

		dx11::id3d11vertexshader_ptr						 m_shader;
		const void*											 m_code;
		std::uint32_t										 m_code_size;
    };
}

#endif