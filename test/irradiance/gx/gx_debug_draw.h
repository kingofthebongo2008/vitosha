#ifndef __GX_DEBUG_DRAW_H__
#define __GX_DEBUG_DRAW_H__

#include <cstdint>

#include <math/math_matrix.h>
#include <math/math_graphics.h>

#include <dx11/dx11_pointers.h>

namespace gx
{
    class render_context;

    void debug_draw_diffuse_gbuffer( ID3D11DeviceContext* device_context, render_context* render_context, math::float4x4 screen_space_transform );
    void debug_draw_normal_gbuffer( ID3D11DeviceContext* device_context, render_context* render_context, math::float4x4 screen_space_transform );
    void debug_draw_specular_gbuffer( ID3D11DeviceContext* device_context, render_context* render_context, math::float4x4 screen_space_transform );
    
    void debug_draw_depth_gbuffer( ID3D11DeviceContext* device_context, render_context* render_context, math::float4x4 screen_space_transform );
    void debug_draw_gloss_gbuffer( ID3D11DeviceContext* device_context, render_context* render_context, math::float4x4 screen_space_transform );
    void debug_draw_depth_buffer( ID3D11DeviceContext* device_context, render_context* render_context,  float zn, float zf, math::float4x4 screen_space_transform );

    void debug_draw_light_buffer( ID3D11DeviceContext* device_context, render_context* render_context, math::float4x4 screen_space_transform );

    inline void debug_draw_depth_buffer( ID3D11DeviceContext* device_context, render_context* render_context,  math::float4x4 projection_matrix, math::float4x4 screen_space_transform )
    {
        std::tuple<float, float> zn_zf = math::extract_near_far( projection_matrix );
        debug_draw_depth_buffer(device_context, render_context, std::get<0>(zn_zf), std::get<1>(zn_zf), screen_space_transform);
    }

}

#endif