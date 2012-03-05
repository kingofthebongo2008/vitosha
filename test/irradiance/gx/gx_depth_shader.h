#ifndef __GX_DEPTH_SHADER_H__
#define __GX_DEPTH_SHADER_H__

#include <cstdint>
#include <memory>

#include <dx11/dx11_error.h>
#include <dx11/dx11_pointers.h>

#include <math/math_matrix.h>

namespace gx
{
	class depth_vertex_shader_constant_buffer 
	{
		public:
		depth_vertex_shader_constant_buffer ( dx11::id3d11device_ptr device );

		void set_pvw(math::matrix_float44 value)
		{
			m_pvw = value;
		}

		void update ( ID3D11DeviceContext* context, math::matrix_float44* value)
		{
			D3D11_MAPPED_SUBRESOURCE mapped_resource;
			
			dx11::throw_if_failed<dx11::d3d11_exception>(context->Map( m_buffer.get(), 0,  D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource) ) ;
			math::matrix_float44* data = static_cast< math::matrix_float44*> (mapped_resource.pData);
			*data = *value;
			
			context->Unmap( m_buffer.get(), 0);
		}

		void flush ( ID3D11DeviceContext* context )
		{
			update(context, &m_pvw);
		}

		void bind_as_vertex_constant_buffer(ID3D11DeviceContext* context)
		{
			context->VSSetConstantBuffers(0, 1, dx11::get_pointer(m_buffer));
		}

		private:

		dx11::id3d11buffer_ptr	m_buffer;
		math::matrix_float44	m_pvw;
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
    };
}

#endif