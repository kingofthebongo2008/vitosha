#ifndef __GX_LIGHT_DIRECTIONAL_VERTEX_SHADER_H__
#define __GX_LIGHT_DIRECTIONAL_VERTEX_SHADER_H__

#include <cstdint>

#include <d3d11/d3d11_pointers.h>

#include <math/math_matrix.h>

#include <gx/gx_constant_buffer_helper.h>

namespace gx
{
	class light_directional_vertex_shader
    {
		public:

		explicit light_directional_vertex_shader ( ID3D11Device* device );

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

    class light_directional_vertex_shader_input_layout
	{
		public:

		light_directional_vertex_shader_input_layout( ID3D11Device* device, light_directional_vertex_shader* shader);

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

    typedef std::tuple < light_directional_vertex_shader, light_directional_vertex_shader_input_layout  > light_directional_vertex_pipeline;

    inline light_directional_vertex_pipeline create_light_directional_vertex_pipeline( ID3D11Device* device )
    {
        light_directional_vertex_shader shader(device);
        return std::make_tuple<  light_directional_vertex_shader, light_directional_vertex_shader_input_layout >
            (
                std::move(shader),
                light_directional_vertex_shader_input_layout(device, &shader)
            ); 
    }
}

#endif