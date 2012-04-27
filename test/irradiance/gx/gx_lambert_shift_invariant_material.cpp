#include "precompiled.h"

#include <gx/gx_lambert_shift_invariant_material.h>

#include <dx11/dx11_error.h>

#include <gx/gx_draw_call_context.h>
#include <gx/gx_material_database.h>
#include <gx/gx_render_context.h>

namespace gx
{
	void lambert_shift_invairant_material::apply(draw_call_context* draw_call_context)
	{
		auto device_context = draw_call_context->m_device_context;

        std::get<1>(m_vertex_pipeline).set_wvp(*draw_call_context->m_wvp_matrix);
        std::get<1>(m_vertex_pipeline).set_normal_transform( math::mul( *draw_call_context->m_view_matrix, *draw_call_context->m_world_matrix ));
        std::get<1>(m_vertex_pipeline).flush(device_context);
        std::get<1>(m_vertex_pipeline).bind_as_vertex_constant_buffer(device_context);

		m_pixel_cbuffer.flush(device_context);
		m_pixel_cbuffer.bind_as_pixel_constant_buffer(device_context);

		dx11::vs_set_shader( device_context, std::get<0>(m_vertex_pipeline) );
		dx11::ps_set_shader( device_context, m_pixel_shader );
		device_context->IASetInputLayout( std::get<2>(m_vertex_pipeline) );

	}

	lambert_shift_invairant_material::lambert_shift_invairant_material 
        ( 
                                        transform_position_normal_uv_vertex_pipeline            vertex_pipeline,
                                        lambert_shift_invariant_pixel_shader					pixel_shader,
										lambert_shift_invariant_pixel_shader_constant_buffer	pixel_cbuffer,
										math::vector_float4								diffuse_albedo
		
		) :   m_vertex_pipeline(vertex_pipeline)
			 , m_pixel_shader(pixel_shader)
			 , m_pixel_cbuffer(pixel_cbuffer)
	{
		m_material_id = gx::create_material_id ( this );
		m_pixel_cbuffer.set_diffuse_albedo(diffuse_albedo);
	}

	lambert_shift_invairant_material create_lambert_shift_invairant_material( render_context* context, math::vector_float4 diffuse_albedo )
	{
		return lambert_shift_invairant_material(
                                std::make_tuple< transform_position_normal_uv_vertex_shader, transform_position_normal_uv_vertex_shader_constant_buffer,  transform_position_normal_uv_input_layout > 
                                ( 
                                    context->m_transform_position_normal_uv_vertex_shader,
                                    context->m_transform_position_normal_uv_vertex_shader_cbuffer,
                                    context->m_transform_position_normal_uv_input_layout
                                ) ,
								context->m_lambert_shift_invariant_pixel_shader,
								context->m_lambert_pixel_cbuffer,
								diffuse_albedo);
	}
}