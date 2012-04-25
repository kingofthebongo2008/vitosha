#include "precompiled.h"

#include <dx11/dx11_helpers.h>

#include <gx/gx_debug_draw.h>

#include <gx/gx_render_context.h>
#include <gx/gx_screen_space_quad.h>

namespace gx
{
    void debug_draw_diffuse_gbuffer( ID3D11DeviceContext* device_context, render_context* render_context, math::matrix_float44 screen_space_transform )
    {
		dx11::ps_set_shader(device_context, render_context->m_color_texture_pixel_shader );
        dx11::ps_set_shader_resources( device_context,  render_context->m_gbuffer_render_data.m_render_set.m_diffuse_view.get() );
        draw_screen_space_quad(device_context, render_context, screen_space_transform);
    }

    void debug_draw_normal_gbuffer( ID3D11DeviceContext* device_context, render_context* render_context, math::matrix_float44 screen_space_transform )
    {
        dx11::ps_set_shader(device_context, render_context->m_color_texture_pixel_shader );
        dx11::ps_set_shader_resources( device_context,  render_context->m_gbuffer_render_data.m_render_set.m_normal_depth_view.get() );
        draw_screen_space_quad(device_context, render_context, screen_space_transform);
    }

    void debug_draw_specular_gbuffer( ID3D11DeviceContext* device_context, render_context* render_context, math::matrix_float44 screen_space_transform )
    {
        dx11::ps_set_shader(device_context, render_context->m_color_texture_pixel_shader );
        dx11::ps_set_shader_resources( device_context ,  render_context->m_gbuffer_render_data.m_render_set.m_specular_view.get() );
        draw_screen_space_quad(device_context, render_context, screen_space_transform);
    }

    void debug_draw_depth_gbuffer( ID3D11DeviceContext* device_context, render_context* render_context, math::matrix_float44 screen_space_transform )
    {
        dx11::ps_set_shader(device_context, render_context->m_color_texture_channel_3_pixel_shader );
        dx11::ps_set_shader_resources( device_context,  render_context->m_gbuffer_render_data.m_render_set.m_normal_depth_view.get() );
        draw_screen_space_quad(device_context, render_context, screen_space_transform);
    }

    void debug_draw_gloss_gbuffer( ID3D11DeviceContext* device_context, render_context* render_context, math::matrix_float44 screen_space_transform )
    {
        dx11::ps_set_shader(device_context, render_context->m_color_texture_channel_3_pixel_shader );
        dx11::ps_set_shader_resources( device_context,  render_context->m_gbuffer_render_data.m_render_set.m_specular_view.get() );
        draw_screen_space_quad(device_context, render_context, screen_space_transform);
    }

    void debug_draw_depth_buffer( ID3D11DeviceContext* device_context, render_context* render_context,  float zn, float zf, math::matrix_float44 screen_space_transform )
    {
        debug_view_space_depth_pixel_shader_constant_buffer& cbuffer = render_context->m_debug_view_space_depth_pixel_shader_cbuffer;

        cbuffer.set_zn_zf ( zn, zf );
        cbuffer.flush(device_context);
        cbuffer.bind_as_pixel_constant_buffer(device_context);

        dx11::ps_set_shader(device_context, render_context->m_debug_view_space_depth_pixel_shader );
        dx11::ps_set_shader_resources( device_context,  render_context->m_light_buffer_render_data.m_render_set.m_depth_stencil_view.get() );

        draw_screen_space_quad(device_context, render_context, screen_space_transform);
    }

    void debug_draw_light_buffer( ID3D11DeviceContext* device_context, render_context* render_context, math::matrix_float44 screen_space_transform )
    {
        dx11::ps_set_shader(device_context, render_context->m_color_texture_pixel_shader );
        dx11::ps_set_shader_resources( device_context ,  render_context->m_light_buffer_render_data.m_render_set.m_light_buffer_view.get() );
        draw_screen_space_quad(device_context, render_context, screen_space_transform);

    }
}