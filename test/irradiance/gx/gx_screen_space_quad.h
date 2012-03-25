#ifndef __GX_SCREEN_SPACE_QUAD_H__
#define __GX_SCREEN_SPACE_QUAD_H__

#include <cstdint>

#include <math/math_matrix.h>
#include <dx11/dx11_pointers.h>

#include <gx/gx_indexed_draw_call.h>
#include <gx/gx_screen_space_vertex_shader.h>

namespace gx
{
	class render_context;

    struct screen_space_quad_render
    {
        public:

		
		void begin(ID3D11DeviceContext* device_context);
		void end(ID3D11DeviceContext* device_context);
		
		void draw_quad(ID3D11DeviceContext* device_context);
		void draw_quad(ID3D11DeviceContext* device_context, math::matrix_float44 transform);


		ID3D11VertexShader*							m_vertex_shader;
		ID3D11Buffer*								m_vertex_buffer;
		ID3D11Buffer*								m_cbuffer;
		ID3D11InputLayout*							m_input_layout;
    };

	inline void draw_screen_space_quad(	screen_space_quad_render* render, ID3D11DeviceContext* device_context )
	{
		render->begin(device_context);
		render->draw_quad(device_context);
		render->end(device_context);
	}

	inline void draw_screen_space_quad(	screen_space_quad_render* render, ID3D11DeviceContext* device_context, math::matrix_float44 transform )
	{
		render->begin(device_context);
		render->draw_quad(device_context, transform);
		render->end(device_context);
	}

	void draw_screen_space_quad(ID3D11DeviceContext* device_context, render_context* context);
	void draw_screen_space_quad(ID3D11DeviceContext* device_context, render_context* context, math::matrix_float44 transform);

	screen_space_quad_render	create_screen_space_quad_render(render_context* context);

}

#endif