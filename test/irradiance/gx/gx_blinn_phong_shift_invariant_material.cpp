#include "precompiled.h"

#include <gx/gx_blinn_phong_shift_invariant_material.h>

#include <d3d11/d3d11_error.h>

#include <gx/gx_draw_call_context.h>
#include <gx/gx_material_database.h>
#include <gx/gx_shader_database.h>

namespace gx
{
	void blinn_phong_shift_invairant_material::apply(draw_call_context* draw_call_context)
	{
		auto device_context = draw_call_context->m_device_context;

        std::get<1>(m_vertex_pipeline).set_w(*draw_call_context->m_world_matrix);
        std::get<1>(m_vertex_pipeline).set_normal_transform( math::transpose( math::inverse( math::mul( *draw_call_context->m_world_matrix, *draw_call_context->m_view_matrix ))));


        std::get<1>(m_vertex_pipeline).flush(device_context);
        std::get<1>(m_vertex_pipeline).bind_as_vertex_constant_buffer(device_context);

		m_pixel_cbuffer.flush(device_context);
    	m_pixel_cbuffer.bind_as_pixel_constant_buffer(device_context);

        d3d11::vs_set_shader(device_context, std::get<0>(m_vertex_pipeline) );
		d3d11::ps_set_shader(device_context, m_pixel_shader );
		device_context->IASetInputLayout( std::get<2>(m_vertex_pipeline) );

	}

	blinn_phong_shift_invairant_material::blinn_phong_shift_invairant_material
        ( 
                const transform_position_normal_uv_vertex_pipeline              vertex_pipeline,
                const blinn_phong_shift_invariant_pixel_shader					pixel_shader,
                const blinn_phong_shift_invariant_pixel_shader_constant_buffer	pixel_cbuffer,
				math::float4                                         kd,
				math::float4                                         ks_gloss
		
		) :  
             m_vertex_pipeline(vertex_pipeline)
			 , m_pixel_shader(pixel_shader)
			 , m_pixel_cbuffer(pixel_cbuffer)
	{
		m_material_id = gx::create_material_id ( this );

        set_kd(kd);
        set_ks_gloss(ks_gloss);
	}

	blinn_phong_shift_invairant_material create_blinn_phong_shift_invairant_material( const shader_database* context, math::float4 kd, math::float4 ks_gloss )
	{
			std::tuple< transform_position_normal_uv_vertex_shader, transform_position_normal_uv_vertex_shader_constant_buffer, transform_position_normal_uv_input_layout >
			tuple(
			        std::move(context->m_transform_position_normal_uv_vertex_shader),
                    std::move(context->m_transform_position_normal_uv_vertex_shader_cbuffer),
                    std::move(context->m_transform_position_normal_uv_input_layout)
            
			);

		    return blinn_phong_shift_invairant_material(
								std::move(tuple),
								context->m_blinn_phong_shift_invariant_pixel_shader,
								context->m_blinn_phong_shift_invariant_pixel_cbuffer,
								kd,
                                ks_gloss
                                );
	}
}