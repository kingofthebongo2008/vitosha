#ifndef __dx11_EXCEPTION_H__
#define __dx11_EXCEPTION_H__

#include <exception>

namespace dx11
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

	class create_deferred_context_exception : public d3d11_exception
    {

    };

	class create_depth_stencil_view : public create_resource_exception
    {

    };

	class create_render_target_view : public create_resource_exception
    {

    };
}


#endif