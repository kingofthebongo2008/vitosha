#include "precompiled.h"

#include <gx/gx_lambert_constant_material.h>

#include <dx11/dx11_error.h>

#include <gx/gx_draw_call_context.h>
#include <gx/gx_material_database.h>
#include <gx/gx_render_context.h>

namespace gx
{
	void lambert_constant_material::apply(draw_call_context* draw_call_context)
	{
		auto device_context = draw_call_context->m_device_context;

		m_vertex_cbuffer.set_wvp(*draw_call_context->m_wvp_matrix);
		m_vertex_cbuffer.set_world(*draw_call_context->m_world_matrix);

		m_vertex_cbuffer.flush(device_context);
		m_pixel_cbuffer.flush(device_context);

		m_vertex_cbuffer.bind_as_vertex_constant_buffer(device_context);
		m_pixel_cbuffer.bind_as_pixel_constant_buffer(device_context);

		device_context->VSSetShader(m_vertex_shader, nullptr, 0 );
		device_context->PSSetShader(m_pixel_shader, nullptr, 0 );
		device_context->IASetInputLayout(m_input_layout.get());
	}

	lambert_constant_material::lambert_constant_material ( 
										lambert_vertex_shader							vertex_shader,
										lambert_vertex_shader_constant_buffer			vertex_cbuffer,
										dx11::id3d11inputlayout_ptr						input_layout,
										lambert_constant_pixel_shader					pixel_shader,
										lambert_constant_pixel_shader_constant_buffer	pixel_cbuffer,
										math::vector_float4								diffuse_albedo
		
		) :   m_vertex_shader(vertex_shader)
			 , m_vertex_cbuffer(vertex_cbuffer)
			 , m_input_layout(input_layout)
			 , m_pixel_shader(pixel_shader)
			 , m_pixel_cbuffer(pixel_cbuffer)
	{
		m_material_id = gx::create_material_id ( this );
		m_pixel_cbuffer.set_diffuse_albedo(diffuse_albedo);
	}

	lambert_constant_material create_lambert_constant_material( render_context* context, math::vector_float4 diffuse_albedo )
	{
		return lambert_constant_material( 
								context->m_lambert_vertex_shader,
								context->m_lambert_vertex_shader_cbuffer,
								context->m_lambert_input_layout,
								context->m_lambert_constant_pixel_shader,
								context->m_lambert_pixel_cbuffer,
								diffuse_albedo);
	}
}