#ifndef __GX_RENDER_RESOURCE_H__
#define __GX_RENDER_RESOURCE_H__

#include <cstdint>

#include <dx11/dx11_pointers.h>

namespace gx
{
    class target_render_resource
    {
        public:

        target_render_resource (    
                                    dx11::id3d11texture2d_ptr			resource,
                                    dx11::id3d11rendertargetview_ptr    resource_rtv,
                                    dx11::id3d11shaderresourceview_ptr	resource_srv
                                    
                                ) : m_resource(resource) , m_resource_rtv(resource_rtv), m_resource_srv(resource_srv)
        {

        }

        inline operator ID3D11Texture2D* ()
        {
            return m_resource.get();
        }

        inline operator const ID3D11Texture2D* () const
        {
            return m_resource.get();
        }

        inline operator ID3D11RenderTargetView* ()
        {
            return m_resource_rtv.get();
        }

        inline operator const ID3D11RenderTargetView* () const
        {
            return m_resource_rtv.get();
        }

        inline operator ID3D11ShaderResourceView* ()
        {
            return m_resource_srv.get();
        }

        inline operator const ID3D11ShaderResourceView* () const
        {
            return m_resource_srv.get();
        }

        dx11::id3d11texture2d_ptr			m_resource;
        dx11::id3d11rendertargetview_ptr    m_resource_rtv;
        dx11::id3d11shaderresourceview_ptr	m_resource_srv;

    };

    class depth_resource
    {
        public:

        depth_resource (    
                                    dx11::id3d11texture2d_ptr			resource,
                                    dx11::id3d11depthstencilview_ptr    resource_dsv,
                                    dx11::id3d11shaderresourceview_ptr	resource_srv
                                ) : m_resource(resource) , m_resource_dsv(resource_dsv), m_resource_srv(resource_srv)
        {

        }

        inline operator ID3D11Texture2D* ()
        {
            return m_resource.get();
        }

        inline operator const ID3D11Texture2D* () const
        {
            return m_resource.get();
        }

        inline operator ID3D11DepthStencilView* ()
        {
            return m_resource_dsv.get();
        }

        inline operator const ID3D11DepthStencilView* () const
        {
            return m_resource_dsv.get();
        }

        inline operator ID3D11ShaderResourceView* ()
        {
            return m_resource_srv.get();
        }

        inline operator const ID3D11ShaderResourceView* () const
        {
            return m_resource_srv.get();
        }

        dx11::id3d11texture2d_ptr			m_resource;
        dx11::id3d11depthstencilview_ptr    m_resource_dsv;
        dx11::id3d11shaderresourceview_ptr	m_resource_srv;

    };


    dx11::id3d11depthstencilview_ptr    create_read_depth_stencil_view( ID3D11Device* device, ID3D11Resource* resource );
    dx11::id3d11depthstencilview_ptr    create_write_depth_stencil_view( ID3D11Device* device, ID3D11Resource* resource );
 
    dx11::id3d11shaderresourceview_ptr  create_depth_resource_view( ID3D11Device* device, ID3D11Resource* resource );

    dx11::id3d11depthstencilstate_ptr   create_depth_test_enable_state( ID3D11Device* device);
    dx11::id3d11depthstencilstate_ptr   create_depth_test_disable_state( ID3D11Device* device );


    target_render_resource              create_target_render_resource(ID3D11Device* device, uint32_t width, uint32_t height, DXGI_FORMAT format);
    target_render_resource              create_normal_resource(ID3D11Device* device, uint32_t width, uint32_t height);
    target_render_resource              create_diffuse_resource(ID3D11Device* device, uint32_t width, uint32_t height);
    target_render_resource              create_specular_resource(ID3D11Device* device, uint32_t width, uint32_t height);

    target_render_resource              create_light_buffer_resource(ID3D11Device* device, uint32_t width, uint32_t height);

    dx11::id3d11blendstate_ptr          create_opaque_blend_state( ID3D11Device* device );
    dx11::id3d11blendstate_ptr          create_additive_blend_state( ID3D11Device* device );
    dx11::id3d11blendstate_ptr          create_gbuffer_opaque_blend_state( ID3D11Device* device );
    

    depth_resource                      create_depth_resource(ID3D11Device* device, uint32_t width, uint32_t height);

    dx11::id3d11samplerstate_ptr              create_default_sampler_state( ID3D11Device* device );
    dx11::id3d11samplerstate_ptr              create_point_sampler_state( ID3D11Device* device );


    dx11::id3d11rasterizerstate_ptr		create_cull_back_rasterizer_state( ID3D11Device* device );
}



#endif

