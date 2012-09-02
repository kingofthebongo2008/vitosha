#ifndef __d3d11_EXCEPTION_H__
#define __d3d11_EXCEPTION_H__

#include <exception>

namespace d3d11
{
    class d3d11_exception : public std::exception
    {

    };

    class create_resource_exception : public d3d11_exception
    {

    };

    class create_device_exception : public d3d11_exception
    {

    };

    class create_dxgi_factory_exception : public d3d11_exception
    {

    };

    class create_swap_chain_exception : public d3d11_exception
    {

    };

    class create_texture_exception : public create_resource_exception
    {

    };

    class create_texture2d_exception : public create_texture_exception
    {

    };

    class create_buffer_exception : public create_resource_exception
    {

    };

    class create_view_exception : public create_resource_exception
    {

    };

    class create_shader_exception : public create_resource_exception
    {

    };

    class create_state_exception : public create_resource_exception
    {

    };

    class create_blend_state_exception : public create_state_exception
    {

    };

    class create_depth_stencil_state_exception : public create_state_exception
    {

    };

    class create_rasterizer_state_exception : public create_state_exception
    {

    };

    class create_sampler_state_exception : public create_state_exception
    {

    };

	class create_deferred_context_exception : public d3d11_exception
    {

    };

	class create_depth_stencil_view_exception : public create_resource_exception
    {

    };

	class create_render_target_view_exception : public create_resource_exception
    {

    };

	class create_resource_view_exception : public create_resource_exception
    {

    };

    class create_shader_resource_view_exception : public create_resource_exception
    {

    };

	class create_vertex_shader : public create_resource_exception
    {

    };

	class create_pixel_shader : public create_resource_exception
    {

    };

    class create_geometry_shader : public create_resource_exception
    {

    };

    class create_input_layout : public create_resource_exception
    {

    };
}


#endif