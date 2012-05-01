#include "precompiled.h"

#include <gx/gx_render_resource.h>

#include <dx11/dx11_helpers.h>

namespace gx
{
    target_render_resource create_target_render_resource(ID3D11Device* device, uint32_t width, uint32_t height, DXGI_FORMAT format)
    {
        D3D11_TEXTURE2D_DESC description = {};

		description.ArraySize = 1;
		description.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		description.CPUAccessFlags = 0;
		description.Format = format;
		description.Height = height;
		description.MipLevels = 1;
		description.MiscFlags = 0;
		description.SampleDesc.Count = 1;
        description.SampleDesc.Quality = 0;

		description.Usage = D3D11_USAGE_DEFAULT;
		description.Width = width;

        dx11::id3d11texture2d_ptr texture = dx11::create_texture_2d( device, &description);

        return target_render_resource( texture , dx11::create_render_target_view( device, texture.get() ),  dx11::create_shader_resource_view( device, texture.get() ) );
    }
 
    target_render_resource create_normal_resource(ID3D11Device* device, uint32_t width, uint32_t height)
    {
        return create_target_render_resource ( device, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT );
    }

    target_render_resource create_diffuse_resource(ID3D11Device* device, uint32_t width, uint32_t height)
    {
        return create_target_render_resource ( device, width, height, DXGI_FORMAT_R8G8B8A8_UNORM );
    }

    target_render_resource create_specular_resource(ID3D11Device* device, uint32_t width, uint32_t height)
    {
        return create_target_render_resource ( device, width, height, DXGI_FORMAT_R8G8B8A8_UNORM );
    }

    target_render_resource create_light_buffer_resource(ID3D11Device* device, uint32_t width, uint32_t height)
    {
        return create_target_render_resource ( device, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT );
    }

    depth_resource  create_depth_resource(ID3D11Device* device, uint32_t width, uint32_t height)
    {
		D3D11_TEXTURE2D_DESC description = {};

		description.ArraySize = 1;
		description.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE ;
		description.CPUAccessFlags = 0;
		description.Format = DXGI_FORMAT_R24G8_TYPELESS;
		description.Height = height;
		description.MipLevels = 1;
		description.MiscFlags = 0;
		description.SampleDesc.Count = 1;
        description.SampleDesc.Quality = 0;
		description.Usage = D3D11_USAGE_DEFAULT;
		description.Width = width;

        dx11::id3d11texture2d_ptr texture = dx11::create_texture_2d( device, &description);

        return depth_resource( texture, create_write_depth_stencil_view( device, texture.get() ),  create_depth_resource_view( device, texture.get() ) );
    }

    dx11::id3d11shaderresourceview_ptr  create_depth_resource_view( ID3D11Device* device, ID3D11Resource* resource )
    {

        D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
        srv.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        srv.Texture2D.MostDetailedMip = 0;
        srv.Texture2D.MipLevels = 1;
        srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

        return dx11::create_shader_resource_view( device, resource, &srv );
    }

    dx11::id3d11depthstencilview_ptr    create_read_depth_stencil_view( ID3D11Device* device, ID3D11Resource* resource )
    {
        D3D11_DEPTH_STENCIL_VIEW_DESC dsv = {};
        dsv.Flags = D3D11_DSV_READ_ONLY_DEPTH;
        dsv.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsv.Texture2D.MipSlice = 0;
        dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

        return dx11::create_depth_stencil_view( device, resource, &dsv );
    }

    dx11::id3d11depthstencilview_ptr    create_write_depth_stencil_view( ID3D11Device* device, ID3D11Resource* resource )
    {
        D3D11_DEPTH_STENCIL_VIEW_DESC dsv = {};

        dsv.Flags = 0;
        dsv.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsv.Texture2D.MipSlice = 0;
        dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

        return dx11::create_depth_stencil_view( device, resource, &dsv ); 
    }

    dx11::id3d11depthstencilstate_ptr   create_depth_test_enable_state( ID3D11Device* device)
    {
        D3D11_DEPTH_STENCIL_DESC dss = {};

		dss.DepthEnable = true;
		dss.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		dss.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

        return dx11::create_depth_stencil_state( device, &dss );
    }

    dx11::id3d11depthstencilstate_ptr   create_depth_test_disable_state( ID3D11Device* device)
    {
        D3D11_DEPTH_STENCIL_DESC dss = {};
        return dx11::create_depth_stencil_state( device, &dss );
    }

}

