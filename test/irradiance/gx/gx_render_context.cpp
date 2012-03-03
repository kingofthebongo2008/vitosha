#include "precompiled.h"

#include <gx/gx_render_context.h>
#include <gx/gx_thread_render_context.h>


namespace gx
{
    render_context::render_context(dx11::system_context sys_context, std::uint32_t thread_render_context_count) : m_system_context(sys_context)
    {
        m_render_contexts.reserve(thread_render_context_count);

        for (std::uint32_t i = 0; i < thread_render_context_count; ++i)
        {
			dx11::id3d11devicecontext_ptr device_context;

			dx11::throw_if_failed<dx11::create_deferred_context_exception> ( sys_context.m_device->CreateDeferredContext(0, dx11::get_pointer(device_context)));

			std::unique_ptr<thread_render_context> ptr(new thread_render_context(this, device_context) );
            m_render_contexts.push_back( std::move(ptr) );
        }


		create_buffers();
    }

	render_context::~render_context()
	{
	}

    void render_context::begin_frame()
    {
        for  (auto it = std::begin(m_render_contexts); it!= std::end(m_render_contexts); ++it)
        {
            (*it)->swap();
        }

		clear_buffers();
    }

    void render_context::end_frame()
    {
        for  (auto it = std::begin(m_render_contexts); it!= std::end(m_render_contexts); ++it)
        {
            (*it)->submit(m_system_context.m_immediate_context);
        }

    }

	void render_context::resize_buffers()
	{
		release_buffers();
		create_buffers();
	}

	void render_context::create_buffers()
	{
		create_back_buffer_render_target();
		create_depth_buffer();
		create_diffuse_buffer();
		create_specular_buffer();
		create_normal_depth_buffer();

		// gbuffer and depth prepass share common depth buffer
		m_gbuffer_render_set.m_depth_stencil = m_depth_stencil;
		m_gbuffer_render_set.m_depth_stencil_target = m_depth_stencil_target;

		m_depth_render_set.m_depth_stencil = m_depth_stencil;
		m_depth_render_set.m_depth_stencil_target = m_depth_stencil_target;
	}

	void render_context::release_buffers()
	{
		m_back_buffer_render_target.reset();

		m_depth_stencil.reset();
		m_depth_stencil_target.reset();

		m_gbuffer_render_set.reset();
		m_depth_render_set.reset();
	}

	void render_context::create_back_buffer_render_target()
	{
		dx11::id3d11texture2d_ptr	back_buffer;
		dx11::throw_if_failed< dx11::d3d11_exception> ( m_system_context.m_swap_chain->GetBuffer(0, __uuidof( ID3D11Texture2D ), (void**) dx11::get_pointer(back_buffer) ) );
		dx11::throw_if_failed< dx11::create_render_target_view> ( m_system_context.m_device->CreateRenderTargetView( back_buffer.get(), 0, dx11::get_pointer(m_back_buffer_render_target) ) );
	}

	void render_context::create_depth_buffer()
	{
		using namespace dx11;
		DXGI_SWAP_CHAIN_DESC desc = {};

		throw_if_failed<dx11::d3d11_exception>( m_system_context.m_swap_chain->GetDesc(&desc));

		D3D11_TEXTURE2D_DESC texture_description = {};

		texture_description.ArraySize = 1;
		texture_description.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texture_description.CPUAccessFlags = 0;
		texture_description.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texture_description.Height = desc.BufferDesc.Height;
		texture_description.MipLevels = 1;
		texture_description.MiscFlags = 0;
		texture_description.SampleDesc = desc.SampleDesc;
		texture_description.Usage = D3D11_USAGE_DEFAULT;
		texture_description.Width = desc.BufferDesc.Height;

		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateTexture2D( &texture_description, 0, dx11::get_pointer(m_depth_stencil)));
		dx11::throw_if_failed< dx11::create_depth_stencil_view> ( m_system_context.m_device->CreateDepthStencilView( m_depth_stencil.get(), 0, dx11::get_pointer(m_depth_stencil_target) ));
	}

	void render_context::create_diffuse_buffer()
	{
		using namespace dx11;
		DXGI_SWAP_CHAIN_DESC desc = {};

		throw_if_failed<dx11::d3d11_exception>( m_system_context.m_swap_chain->GetDesc(&desc));

		D3D11_TEXTURE2D_DESC texture_description = {};

		texture_description.ArraySize = 1;
		texture_description.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texture_description.CPUAccessFlags = 0;
		texture_description.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texture_description.Height = desc.BufferDesc.Height;
		texture_description.MipLevels = 1;
		texture_description.MiscFlags = 0;
		texture_description.SampleDesc = desc.SampleDesc;
		texture_description.Usage = D3D11_USAGE_DEFAULT;
		texture_description.Width = desc.BufferDesc.Height;

		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateTexture2D( & texture_description, 0, dx11::get_pointer(m_gbuffer_render_set.m_diffuse)));
		dx11::throw_if_failed< dx11::create_render_target_view> ( m_system_context.m_device->CreateRenderTargetView( m_gbuffer_render_set.m_diffuse.get(), 0, dx11::get_pointer(m_gbuffer_render_set.m_diffuse_target)));
	}

	void render_context::create_specular_buffer()
	{
		using namespace dx11;
		DXGI_SWAP_CHAIN_DESC desc = {};

		throw_if_failed<dx11::d3d11_exception>( m_system_context.m_swap_chain->GetDesc(&desc));

		D3D11_TEXTURE2D_DESC texture_description = {};

		texture_description.ArraySize = 1;
		texture_description.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texture_description.CPUAccessFlags = 0;
		texture_description.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texture_description.Height = desc.BufferDesc.Height;
		texture_description.MipLevels = 1;
		texture_description.MiscFlags = 0;
		texture_description.SampleDesc = desc.SampleDesc;
		texture_description.Usage = D3D11_USAGE_DEFAULT;
		texture_description.Width = desc.BufferDesc.Height;

		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateTexture2D( &texture_description, 0, dx11::get_pointer(m_gbuffer_render_set.m_specular)));
		dx11::throw_if_failed< dx11::create_render_target_view> ( m_system_context.m_device->CreateRenderTargetView( m_gbuffer_render_set.m_diffuse.get(), 0, dx11::get_pointer(m_gbuffer_render_set.m_specular_target)));
	}

	void render_context::create_normal_depth_buffer()
	{
		using namespace dx11;
		DXGI_SWAP_CHAIN_DESC desc = {};

		throw_if_failed<dx11::d3d11_exception>( m_system_context.m_swap_chain->GetDesc(&desc));

		D3D11_TEXTURE2D_DESC texture_description = {};

		texture_description.ArraySize = 1;
		texture_description.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texture_description.CPUAccessFlags = 0;
		texture_description.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
		texture_description.Height = desc.BufferDesc.Height;
		texture_description.MipLevels = 1;
		texture_description.MiscFlags = 0;
		texture_description.SampleDesc = desc.SampleDesc;
		texture_description.Usage = D3D11_USAGE_DEFAULT;
		texture_description.Width = desc.BufferDesc.Height;

		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateTexture2D( &texture_description, 0, dx11::get_pointer(m_gbuffer_render_set.m_normal_depth)));
		dx11::throw_if_failed< dx11::create_render_target_view> ( m_system_context.m_device->CreateRenderTargetView( m_gbuffer_render_set.m_diffuse.get(), 0, dx11::get_pointer(m_gbuffer_render_set.m_normal_depth_target)));
	}
	
	void render_context::clear_buffers()
	{
		m_system_context.m_immediate_context->ClearDepthStencilView( m_depth_stencil_target.get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

		float clear_color_1[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float clear_color_2[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
		float clear_color_3[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

		m_system_context.m_immediate_context->ClearRenderTargetView( m_back_buffer_render_target.get(), clear_color_1);
		m_system_context.m_immediate_context->ClearRenderTargetView( m_gbuffer_render_set.m_normal_depth_target.get(), clear_color_2);
		m_system_context.m_immediate_context->ClearRenderTargetView( m_gbuffer_render_set.m_diffuse_target.get(), clear_color_3);
		m_system_context.m_immediate_context->ClearRenderTargetView( m_gbuffer_render_set.m_specular_target.get(), clear_color_3);
	}

	void render_context::select_gbuffer(dx11::id3d11devicecontext_ptr device_context)
	{
		ID3D11RenderTargetView* views[3] =
		{ 
												m_gbuffer_render_set.m_normal_depth_target.get(),
												m_gbuffer_render_set.m_diffuse_target.get(),
												m_gbuffer_render_set.m_specular_target.get()
		};

		device_context->OMSetRenderTargets( 3, views, m_depth_stencil_target.get() );
	}
	
	void render_context::select_main_target(dx11::id3d11devicecontext_ptr device_context)
	{
		device_context->OMSetRenderTargets( 1, dx11::get_pointer(m_back_buffer_render_target), m_depth_stencil_target.get() );
	}

}

