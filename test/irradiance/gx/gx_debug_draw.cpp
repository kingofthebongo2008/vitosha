#include "precompiled.h"

#include <d3d11/d3d11_helpers.h>

#include <gx/gx_debug_draw.h>

#include <gx/gx_render_context.h>
#include <gx/gx_screen_space_quad.h> 

namespace gx
{
    void debug_draw_diffuse_gbuffer( ID3D11DeviceContext* device_context, render_context* render_context, math::float4x4 screen_space_transform )
    {
		d3d11::ps_set_shader(device_context, render_context->get_shader_database()->m_color_texture_pixel_shader );
        d3d11::ps_set_shader_resources( device_context,  render_context->m_gbuffer_render_data.m_diffuse );
        draw_screen_space_quad(device_context, render_context, screen_space_transform);
    }

    void debug_draw_normal_gbuffer( ID3D11DeviceContext* device_context, render_context* render_context, math::float4x4 screen_space_transform )
    {
        d3d11::ps_set_shader(device_context, render_context->get_shader_database()->m_color_texture_pixel_shader );
        d3d11::ps_set_shader_resources( device_context,  render_context->m_gbuffer_render_data.m_normal );
        draw_screen_space_quad(device_context, render_context, screen_space_transform);
    }

    void debug_draw_specular_gbuffer( ID3D11DeviceContext* device_context, render_context* render_context, math::float4x4 screen_space_transform )
    {
        d3d11::ps_set_shader(device_context, render_context->get_shader_database()->m_color_texture_pixel_shader );
        d3d11::ps_set_shader_resources( device_context ,  render_context->m_gbuffer_render_data.m_specular );
        draw_screen_space_quad(device_context, render_context, screen_space_transform);
    }

    void debug_draw_gloss_gbuffer( ID3D11DeviceContext* device_context, render_context* render_context, math::float4x4 screen_space_transform )
    {
        d3d11::ps_set_shader(device_context, render_context->get_shader_database()->m_color_texture_channel_3_pixel_shader );
        d3d11::ps_set_shader_resources( device_context,  render_context->m_gbuffer_render_data.m_specular );
        draw_screen_space_quad(device_context, render_context, screen_space_transform);
    }

    void debug_draw_depth_buffer( ID3D11DeviceContext* device_context, render_context* render_context,  float zn, float zf, math::float4x4 screen_space_transform )
    {
        debug_view_space_depth_pixel_shader_constant_buffer& cbuffer = render_context->get_shader_database()->m_debug_view_space_depth_pixel_shader_cbuffer;

        cbuffer.set_zn_zf ( zn, zf );
        cbuffer.flush(device_context);
        cbuffer.bind_as_pixel_constant_buffer(device_context);

        d3d11::ps_set_shader(device_context, render_context->get_shader_database()->m_debug_view_space_depth_pixel_shader );
        d3d11::ps_set_shader_resources( device_context,  render_context->m_debug_render_data.m_depth_buffer_copy );
        draw_screen_space_quad(device_context, render_context, screen_space_transform);
    }

    void debug_draw_light_buffer( ID3D11DeviceContext* device_context, render_context* render_context, math::float4x4 screen_space_transform )
    {
        d3d11::ps_set_shader(device_context, render_context->get_shader_database()->m_color_texture_pixel_shader );
        d3d11::ps_set_shader_resources( device_context ,  render_context->m_light_buffer_render_data.m_light_buffer );
        draw_screen_space_quad(device_context, render_context, screen_space_transform);
    }
}