#include "precompiled.h"

#include <gx/gx_render_context.h>
#include <gx/gx_thread_render_context.h>


namespace gx
{
    render_context::render_context(dx11::system_context sys_context, std::uint32_t thread_render_context_count) : 
		m_system_context(sys_context)
		, m_depth_vertex_shader( sys_context.m_device.get() )
		, m_depth_constant_buffer ( sys_context.m_device.get() )
    {
        m_render_contexts.reserve(thread_render_context_count);

        for (std::uint32_t i = 0; i < thread_render_context_count; ++i)
        {
			dx11::id3d11devicecontext_ptr device_context;

			dx11::throw_if_failed<dx11::create_deferred_context_exception> ( sys_context.m_device->CreateDeferredContext(0, dx11::get_pointer(device_context)));

			std::unique_ptr<thread_render_context> ptr(new thread_render_context(this, device_context) );
            m_render_contexts.push_back( std::move(ptr) );
        }


		create_swap_chain_buffers();
		create_gbuffer_state();
		create_depth_state();
    }

	render_context::~render_context()
	{
	}

    void render_context::begin_frame()
    {
		//submit everything we have done in the previous frame
		for  (auto it = std::begin(m_render_contexts); it!= std::end(m_render_contexts); ++it)
        {
            (*it)->submit(m_system_context.m_immediate_context);
        }

		//begin new command lists
        for  (auto it = std::begin(m_render_contexts); it!= std::end(m_render_contexts); ++it)
        {
            (*it)->swap();
        }
		
		dx11::id3d11devicecontext_ptr device_context = m_render_contexts.front()->get_device_context();
		clear_buffers( device_context.get() );
    }

    void render_context::end_frame()
    {

    }

	void render_context::create_swap_chain_buffers()
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

	void render_context::release_swap_chain_buffers()
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
	
	void render_context::clear_buffers(ID3D11DeviceContext* device_context)
	{
		device_context->ClearDepthStencilView( m_depth_stencil_target.get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

		float clear_color_1[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
		float clear_color_2[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
		float clear_color_3[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

		device_context->ClearRenderTargetView( m_back_buffer_render_target.get(), clear_color_1);
		device_context->ClearRenderTargetView( m_gbuffer_render_set.m_normal_depth_target.get(), clear_color_2);
		device_context->ClearRenderTargetView( m_gbuffer_render_set.m_diffuse_target.get(), clear_color_3);
		device_context->ClearRenderTargetView( m_gbuffer_render_set.m_specular_target.get(), clear_color_3);
	}

	void render_context::select_gbuffer(ID3D11DeviceContext* device_context)
	{
		ID3D11RenderTargetView* views[3] =
		{ 
												m_gbuffer_render_set.m_normal_depth_target.get(),
												m_gbuffer_render_set.m_diffuse_target.get(),
												m_gbuffer_render_set.m_specular_target.get()
		};

		device_context->OMSetRenderTargets( 3, views, m_depth_stencil_target.get() );
		device_context->OMSetBlendState(m_gbuffer_state.m_blend_opaque.get(), nullptr, 0xFFFFFFFF);
		device_context->OMSetDepthStencilState(m_gbuffer_state.m_depth.get(), 0 );
		
		device_context->RSSetState(m_gbuffer_state.m_rasterizer.get());

		device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	
	void render_context::select_back_buffer_target(ID3D11DeviceContext* device_context)
	{
		device_context->OMSetRenderTargets( 1, dx11::get_pointer(m_back_buffer_render_target), m_depth_stencil_target.get() );
	}

	void render_context::select_depth_pass(ID3D11DeviceContext* device_context)
	{
		device_context->OMSetRenderTargets(0, nullptr, m_depth_render_set.m_depth_stencil_target.get());
		device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		device_context->PSSetShader(nullptr, nullptr, 0);

		device_context->OMSetBlendState(m_depth_state.m_blend_opaque.get(), nullptr, 0xFFFFFFFF);
		device_context->OMSetDepthStencilState(m_depth_state.m_depth.get(), 0 );
		device_context->RSSetState(m_depth_state.m_rasterizer.get());

		m_depth_vertex_shader.bind(device_context, &m_depth_constant_buffer);
	}

	void render_context::create_gbuffer_state()
	{
		D3D11_SAMPLER_DESC sampler = {};

		sampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampler.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampler.MaxAnisotropy = 16;
		sampler.MaxLOD = std::numeric_limits<float>::max();
		sampler.MinLOD = std::numeric_limits<float>::min();
		sampler.MipLODBias = 0;
		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateSamplerState(&sampler, dx11::get_pointer(m_gbuffer_state.m_sampler)));

		D3D11_BLEND_DESC blend = {};
		blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateBlendState(&blend, dx11::get_pointer(m_gbuffer_state.m_blend_opaque)));

		D3D11_DEPTH_STENCIL_DESC depth_stencil = {};
		depth_stencil.DepthEnable = true;
		depth_stencil.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		depth_stencil.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateDepthStencilState(&depth_stencil, dx11::get_pointer(m_gbuffer_state.m_depth)));


		D3D11_RASTERIZER_DESC rasterizer = {};

		rasterizer.FillMode = D3D11_FILL_SOLID;
		rasterizer.CullMode = D3D11_CULL_BACK;
		rasterizer.DepthClipEnable = 1;
		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateRasterizerState(&rasterizer, dx11::get_pointer(m_gbuffer_state.m_rasterizer)));
	}

	void render_context::create_depth_state()
	{
		D3D11_SAMPLER_DESC sampler = {};

		sampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampler.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampler.MaxAnisotropy = 16;
		sampler.MaxLOD = std::numeric_limits<float>::max();
		sampler.MinLOD = std::numeric_limits<float>::min();
		sampler.MipLODBias = 0;
		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateSamplerState(&sampler, dx11::get_pointer(m_depth_state.m_sampler)));

		D3D11_BLEND_DESC blend = {};
		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateBlendState(&blend, dx11::get_pointer(m_depth_state.m_blend_opaque)));

		D3D11_DEPTH_STENCIL_DESC depth_stencil = {};
		depth_stencil.DepthEnable = true;
		depth_stencil.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		depth_stencil.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateDepthStencilState(&depth_stencil, dx11::get_pointer(m_depth_state.m_depth)));


		D3D11_RASTERIZER_DESC rasterizer = {};

		rasterizer.FillMode = D3D11_FILL_SOLID;
		rasterizer.CullMode = D3D11_CULL_BACK;
		rasterizer.DepthClipEnable = 1;
		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateRasterizerState(&rasterizer, dx11::get_pointer(m_depth_state.m_rasterizer)));
	}


	/*
	id3d11samplerstate_ptr
	id3d11depthstencilstate_ptr
	id3d11blendstate_ptr
	id3d11rasterizerstate_ptr


	CreateDepthStencilState
	CreateRasterizerState
	CreateSamplerState
	CreateBlendState
	*/
}

