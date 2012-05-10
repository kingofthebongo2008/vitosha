#ifndef __GX_TRANSFORM_POSITION_SHADER_H__
#define __GX_TRANSFORM_POSITION_SHADER_H__

#include <cstdint>

#include <dx11/dx11_pointers.h>

#include <math/math_matrix.h>

#include <gx/gx_constant_buffer_helper.h>

namespace gx
{
	class transform_position_vertex_shader_constant_buffer 
	{
		public:

		explicit transform_position_vertex_shader_constant_buffer ( ID3D11Device* device );

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
			context->VSSetConstantBuffers(0, 1, dx11::get_pointer(m_buffer));
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

		dx11::id3d11buffer_ptr	m_buffer;
		math::float4x4	m_wvp;
	};

	class transform_position_vertex_shader
    {
		public:

		explicit transform_position_vertex_shader ( ID3D11Device* device );

		operator ID3D11VertexShader*()
		{
			return m_shader.get();
		}

		operator const ID3D11VertexShader*() const
		{
			return m_shader.get();
		}

		dx11::id3d11vertexshader_ptr	m_shader;
		const void*						m_code;
		uint32_t						m_code_size;
    };

	class transform_position_input_layout
	{
		public:

		transform_position_input_layout( ID3D11Device* device, transform_position_vertex_shader* shader);

		operator ID3D11InputLayout*()
		{
			return m_input_layout.get();
		}

		operator const ID3D11InputLayout*() const
		{
			return m_input_layout.get();
		}

		dx11::id3d11inputlayout_ptr	m_input_layout;
	};

}

#endif