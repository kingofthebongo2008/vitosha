#ifndef __GX_COLOR_MATERIAL_H__
#define __GX_COLOR_MATERIAL_H__

#include <dx11/dx11_pointers.h>
#include <math/math_vector.h>

#include <gx/gx_color_pixel_shader.h>
#include <gx/gx_depth_shader.h>

namespace gx
{
	struct draw_call_context;
	class  render_context;

	class __declspec(align(16)) color_material
	{
		public:

		color_material::color_material ( 
										depth_vertex_shader					vertex_shader,
										depth_vertex_shader_constant_buffer	vertex_cbuffer,
										dx11::id3d11inputlayout_ptr			input_layout,
										color_pixel_shader					pixel_shader,
										color_pixel_shader_constant_buffer	pixel_cbuffer,
										math::vector_float4 color );

		void apply(draw_call_context* draw_call_context);

		uint16_t get_id() const
		{
			return m_material_id;
		}

		math::vector_float4 get_color() const
		{
			return m_pixel_cbuffer.get_color();
		}

		void set_color(math::vector_float4 value)
		{
			m_pixel_cbuffer.set_color(value);
		}

		private:
		depth_vertex_shader					m_vertex_shader;
		depth_vertex_shader_constant_buffer	m_vertex_cbuffer;

		dx11::id3d11inputlayout_ptr			m_input_layout;
		
		color_pixel_shader					m_pixel_shader;
		color_pixel_shader_constant_buffer	m_pixel_cbuffer;

		uint16_t							m_material_id;

	};

	color_material create_color_material( render_context* context, math::vector_float4 color );
}

#endif