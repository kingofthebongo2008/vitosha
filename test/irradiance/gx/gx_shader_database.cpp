#include "precompiled.h"


#include <gx/gx_shader_database.h>
#include <gx/gx_constant_buffer_helper.h>

namespace gx
{
    shader_database::shader_database(ID3D11Device* device) : 
          m_transform_position_vertex_shader( device )
        , m_transform_position_vertex_shader_cbuffer( device )
        , m_transform_position_input_layout ( device, &m_transform_position_vertex_shader )

        , m_transform_position_uv_vertex_shader( device )
        , m_transform_position_uv_vertex_shader_cbuffer( device )
		, m_transform_position_uv_input_layout( device, &m_transform_position_uv_vertex_shader )

        , m_transform_position_normal_vertex_shader( device )
        , m_transform_position_normal_vertex_shader_cbuffer( device )
        , m_transform_position_normal_input_layout ( device, &m_transform_position_normal_vertex_shader )

        , m_transform_position_normal_uv_vertex_shader( device )
        , m_transform_position_normal_uv_vertex_shader_cbuffer( device )
		, m_transform_position_normal_uv_input_layout ( device, &m_transform_position_normal_uv_vertex_shader )

        , m_copy_depth_pixel_shader ( device )
		, m_color_pixel_shader ( device )
		, m_color_pixel_shader_cbuffer ( device )
		, m_color_texture_pixel_shader (  device )
        , m_color_texture_channel_3_pixel_shader (  device )

		, m_encode_for_back_buffer_pixel_shader ( device )
        
        , m_debug_view_space_depth_pixel_shader (  device )
        , m_debug_view_space_depth_pixel_shader_cbuffer (  device )

		, m_lambert_shift_invariant_pixel_shader(device )
		, m_lambert_pixel_cbuffer( device )
        , m_blinn_phong_shift_invariant_pixel_shader( device  )
		, m_blinn_phong_shift_invariant_pixel_cbuffer( device )

		, m_gbuffer_dt_ng_sc_gc_pixel_shader( device ) 
		, m_gbuffer_dt_ng_sc_gc_pixel_shader_constant_buffer ( device )
    {

    }

	shader_database::~shader_database()
	{

	}
}

