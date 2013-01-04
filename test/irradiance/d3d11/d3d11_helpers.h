#ifndef __d3d11_HELPERS_H__
#define __d3d11_HELPERS_H__

#include <cstdint>

#include <d3d11/dxgi_helpers.h>
#include <d3d11/d3d11_pointers.h>
#include <d3d11/d3d11_error.h>

namespace d3d11
{
	inline D3D11_RENDER_TARGET_VIEW_DESC create_srgb_render_target_view_desc(ID3D11Texture2D* texture)
	{
		D3D11_TEXTURE2D_DESC desc = {};
		texture->GetDesc(&desc);

		D3D11_RENDER_TARGET_VIEW_DESC result =
				{
					dxgi::format_2_srgb_format( desc.Format ),
					D3D11_RTV_DIMENSION_TEXTURE2D,
					0,
					0
				};

		return result;
	}

	inline D3D11_SHADER_RESOURCE_VIEW_DESC create_srgb_shader_resource_view_desc(ID3D11Texture2D* texture)
	{
		D3D11_TEXTURE2D_DESC desc = {};
		texture->GetDesc(&desc);

		D3D11_SHADER_RESOURCE_VIEW_DESC result =
				{
					dxgi::format_2_srgb_format( desc.Format ),
					D3D11_SRV_DIMENSION_TEXTURE2D,
					static_cast<uint32_t> (0),
					desc.MipLevels
				};

		return result;
	}


    inline iblendstate_ptr   create_blend_state(ID3D11Device* device,   const D3D11_BLEND_DESC* description )
    {
        d3d11::iblendstate_ptr result;
        dx::throw_if_failed< d3d11::create_blend_state_exception> ( device->CreateBlendState(description, dx::get_pointer(result) ) );
        return result;
    }
    
    inline idepthstencilstate_ptr    create_depth_stencil_state(ID3D11Device* device,  const D3D11_DEPTH_STENCIL_DESC* description )
    {
        idepthstencilstate_ptr result;
        dx::throw_if_failed< d3d11::create_depth_stencil_state_exception> ( device->CreateDepthStencilState( description, dx::get_pointer(result) ) );
        return result;
    }

    inline idepthstencilview_ptr    create_depth_stencil_view(ID3D11Device* device, ID3D11Resource* resource,  const D3D11_DEPTH_STENCIL_VIEW_DESC* description )
    {
        idepthstencilview_ptr result;
        dx::throw_if_failed< d3d11::create_depth_stencil_view_exception> ( device->CreateDepthStencilView(resource, description, dx::get_pointer(result) ) );
        return result;
    }

    inline irasterizerstate_ptr      create_raster_state(ID3D11Device* device, const D3D11_RASTERIZER_DESC* description )
    {
        irasterizerstate_ptr result;
        dx::throw_if_failed< d3d11::create_rasterizer_state_exception> (device->CreateRasterizerState(description, dx::get_pointer(result) ) );
        return result;
    }
    
    inline id3d11rendertargetview_ptr    create_render_target_view(ID3D11Device* device, ID3D11Resource* resource, const D3D11_RENDER_TARGET_VIEW_DESC* description )
    {
        id3d11rendertargetview_ptr result;
        dx::throw_if_failed< d3d11::create_render_target_view_exception> (device->CreateRenderTargetView(resource, description, dx::get_pointer(result) ) );
        return result;
    }

    inline id3d11rendertargetview_ptr    create_render_target_view(ID3D11Device* device, ID3D11Resource* resource )
    {
		return create_render_target_view(device, resource, nullptr);
    }

	inline id3d11rendertargetview_ptr    create_srgb_render_target_view(ID3D11Device* device, ID3D11Texture2D* resource )
    {
		D3D11_RENDER_TARGET_VIEW_DESC desc = create_srgb_render_target_view_desc(resource);
		return create_render_target_view(device, resource, &desc);
    }

    inline isamplerstate_ptr        create_sampler_state(ID3D11Device* device, const D3D11_SAMPLER_DESC* description )
    {
        isamplerstate_ptr result;
        dx::throw_if_failed< d3d11::create_sampler_state_exception> (device->CreateSamplerState(description, dx::get_pointer(result) ) );
        return result;
    }

    inline ishaderresourceview_ptr        create_shader_resource_view(ID3D11Device* device, ID3D11Resource* resource , const D3D11_SHADER_RESOURCE_VIEW_DESC* description)
    {
        ishaderresourceview_ptr result;
        dx::throw_if_failed< d3d11::create_shader_resource_view_exception> ( device->CreateShaderResourceView( resource, description, dx::get_pointer(result) )  );
        return result;
    }

    inline ishaderresourceview_ptr create_shader_resource_view(ID3D11Device* device, ID3D11Resource* resource)
    {
        return create_shader_resource_view(device, resource, nullptr);
    }

	inline ishaderresourceview_ptr create_srgb_shader_resource_view(ID3D11Device* device, ID3D11Texture2D* resource)
    {
		D3D11_SHADER_RESOURCE_VIEW_DESC desc = create_srgb_shader_resource_view_desc(resource);
        return create_shader_resource_view(device, resource, &desc);
    }

    inline itexture2d_ptr           create_texture_2d(ID3D11Device* device,  const D3D11_TEXTURE2D_DESC* description, const D3D11_SUBRESOURCE_DATA* initial_data )
    {
        itexture2d_ptr result;
        dx::throw_if_failed< d3d11::create_texture2d_exception> (device->CreateTexture2D(description, initial_data, dx::get_pointer(result) ) );
        return result;
    }

    inline itexture2d_ptr           create_texture_2d(ID3D11Device* device,  const D3D11_TEXTURE2D_DESC* description)
    {
        return create_texture_2d(device, description, nullptr);
    }

    ibuffer_ptr              create_constant_buffer(ID3D11Device* device, size_t size);
	ibuffer_ptr              create_default_index_buffer(ID3D11Device* device, const void* initial_data, size_t size );
	ibuffer_ptr              create_default_vertex_buffer(ID3D11Device* device, const void* initial_data, size_t size );

    ibuffer_ptr              create_stream_out_vertex_buffer(ID3D11Device* device, const void* initial_data, size_t size );

	ibuffer_ptr              create_dynamic_index_buffer(ID3D11Device* device, const void* initial_data, size_t size );
	ibuffer_ptr              create_dynamic_vertex_buffer(ID3D11Device* device, const void* initial_data, size_t size );

	ibuffer_ptr              create_immutable_index_buffer(ID3D11Device* device, const void* initial_data, size_t size );
	ibuffer_ptr              create_immutable_vertex_buffer(ID3D11Device* device, const void* initial_data, size_t size );

	struct d3d11_buffer_scope_lock
	{
		d3d11_buffer_scope_lock( ID3D11DeviceContext* context, ID3D11Buffer* buffer) : m_context(context), m_buffer(buffer)
		{
			dx::throw_if_failed<d3d11::d3d11_exception>(context->Map( buffer, 0,  D3D11_MAP_WRITE_DISCARD, 0, &m_mapped_resource) ) ;
		}

		~d3d11_buffer_scope_lock()
		{
			m_context->Unmap(m_buffer, 0);
		}

		D3D11_MAPPED_SUBRESOURCE	m_mapped_resource;
		ID3D11DeviceContext*		m_context;
		ID3D11Buffer*				m_buffer;
	};

    inline void gs_set_shader_resources(ID3D11DeviceContext* device_context, ID3D11ShaderResourceView * const * shader_resource_view)
    {
        device_context->GSSetShaderResources( 0, 1, shader_resource_view);
    }

    inline void gs_set_shader_resources(ID3D11DeviceContext* device_context, uint32_t num_views, ID3D11ShaderResourceView * const * shader_resource_view)
    {
        device_context->GSSetShaderResources( 0, num_views, shader_resource_view);
    }

    inline void gs_set_shader(ID3D11DeviceContext* device_context, ID3D11GeometryShader * vertex_shader )
    {
        device_context->GSSetShader( vertex_shader, nullptr, 0) ;
    }

    inline void vs_set_shader_resources(ID3D11DeviceContext* device_context, ID3D11ShaderResourceView * const * shader_resource_view)
    {
        device_context->VSSetShaderResources( 0, 1, shader_resource_view);
    }

    inline void vs_set_shader_resources(ID3D11DeviceContext* device_context, uint32_t num_views, ID3D11ShaderResourceView * const * shader_resource_view)
    {
        device_context->VSSetShaderResources( 0, num_views, shader_resource_view);
    }

    inline void vs_set_shader(ID3D11DeviceContext* device_context, const ID3D11VertexShader * const vertex_shader )
    {
        device_context->VSSetShader( const_cast< ID3D11VertexShader* > (vertex_shader), nullptr, 0) ;
    }

    inline void ps_set_shader_resources(ID3D11DeviceContext* device_context, uint32_t view_count, ID3D11ShaderResourceView * const shader_resource_view [] )
    {
        device_context->PSSetShaderResources( 0, view_count , &shader_resource_view[0]);
    }

    inline void ps_set_shader_resources(ID3D11DeviceContext* device_context, ID3D11ShaderResourceView * const shader_resource_view)
    {
        device_context->PSSetShaderResources( 0, 1, &shader_resource_view);
    }

    inline void ps_set_shader_resources(ID3D11DeviceContext* device_context, ID3D11ShaderResourceView * const resource_0, ID3D11ShaderResourceView * const resource_1)
    {
        ID3D11ShaderResourceView * const resources[2] = { resource_0 , resource_1};
        ps_set_shader_resources(device_context, 2, resources);
    }

    inline void ps_set_shader(ID3D11DeviceContext* device_context, const ID3D11PixelShader * const pixel_shader )
    {
        device_context->PSSetShader( const_cast<ID3D11PixelShader *> (pixel_shader), nullptr, 0) ;
    }
}


#endif

