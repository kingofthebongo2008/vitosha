#ifndef __GX_TRANSFORM_POSITION_NORMAL_SHADER_H__
#define __GX_TRANSFORM_POSITION_NORMAL_SHADER_H__

#include <cstdint>
#include <tuple>

#include <dx11/dx11_pointers.h>

#include <math/math_matrix.h>

#include <gx/gx_constant_buffer_helper.h>

namespace gx
{
	class transform_position_normal_vertex_shader_constant_buffer 
	{
		public:

		explicit transform_position_normal_vertex_shader_constant_buffer ( ID3D11Device* device );

        void set_wvp(math::matrix_float44 value)
		{
			m_wvp = value;
		}

        void set_normal_transform(math::matrix_float44 value)
		{
			m_world = value;
		}

        void flush ( ID3D11DeviceContext* context )
		{
            constant_buffer_update(context, m_buffer.get(), (void*) &m_wvp, size() );
		}

        void bind_as_vertex_constant_buffer(ID3D11DeviceContext* context)
		{
			context->VSSetConstantBuffers(0, 1, dx11::get_pointer(m_buffer));
		}

        void bind_as_geometry_constant_buffer(ID3D11DeviceContext* context)
		{
			context->GSSetConstantBuffers(0, 1, dx11::get_pointer(m_buffer));
		}

        inline size_t size() const
		{
			 return sizeof(m_wvp) + sizeof(m_world);
		}


        operator ID3D11Buffer*()
		{
			return m_buffer.get();
		}

		operator const ID3D11Buffer*() const
		{
			return m_buffer.get();
		}

		private:

		dx11::id3d11buffer_ptr	m_buffer;

		math::matrix_float44	m_wvp;
		math::matrix_float44	m_world;
	};

	class transform_position_normal_vertex_shader
    {
		public:

		explicit transform_position_normal_vertex_shader ( ID3D11Device* device );

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

		dx11::id3d11vertexshader_ptr	m_shader;
		const void*						m_code;
		uint32_t						m_code_size;
    };

	class transform_position_normal_input_layout
	{
		public:

		transform_position_normal_input_layout( ID3D11Device* device, transform_position_normal_vertex_shader* shader);

		operator dx11::id3d11inputlayout_ptr()
		{
			return m_input_layout;
		}

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

    typedef std::tuple < transform_position_normal_vertex_shader, transform_position_normal_vertex_shader_constant_buffer,  transform_position_normal_input_layout  > transform_position_normal_vertex_pipeline;
}

#endif