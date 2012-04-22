#include "precompiled.h"

#include <gx/gx_color_material.h>

#include <dx11/dx11_error.h>

#include <gx/gx_draw_call_context.h>
#include <gx/gx_material_database.h>
#include <gx/gx_render_context.h>

namespace gx
{
	void color_material::apply(draw_call_context* draw_call_context)
	{
		ID3D11DeviceContext* device_context = draw_call_context->m_device_context;

		m_vertex_cbuffer.update(device_context, draw_call_context->m_wvp_matrix);
		m_pixel_cbuffer.flush(device_context);

		m_vertex_cbuffer.bind_as_vertex_constant_buffer(device_context);
		m_pixel_cbuffer.bind_as_pixel_constant_buffer(device_context);

		device_context->VSSetShader(m_vertex_shader, nullptr, 0 );
		device_context->PSSetShader(m_pixel_shader, nullptr, 0 );
		device_context->IASetInputLayout(m_input_layout);
	}

	color_material::color_material ( 
										transform_position_vertex_shader					vertex_shader,
										transform_position_vertex_shader_constant_buffer	vertex_cbuffer,
										transform_position_input_layout                     input_layout,
										color_pixel_shader					pixel_shader,
										color_pixel_shader_constant_buffer	pixel_cbuffer,
										math::vector_float4 color
		
		) :   m_vertex_shader(vertex_shader)
			 , m_vertex_cbuffer(vertex_cbuffer)
			 , m_input_layout(input_layout)
			 , m_pixel_shader(pixel_shader)
			 , m_pixel_cbuffer(pixel_cbuffer)
	{
		m_material_id = gx::create_material_id ( this );
		m_pixel_cbuffer.set_color(color);
	}

	color_material create_color_material( render_context* context, math::vector_float4 color )
	{
		return color_material( 
								context->m_depth_render_data.m_depth_vertex_shader,
								context->m_depth_render_data.m_depth_constant_buffer,
								context->m_depth_render_data.m_input_layout,
								context->m_color_pixel_shader,
								context->m_color_pixel_shader_cbuffer,
								color);
	}
}