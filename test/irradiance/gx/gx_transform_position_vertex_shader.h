#ifndef __GX_TRANSFORM_POSITION_SHADER_H__
#define __GX_TRANSFORM_POSITION_SHADER_H__

#include <cstdint>

#include <d3d11/d3d11_pointers.h>

#include <math/math_matrix.h>

#include <gx/gx_constant_buffer_helper.h>

namespace gx
{
	class transform_position_vertex_shader_constant_buffer final
	{
		public:

		explicit transform_position_vertex_shader_constant_buffer ( ID3D11Device* device );

		void set_w(math::float4x4 value)
		{
			m_w = value;
		}

		void update ( ID3D11DeviceContext* context, math::float4x4* value )
		{
			constant_buffer_update( context, m_buffer.get(), value);
		}

		void flush ( ID3D11DeviceContext* context )
		{
			update(context, &m_w);
		}

		void bind_as_vertex_constant_buffer(ID3D11DeviceContext* context)
		{
			context->VSSetConstantBuffers(1, 1, dx::get_pointer(m_buffer));
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
            return sizeof(m_w);
        }

		private:

		d3d11::ibuffer_ptr	m_buffer;
		math::float4x4		m_w;
	};

	class transform_position_vertex_shader final
    {
		public:

		explicit transform_position_vertex_shader ( ID3D11Device* device );

		operator const ID3D11VertexShader*() const
		{
			return m_shader.get();
		}

		d3d11::ivertexshader_ptr	m_shader;
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

		d3d11::iinputlayout_ptr	m_input_layout;
	};

}

#endif