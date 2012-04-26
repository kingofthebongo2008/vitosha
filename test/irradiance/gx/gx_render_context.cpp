#include "precompiled.h"

#include <array>

#include <gx/gx_render_context.h>
#include <gx/gx_thread_render_context.h>

#include <math/math_half.h>


namespace gx
{
    render_context::render_context(dx11::system_context sys_context, uint32_t thread_render_context_count, view_port view_port) : 
		m_system_context(sys_context)
		, m_depth_render_data( sys_context.m_device.get() )
		, m_view_port(view_port)
		, m_screen_space_render_data ( sys_context.m_device.get() )

        , m_transform_position_vertex_shader( sys_context.m_device.get() )
        , m_transform_position_vertex_shader_cbuffer( sys_context.m_device.get() )
        , m_transform_position_input_layout ( sys_context.m_device.get(), &m_transform_position_vertex_shader )

        , m_transform_position_uv_vertex_shader( sys_context.m_device.get() )
        , m_transform_position_uv_vertex_shader_cbuffer( sys_context.m_device.get() )
		, m_transform_position_uv_input_layout( sys_context.m_device.get(), &m_transform_position_uv_vertex_shader )

        , m_transform_position_normal_uv_vertex_shader( sys_context.m_device.get() )
        , m_transform_position_normal_uv_vertex_shader_cbuffer( sys_context.m_device.get() )
		, m_transform_position_normal_uv_input_layout ( sys_context.m_device.get(), &m_transform_position_normal_uv_vertex_shader )

		, m_color_pixel_shader ( sys_context.m_device.get() )
		, m_color_pixel_shader_cbuffer ( sys_context.m_device.get() )
		, m_color_texture_pixel_shader (  sys_context.m_device.get() )
        , m_color_texture_channel_3_pixel_shader (  sys_context.m_device.get() )
        
        , m_debug_view_space_depth_pixel_shader (  sys_context.m_device.get() )
        , m_debug_view_space_depth_pixel_shader_cbuffer (  sys_context.m_device.get() )

        , m_screen_space_uv_vertex_shader         (  sys_context.m_device.get() )
        , m_screen_space_uv_vertex_shader_cbuffer (  sys_context.m_device.get() )

		, m_lambert_shift_invariant_pixel_shader(sys_context.m_device.get() )
		, m_lambert_pixel_cbuffer( sys_context.m_device.get() )
        , m_blinn_phong_shift_invariant_pixel_shader( sys_context.m_device.get()  )
		, m_blinn_phong_pixel_cbuffer( sys_context.m_device.get() )

    {
        m_render_contexts.reserve(thread_render_context_count);

        for (uint32_t i = 0; i < thread_render_context_count; ++i)
        {
			dx11::id3d11devicecontext_ptr device_context;

			dx11::throw_if_failed<dx11::create_deferred_context_exception> ( sys_context.m_device->CreateDeferredContext(0, dx11::get_pointer(device_context)));

			std::unique_ptr<thread_render_context> ptr(new thread_render_context(this, device_context) );
            m_render_contexts.push_back( std::move(ptr) );
        }

		create_swap_chain_buffers();
		create_gbuffer_state();
		create_depth_state();

		create_depth_buffer_layout();
		create_screen_space_input_layout();
		create_screen_space_vertex_buffer();

		create_default_render_data();
        create_light_buffer_render_data();
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
		clear_state( device_context.get() );
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
        create_light_buffer();

		// gbuffer and depth prepass share common depth buffer
		m_gbuffer_render_data.m_render_set.m_depth_stencil = m_depth_stencil;
		m_gbuffer_render_data.m_render_set.m_depth_stencil_target = m_depth_stencil_target;

		m_depth_render_data.m_render_set.m_depth_stencil = m_depth_stencil;
		m_depth_render_data.m_render_set.m_depth_stencil_target = m_depth_stencil_target;

		m_default_render_data.m_render_set.m_back_buffer_render_target = m_back_buffer_render_target;
		m_default_render_data.m_render_set.m_depth_stencil = m_depth_stencil;
		m_default_render_data.m_render_set.m_depth_stencil_target = m_depth_stencil_target;
	}

	void render_context::release_swap_chain_buffers()
	{
		m_back_buffer_render_target.reset();

		m_depth_stencil.reset();
		m_depth_stencil_target.reset();
				
		m_gbuffer_render_data.m_render_set.reset();
		m_depth_render_data.m_render_set.reset();
		m_default_render_data.m_render_set.reset();

        m_light_buffer_render_data.m_render_set.reset();
	}

	void render_context::create_back_buffer_render_target()
	{
		dx11::id3d11texture2d_ptr	back_buffer;
		dx11::throw_if_failed< dx11::d3d11_exception> ( m_system_context.m_swap_chain->GetBuffer(0, __uuidof( ID3D11Texture2D ), (void**) dx11::get_pointer(back_buffer) ) );
		dx11::throw_if_failed< dx11::create_render_target_view_exception> ( m_system_context.m_device->CreateRenderTargetView( back_buffer.get(), 0, dx11::get_pointer(m_back_buffer_render_target) ) );
	}

	void render_context::create_depth_buffer()
	{
		using namespace dx11;
		DXGI_SWAP_CHAIN_DESC desc = {};

		throw_if_failed<dx11::d3d11_exception>( m_system_context.m_swap_chain->GetDesc(&desc));

		D3D11_TEXTURE2D_DESC texture_description = {};

		texture_description.ArraySize = 1;
		texture_description.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE ;
		texture_description.CPUAccessFlags = 0;
		texture_description.Format = DXGI_FORMAT_R24G8_TYPELESS;
		texture_description.Height = desc.BufferDesc.Height;
		texture_description.MipLevels = 1;
		texture_description.MiscFlags = 0;
		texture_description.SampleDesc = desc.SampleDesc;
		texture_description.Usage = D3D11_USAGE_DEFAULT;
		texture_description.Width = desc.BufferDesc.Width;

		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateTexture2D( &texture_description, 0, dx11::get_pointer(m_depth_stencil)));
        
        D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view = {};

        depth_stencil_view.Flags = 0;
        depth_stencil_view.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depth_stencil_view.Texture2D.MipSlice = 0;
        depth_stencil_view.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dx11::throw_if_failed< dx11::create_depth_stencil_view_exception> ( m_system_context.m_device->CreateDepthStencilView( m_depth_stencil.get(), &depth_stencil_view, dx11::get_pointer(m_depth_stencil_target) ));


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
		texture_description.Width = desc.BufferDesc.Width;

		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateTexture2D( & texture_description, 0, dx11::get_pointer(m_gbuffer_render_data.m_render_set.m_diffuse)));
		dx11::throw_if_failed< dx11::create_render_target_view_exception> ( m_system_context.m_device->CreateRenderTargetView( m_gbuffer_render_data.m_render_set.m_diffuse.get(), 0, dx11::get_pointer(m_gbuffer_render_data.m_render_set.m_diffuse_target)));
		dx11::throw_if_failed< dx11::create_resource_view_exception>( m_system_context.m_device->CreateShaderResourceView( m_gbuffer_render_data.m_render_set.m_diffuse.get(), nullptr, dx11::get_pointer(m_gbuffer_render_data.m_render_set.m_diffuse_view)));
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
		texture_description.Width = desc.BufferDesc.Width;

		dx11::throw_if_failed< dx11::create_texture_exception > ( m_system_context.m_device->CreateTexture2D( &texture_description, 0, dx11::get_pointer(m_gbuffer_render_data.m_render_set.m_specular)));
		dx11::throw_if_failed< dx11::create_render_target_view_exception > ( m_system_context.m_device->CreateRenderTargetView( m_gbuffer_render_data.m_render_set.m_specular.get(), 0, dx11::get_pointer(m_gbuffer_render_data.m_render_set.m_specular_target)));
		dx11::throw_if_failed< dx11::create_resource_view_exception>( m_system_context.m_device->CreateShaderResourceView( m_gbuffer_render_data.m_render_set.m_specular.get(), nullptr, dx11::get_pointer(m_gbuffer_render_data.m_render_set.m_specular_view)));
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
		texture_description.Width = desc.BufferDesc.Width;

		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateTexture2D( &texture_description, 0, dx11::get_pointer(m_gbuffer_render_data.m_render_set.m_normal_depth)));
		dx11::throw_if_failed< dx11::create_render_target_view_exception> ( m_system_context.m_device->CreateRenderTargetView( m_gbuffer_render_data.m_render_set.m_normal_depth.get(), 0, dx11::get_pointer(m_gbuffer_render_data.m_render_set.m_normal_depth_target)));
		dx11::throw_if_failed< dx11::create_resource_view_exception>( m_system_context.m_device->CreateShaderResourceView( m_gbuffer_render_data.m_render_set.m_normal_depth.get(), nullptr, dx11::get_pointer(m_gbuffer_render_data.m_render_set.m_normal_depth_view) ) );
	}

    void render_context::create_light_buffer()
    {
        using namespace dx11;
		DXGI_SWAP_CHAIN_DESC desc = {};

		throw_if_failed<dx11::d3d11_exception>( m_system_context.m_swap_chain->GetDesc(&desc));

		D3D11_TEXTURE2D_DESC texture_description = {};

		texture_description.ArraySize = 1;
		texture_description.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texture_description.CPUAccessFlags = 0;
		texture_description.Format = DXGI_FORMAT_R16G16B16A16_UNORM;//DXGI_FORMAT_R8G8B8A8_UNORM;//DXGI_FORMAT_R16G16B16A16_UNORM;
		texture_description.Height = desc.BufferDesc.Height;
		texture_description.MipLevels = 1;
		texture_description.MiscFlags = 0;
		texture_description.SampleDesc = desc.SampleDesc;
		texture_description.Usage = D3D11_USAGE_DEFAULT;
		texture_description.Width = desc.BufferDesc.Width;
        
		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateTexture2D( &texture_description, 0, dx11::get_pointer(m_light_buffer_render_data.m_render_set.m_light_buffer)));
		dx11::throw_if_failed< dx11::create_render_target_view_exception> ( m_system_context.m_device->CreateRenderTargetView( m_light_buffer_render_data.m_render_set.m_light_buffer.get(), 0, dx11::get_pointer(m_light_buffer_render_data.m_render_set.m_light_buffer_target)));
        dx11::throw_if_failed< dx11::create_resource_view_exception>( m_system_context.m_device->CreateShaderResourceView( m_light_buffer_render_data.m_render_set.m_light_buffer.get(), nullptr, dx11::get_pointer(m_light_buffer_render_data.m_render_set.m_light_buffer_view) ) );
        


        D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view = {};
        depth_stencil_view.Flags = D3D11_DSV_READ_ONLY_DEPTH;
        depth_stencil_view.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depth_stencil_view.Texture2D.MipSlice = 0;
        depth_stencil_view.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dx11::throw_if_failed< dx11::create_depth_stencil_view_exception> ( m_system_context.m_device->CreateDepthStencilView( m_depth_stencil.get(), &depth_stencil_view, dx11::get_pointer( m_light_buffer_render_data.m_render_set.m_depth_stencil_target) ));

        D3D11_SHADER_RESOURCE_VIEW_DESC depth_stencil_resource_view = {};
        depth_stencil_resource_view.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        depth_stencil_resource_view.Texture2D.MostDetailedMip = 0;
        depth_stencil_resource_view.Texture2D.MipLevels = 1;
        depth_stencil_resource_view.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

        dx11::throw_if_failed< dx11::create_resource_view_exception>( m_system_context.m_device->CreateShaderResourceView( m_depth_stencil.get(), &depth_stencil_resource_view, dx11::get_pointer(m_light_buffer_render_data.m_render_set.m_depth_stencil_view) ) );
    }
	
	void render_context::clear_buffers(ID3D11DeviceContext* device_context)
	{
		device_context->ClearDepthStencilView( m_depth_stencil_target.get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

		float clear_color_1[4] = { 0.449019607f, 0.449019607f, 0.449019607f, 1.0f };
		float clear_color_2[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float clear_color_3[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

		device_context->ClearRenderTargetView( m_back_buffer_render_target.get(), clear_color_1);
		device_context->ClearRenderTargetView( m_gbuffer_render_data.m_render_set.m_normal_depth_target.get(), clear_color_2);
		device_context->ClearRenderTargetView( m_gbuffer_render_data.m_render_set.m_diffuse_target.get(), clear_color_3);
		device_context->ClearRenderTargetView( m_gbuffer_render_data.m_render_set.m_specular_target.get(), clear_color_3);

        device_context->ClearRenderTargetView( m_light_buffer_render_data.m_render_set.m_light_buffer_target.get(), clear_color_3);
	}

	void render_context::clear_state(ID3D11DeviceContext* device_context)
	{
		clear_buffers(device_context);
		reset_shader_resources(device_context);
		reset_render_targets(device_context);
	}

	void render_context::select_gbuffer(ID3D11DeviceContext* device_context)
	{
		reset_render_targets(device_context);
		reset_shader_resources(device_context);

		ID3D11RenderTargetView* views[3] =
		{ 
												m_gbuffer_render_data.m_render_set.m_diffuse_target.get(),
												m_gbuffer_render_data.m_render_set.m_normal_depth_target.get(),
												m_gbuffer_render_data.m_render_set.m_specular_target.get()
		};

		device_context->OMSetRenderTargets( 3, views, m_depth_stencil_target.get() );
		device_context->OMSetBlendState(m_gbuffer_render_data.m_state.m_blend_opaque.get(), nullptr, 0xFFFFFFFF);
		device_context->OMSetDepthStencilState(m_gbuffer_render_data.m_state.m_depth.get(), 0 );
		device_context->RSSetState(m_gbuffer_render_data.m_state.m_rasterizer.get());
		device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		ID3D11SamplerState* samplers[] =	{ 
												m_gbuffer_render_data.m_state.m_sampler.get(), 
												m_gbuffer_render_data.m_state.m_sampler.get(), 
												m_gbuffer_render_data.m_state.m_sampler.get(), 
												m_gbuffer_render_data.m_state.m_sampler.get()
											};

		device_context->PSSetSamplers( 0, sizeof(samplers)/sizeof(samplers[0]), &samplers[0] ); 

		select_view_port(device_context);
	}

    void render_context::select_light_buffer(ID3D11DeviceContext* device_context)
    {
        reset_render_targets(device_context);
		reset_shader_resources(device_context);

        ID3D11RenderTargetView* views[] =
		{ 
            m_light_buffer_render_data.m_render_set.m_light_buffer_target.get()
		};

        device_context->OMSetRenderTargets( 1, &views[0], m_light_buffer_render_data.m_render_set.m_depth_stencil_target.get() );
		device_context->OMSetBlendState(m_light_buffer_render_data.m_state.m_blend_opaque.get(), nullptr, 0xFFFFFFFF);
		device_context->OMSetDepthStencilState(m_light_buffer_render_data.m_state.m_depth.get(), 0 );
		device_context->RSSetState(m_light_buffer_render_data.m_state.m_rasterizer.get());
		device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		ID3D11SamplerState* samplers[] =	{ 
												m_light_buffer_render_data.m_state.m_sampler.get(), 
												m_light_buffer_render_data.m_state.m_sampler.get(), 
												m_light_buffer_render_data.m_state.m_sampler.get(), 
												m_light_buffer_render_data.m_state.m_sampler.get()
											};

		device_context->PSSetSamplers( 0, sizeof(samplers)/sizeof(samplers[0]), &samplers[0] ); 

        select_view_port(device_context);

        ID3D11ShaderResourceView * shader_resource_view[] =
        {
                         m_gbuffer_render_data.m_render_set.m_normal_depth_view.get(),  
                         m_gbuffer_render_data.m_render_set.m_diffuse_view.get(),
                         m_gbuffer_render_data.m_render_set.m_specular_view.get(),
                         m_light_buffer_render_data.m_render_set.m_depth_stencil_view.get() //depth buffer from the gbuffer pass
        };

        dx11::ps_set_shader_resources( device_context, sizeof(shader_resource_view) / sizeof( shader_resource_view[0] ) , shader_resource_view);
        //device_context->PSSetShaderResources( 0, sizeof(shader_resource_view)/sizeof(shader_resource_view[0]), &shader_resource_view[0]);
    }

    void render_context::end_light_buffer(ID3D11DeviceContext* device_context)
    {
        reset_render_targets(device_context);
		reset_shader_resources(device_context);
    }

	void render_context::end_gbuffer(ID3D11DeviceContext* device_context)
	{
		reset_render_targets(device_context);
		reset_shader_resources(device_context);
	}

	void render_context::select_back_buffer_target(ID3D11DeviceContext* device_context)
	{
		reset_render_targets(device_context);
		reset_shader_resources(device_context);

		device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		device_context->OMSetBlendState(m_default_render_data.m_state.m_blend_opaque.get(), nullptr, 0xFFFFFFFF);
		device_context->OMSetDepthStencilState(m_default_render_data.m_state.m_depth.get(), 0 );

		ID3D11RenderTargetView* views[1] =
		{
			m_default_render_data.m_render_set.m_back_buffer_render_target.get()
		};

		//device_context->OMSetRenderTargets( 1, &views[0], m_default_render_data.m_render_set.m_depth_stencil_target.get() );
        device_context->OMSetRenderTargets( 1, &views[0], m_light_buffer_render_data.m_render_set.m_depth_stencil_target.get() );
        device_context->RSSetState(m_default_render_data.m_state.m_rasterizer.get());

        ID3D11SamplerState* samplers[] =	{ 
												m_default_render_data.m_state.m_sampler.get(), 
												m_default_render_data.m_state.m_sampler.get(), 
												m_default_render_data.m_state.m_sampler.get(), 
												m_default_render_data.m_state.m_sampler.get()
											};

		device_context->PSSetSamplers( 0, sizeof(samplers)/sizeof(samplers[0]), &samplers[0] ); 
		select_view_port(device_context);
	}

	void render_context::select_depth_pass(ID3D11DeviceContext* device_context)
	{
		reset_render_targets(device_context);
		reset_shader_resources(device_context);

		device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		device_context->IASetInputLayout(m_depth_render_data.m_input_layout);

        dx11::ps_set_shader(device_context, nullptr);

		device_context->OMSetRenderTargets(1, nullptr, m_depth_render_data.m_render_set.m_depth_stencil_target.get());
		device_context->OMSetBlendState(m_depth_render_data.m_state.m_blend_opaque.get(), nullptr, 0xFFFFFFFF);
		device_context->OMSetDepthStencilState(m_depth_render_data.m_state.m_depth.get(), 0 );

        device_context->RSSetState(m_depth_render_data.m_state.m_rasterizer.get());
                
        m_depth_render_data.m_depth_constant_buffer.bind_as_vertex_constant_buffer(device_context);

		select_view_port(device_context);
	}

	void render_context::end_depth_pass(ID3D11DeviceContext* device_context)
	{
		reset_shader_resources(device_context);
		reset_render_targets(device_context);
	}

	void render_context::reset_shader_resources(ID3D11DeviceContext* device_context)
	{
		ID3D11ShaderResourceView* resources[6] = 
		{ 
			nullptr,
			nullptr,
			nullptr,

			nullptr,
			nullptr,
			nullptr
		};

        dx11::ps_set_shader_resources ( device_context, sizeof(resources) / sizeof(resources[0] ) , resources );
	}

	void render_context::reset_render_targets(ID3D11DeviceContext* device_context)
	{
		std::array<ID3D11RenderTargetView*,3> views = 
		{ 
												nullptr,
												nullptr,
												nullptr
		};

		device_context->OMSetRenderTargets( static_cast<uint32_t> (views.size() ) , &views[0], nullptr );
	}

	void render_context::select_view_port(ID3D11DeviceContext* device_context)
	{
		D3D11_VIEWPORT view_port;

		view_port.Height	= static_cast<float> (m_view_port.get_height());
		view_port.MaxDepth	= m_view_port.get_max_z();
		view_port.MinDepth	= m_view_port.get_min_z();
		view_port.TopLeftX	= static_cast<float> (m_view_port.get_left());
		view_port.TopLeftY	= static_cast<float> (m_view_port.get_top());
		view_port.Width		= static_cast<float> (m_view_port.get_width()) ;

		device_context->RSSetViewports(1, &view_port);
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
		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateSamplerState(&sampler, dx11::get_pointer(m_gbuffer_render_data.m_state.m_sampler)));

		D3D11_BLEND_DESC blend = {};
		blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateBlendState(&blend, dx11::get_pointer(m_gbuffer_render_data.m_state.m_blend_opaque)));

		D3D11_DEPTH_STENCIL_DESC depth_stencil = {};
		depth_stencil.DepthEnable = true;
		depth_stencil.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		depth_stencil.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateDepthStencilState(&depth_stencil, dx11::get_pointer(m_gbuffer_render_data.m_state.m_depth)));


		D3D11_RASTERIZER_DESC rasterizer = {};

		rasterizer.FillMode = D3D11_FILL_SOLID;//D3D11_FILL_WIREFRAME;//D3D11_FILL_SOLID;
		rasterizer.CullMode = D3D11_CULL_BACK;
		rasterizer.DepthClipEnable = 1;
		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateRasterizerState(&rasterizer, dx11::get_pointer(m_gbuffer_render_data.m_state.m_rasterizer)));
	}

    void render_context::create_light_buffer_state()
    {

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
		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateSamplerState(&sampler, dx11::get_pointer(m_depth_render_data.m_state.m_sampler)));

		D3D11_BLEND_DESC blend = {};
		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateBlendState(&blend, dx11::get_pointer(m_depth_render_data.m_state.m_blend_opaque)));

		D3D11_DEPTH_STENCIL_DESC depth_stencil = {};
		depth_stencil.DepthEnable = true;
		depth_stencil.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		depth_stencil.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateDepthStencilState(&depth_stencil, dx11::get_pointer(m_depth_render_data.m_state.m_depth)));


		D3D11_RASTERIZER_DESC rasterizer = {};

		rasterizer.FillMode = D3D11_FILL_SOLID;//D3D11_FILL_WIREFRAME;//D3D11_FILL_SOLID;
		rasterizer.CullMode = D3D11_CULL_BACK;
		rasterizer.DepthClipEnable = 1;
		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateRasterizerState(&rasterizer, dx11::get_pointer(m_depth_render_data.m_state.m_rasterizer)));
	}

	void render_context::create_depth_buffer_layout()
	{
        m_depth_render_data.m_input_layout = m_transform_position_input_layout;
	}

	void render_context::create_screen_space_input_layout()
	{
        m_screen_space_render_data.m_screen_space_input_layout = m_transform_position_uv_input_layout;
	}

	screen_space_quad_render	render_context::create_screen_space_quad_render()
	{
		screen_space_quad_render quad_render  = { 
													  m_screen_space_render_data.m_screen_space_vertex_shader
													, m_screen_space_render_data.m_screen_space_vertex_buffer.get()
													, m_screen_space_render_data.m_screen_space_constant_buffer
													, m_screen_space_render_data.m_screen_space_input_layout.get()
												};

		return quad_render;
	}

	void render_context::create_screen_space_vertex_buffer()
	{
		using namespace math;
		D3D11_BUFFER_DESC desc = {};

		struct vertex
		{
			half v[4];
			half uv[2];
		};

		struct vertex_float
		{
			float v[4];
			float uv[2];
		};

		const vertex_float v_1[ 6 + 2 ] =
		{ 
			 { {-1.0f,	-1.0f,	0.0f, 1.0f},  {0.0f, 0.0f}},
			 { {-1.0f,	 1.0f,	0.0f, 1.0f},  {0.0f, 1.0f}},
			 { {1.0f,	 1.0f,	0.0f, 1.0f},  {1.0f, 1.0f}},

			 { {1.0f,	 1.0f,	0.0f, 1.0f} , {1.0f, 1.0f}},
			 { {1.0f,	-1.0f,	0.0f, 1.0f} , {1.0f, 0.0f}},
			 { {-1.0f,	-1.0f,	0.0f, 1.0f} , {0.0f,0.0f}},

			 { {0.0f,	0.0f,	0.0f, 0.0f} , {0.0f,0.0f}},	//padding
			 { {0.0f,	0.0f,	0.0f, 0.0f} , {0.0f,0.0f}},	//padding
		};

		__declspec( align(16) ) math::half h_1 [ 40 ];

		math::convert_f32_f16_stream(reinterpret_cast<const float*> (&v_1[0]), static_cast<uint32_t>(40), &h_1[0] );

		desc.ByteWidth = 6 * sizeof(vertex);
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA initial_data = { &h_1[0], 0, 0};

		dx11::throw_if_failed<dx11::create_buffer_exception> (m_system_context.m_device->CreateBuffer(&desc, &initial_data, dx11::get_pointer(m_screen_space_render_data.m_screen_space_vertex_buffer)));
	}

	void render_context::create_default_render_data()
	{
		m_default_render_data.m_render_set.m_back_buffer_render_target = m_back_buffer_render_target;
		m_default_render_data.m_render_set.m_depth_stencil = m_depth_stencil;
		m_default_render_data.m_render_set.m_depth_stencil_target = m_depth_stencil_target;

		m_default_render_data.m_state.m_blend_opaque = m_gbuffer_render_data.m_state.m_blend_opaque;
		m_default_render_data.m_state.m_depth = m_gbuffer_render_data.m_state.m_depth;
		m_default_render_data.m_state.m_rasterizer = m_gbuffer_render_data.m_state.m_rasterizer;
		m_default_render_data.m_state.m_sampler = m_gbuffer_render_data.m_state.m_sampler;
	}

    void render_context::create_light_buffer_render_data()
    {
		m_light_buffer_render_data.m_state = m_gbuffer_render_data.m_state;

        m_light_buffer_render_data.m_state.m_depth.reset();

        D3D11_DEPTH_STENCIL_DESC depth_stencil = {};
		depth_stencil.DepthEnable = false;
		depth_stencil.DepthFunc = D3D11_COMPARISON_ALWAYS;
		depth_stencil.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dx11::throw_if_failed< dx11::create_texture_exception> ( m_system_context.m_device->CreateDepthStencilState(&depth_stencil, dx11::get_pointer(m_light_buffer_render_data.m_state.m_depth)));

    }
}

