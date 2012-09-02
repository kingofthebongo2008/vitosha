#ifndef __dx11_HELPERS_H__
#define __dx11_HELPERS_H__

#include <cstdint>

#include <dx11/dx11_pointers.h>
#include <dx11/dx11_error.h>

namespace dx11
{
    inline id3d11blendstate_ptr   create_blend_state(ID3D11Device* device,   const D3D11_BLEND_DESC* description )
    {
        dx11::id3d11blendstate_ptr result;
        dx11::throw_if_failed< dx11::create_blend_state_exception> ( device->CreateBlendState(description, dx11::get_pointer(result) ) );
        return result;
    }
    
    inline id3d11depthstencilstate_ptr    create_depth_stencil_state(ID3D11Device* device,  const D3D11_DEPTH_STENCIL_DESC* description )
    {
        id3d11depthstencilstate_ptr result;
        dx11::throw_if_failed< dx11::create_depth_stencil_state_exception> ( device->CreateDepthStencilState( description, dx11::get_pointer(result) ) );
        return result;
    }

    inline id3d11depthstencilview_ptr    create_depth_stencil_view(ID3D11Device* device, ID3D11Resource* resource,  const D3D11_DEPTH_STENCIL_VIEW_DESC* description )
    {
        id3d11depthstencilview_ptr result;
        dx11::throw_if_failed< dx11::create_depth_stencil_view_exception> ( device->CreateDepthStencilView(resource, description, dx11::get_pointer(result) ) );
        return result;
    }

    inline id3d11rasterizerstate_ptr      create_raster_state(ID3D11Device* device, const D3D11_RASTERIZER_DESC* description )
    {
        id3d11rasterizerstate_ptr result;
        dx11::throw_if_failed< dx11::create_rasterizer_state_exception> (device->CreateRasterizerState(description, dx11::get_pointer(result) ) );
        return result;
    }
    
    inline id3d11rendertargetview_ptr    create_render_target_view(ID3D11Device* device, ID3D11Resource* resource, const D3D11_RENDER_TARGET_VIEW_DESC* description )
    {
        id3d11rendertargetview_ptr result;
        dx11::throw_if_failed< dx11::create_render_target_view_exception> (device->CreateRenderTargetView(resource, description, dx11::get_pointer(result) ) );
        return result;
    }

    inline id3d11rendertargetview_ptr    create_render_target_view(ID3D11Device* device, ID3D11Resource* resource )
    {
        id3d11rendertargetview_ptr result;
        dx11::throw_if_failed< dx11::create_render_target_view_exception> (device->CreateRenderTargetView(resource, nullptr, dx11::get_pointer(result) ) );
        return result;
    }

    inline id3d11samplerstate_ptr        create_sampler_state(ID3D11Device* device, const D3D11_SAMPLER_DESC* description )
    {
        id3d11samplerstate_ptr result;
        dx11::throw_if_failed< dx11::create_sampler_state_exception> (device->CreateSamplerState(description, dx11::get_pointer(result) ) );
        return result;
    }

    inline id3d11shaderresourceview_ptr        create_shader_resource_view(ID3D11Device* device, ID3D11Resource* resource , const D3D11_SHADER_RESOURCE_VIEW_DESC* description)
    {
        id3d11shaderresourceview_ptr result;
        dx11::throw_if_failed< dx11::create_shader_resource_view_exception> ( device->CreateShaderResourceView( resource, description, dx11::get_pointer(result) )  );
        return result;
    }

    inline id3d11shaderresourceview_ptr create_shader_resource_view(ID3D11Device* device, ID3D11Resource* resource)
    {
        return create_shader_resource_view(device, resource, nullptr);
    }

    inline id3d11texture2d_ptr           create_texture_2d(ID3D11Device* device,  const D3D11_TEXTURE2D_DESC* description, const D3D11_SUBRESOURCE_DATA* initial_data )
    {
        id3d11texture2d_ptr result;
        dx11::throw_if_failed< dx11::create_texture2d_exception> (device->CreateTexture2D(description, initial_data, dx11::get_pointer(result) ) );
        return result;
    }

    inline id3d11texture2d_ptr           create_texture_2d(ID3D11Device* device,  const D3D11_TEXTURE2D_DESC* description)
    {
        return create_texture_2d(device, description, nullptr);
    }

    id3d11buffer_ptr              create_constant_buffer(ID3D11Device* device, size_t size);
	id3d11buffer_ptr              create_default_index_buffer(ID3D11Device* device, const void* initial_data, size_t size );
	id3d11buffer_ptr              create_default_vertex_buffer(ID3D11Device* device, const void* initial_data, size_t size );

    id3d11buffer_ptr              create_stream_out_vertex_buffer(ID3D11Device* device, const void* initial_data, size_t size );

	id3d11buffer_ptr              create_dynamic_index_buffer(ID3D11Device* device, const void* initial_data, size_t size );
	id3d11buffer_ptr              create_dynamic_vertex_buffer(ID3D11Device* device, const void* initial_data, size_t size );

	id3d11buffer_ptr              create_immutable_index_buffer(ID3D11Device* device, const void* initial_data, size_t size );
	id3d11buffer_ptr              create_immutable_vertex_buffer(ID3D11Device* device, const void* initial_data, size_t size );

	struct d3d11_buffer_scope_lock
	{
		d3d11_buffer_scope_lock( ID3D11DeviceContext* context, ID3D11Buffer* buffer) : m_context(context), m_buffer(buffer)
		{
			dx11::throw_if_failed<dx11::d3d11_exception>(context->Map( buffer, 0,  D3D11_MAP_WRITE_DISCARD, 0, &m_mapped_resource) ) ;
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

