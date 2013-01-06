#ifndef __GX_SHADER_DATABASE_H__
#define __GX_SHADER_DATABASE_H__

#include <memory>
#include <vector>
#include <cstdint>
#include <boost/noncopyable.hpp>

#include <d3d11/d3d11_pointers.h>


#include <gx/gx_blinn_phong_shift_invariant_pixel_shader.h>

#include <gx/gx_color_pixel_shader.h>
#include <gx/gx_color_texture_pixel_shader.h>
#include <gx/gx_color_texture_channel_3_pixel_shader.h>
#include <gx/gx_copy_depth_pixel_shader.h>

#include <gx/gx_debug_view_space_depth_pixel_shader.h>
#include <gx/gx_encode_for_back_buffer_pixel_shader.h>

#include <gx/gx_lambert_pixel_shader.h>
#include <gx/gx_lambert_shift_invariant_pixel_shader.h>

#include <gx/gx_screen_space_uv_vertex_shader.h>
#include <gx/gx_transform_position_vertex_shader.h>
#include <gx/gx_transform_position_normal_vertex_shader.h>
#include <gx/gx_transform_position_normal_uv_vertex_shader.h>
#include <gx/gx_transform_position_uv_vertex_shader.h>

#include <gx/gx_transform_position_normal_uv_vertex_shader.h>
#include <gx/gx_gbuffer_dt_ng_sc_gc_pixel_shader.h>

namespace gx
{
    class shader_database : public boost::noncopyable
    {
        public:

		explicit shader_database( ID3D11Device* device );
		~shader_database();

        public:

        transform_position_vertex_shader							m_transform_position_vertex_shader;
        transform_position_vertex_shader_constant_buffer			m_transform_position_vertex_shader_cbuffer;
		transform_position_input_layout								m_transform_position_input_layout;

        transform_position_uv_vertex_shader							m_transform_position_uv_vertex_shader;
        transform_position_uv_vertex_shader_constant_buffer			m_transform_position_uv_vertex_shader_cbuffer;
		transform_position_uv_input_layout							m_transform_position_uv_input_layout;

        transform_position_normal_vertex_shader						m_transform_position_normal_vertex_shader;
        transform_position_normal_vertex_shader_constant_buffer		m_transform_position_normal_vertex_shader_cbuffer;
		transform_position_normal_input_layout						m_transform_position_normal_input_layout;

        transform_position_normal_uv_vertex_shader				    m_transform_position_normal_uv_vertex_shader;
        transform_position_normal_uv_vertex_shader_constant_buffer  m_transform_position_normal_uv_vertex_shader_cbuffer;
		transform_position_normal_uv_input_layout                   m_transform_position_normal_uv_input_layout;

        copy_depth_pixel_shader                                     m_copy_depth_pixel_shader;
		color_pixel_shader										    m_color_pixel_shader;
		color_pixel_shader_constant_buffer						    m_color_pixel_shader_cbuffer;

		color_texture_pixel_shader								    m_color_texture_pixel_shader;
        color_texture_channel_3_pixel_shader					    m_color_texture_channel_3_pixel_shader;

		encode_for_back_buffer_pixel_shader							m_encode_for_back_buffer_pixel_shader;

        debug_view_space_depth_pixel_shader                         m_debug_view_space_depth_pixel_shader;
        debug_view_space_depth_pixel_shader_constant_buffer         m_debug_view_space_depth_pixel_shader_cbuffer;

		lambert_shift_invariant_pixel_shader					    m_lambert_shift_invariant_pixel_shader;
		lambert_shift_invariant_pixel_shader_constant_buffer	    m_lambert_pixel_cbuffer;

        blinn_phong_shift_invariant_pixel_shader				    m_blinn_phong_shift_invariant_pixel_shader;
		blinn_phong_shift_invariant_pixel_shader_constant_buffer	m_blinn_phong_shift_invariant_pixel_cbuffer;

		gbuffer_dt_ng_sc_gc_pixel_shader							m_gbuffer_dt_ng_sc_gc_pixel_shader;
		gbuffer_dt_ng_sc_gc_pixel_shader_constant_buffer			m_gbuffer_dt_ng_sc_gc_pixel_shader_constant_buffer;

    };
}



#endif

