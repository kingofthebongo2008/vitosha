#include "precompiled.h"

#include <array>

#include <gx/gx_render_context.h>

#include <d3d11/dxgi_helpers.h>

#include <math/math_half.h>

#include <gx/gx_constant_buffer_helper.h>
#include <gx/gx_geometry_pass_common.h>
#include <gx/gx_profile.h>
#include <gx/gx_thread_render_context.h>


namespace gx
{
    namespace
    {
        void reset_shader_resources(ID3D11DeviceContext* device_context)
        {
            ID3D11ShaderResourceView* resources[ D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT ];

            for (auto i = 0; i <D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT;++i)
            {
                resources[i] = nullptr;
            }

            d3d11::ps_set_shader_resources ( device_context, sizeof(resources) / sizeof(resources[0] ) , resources );
            d3d11::vs_set_shader_resources ( device_context, sizeof(resources) / sizeof(resources[0] ) , resources );
        }

        void reset_constant_buffers(ID3D11DeviceContext* device_context)
        {
            ID3D11Buffer * buffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];

            for (auto i = 0; i <D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT;++i)
            {
                buffers[i] = nullptr;
            }

            device_context->VSSetConstantBuffers(0, sizeof(buffers)/ sizeof(buffers[0]), &buffers[0]); 
            device_context->PSSetConstantBuffers(0, sizeof(buffers)/ sizeof(buffers[0]), &buffers[0]); 
        }

        void reset_render_targets(ID3D11DeviceContext* device_context)
        {
            std::array<ID3D11RenderTargetView*, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> views = 
            { 
                                                    nullptr,
                                                    nullptr,
                                                    nullptr,
                                                    nullptr,

                                                    nullptr,
                                                    nullptr,
                                                    nullptr,
                                                    nullptr
            };

            device_context->OMSetRenderTargets( static_cast<uint32_t> (views.size() ) , &views[0], nullptr );
        }
    }

    render_context::render_context(d3d11::system_context sys_context, std::shared_ptr<shader_database> shader_database, uint32_t thread_render_context_count, view_port view_port) : 
        m_system_context(sys_context)
        , m_shader_database(shader_database)
        , m_per_pass_vertex_buffer( d3d11::create_constant_buffer( sys_context.m_device.get(), sizeof( cbuffer_constants_per_pass ) ) )
        , m_per_pass_pixel_buffer( d3d11::create_constant_buffer( sys_context.m_device.get(), sizeof( cbuffer_constants_per_pass ) ) )
        , m_depth_buffer( create_depth_resource( sys_context.m_device.get(), 320, 240 ) )
        , m_gbuffer_render_data ( sys_context.m_device.get(), m_per_pass_vertex_buffer ) 
        , m_depth_render_data( sys_context.m_device.get() )
        , m_light_buffer_render_data( sys_context.m_device.get() )
        , m_debug_render_data( sys_context.m_device.get() )
        , m_view_port(view_port)
        , m_screen_space_render_data ( sys_context.m_device.get() )
    {
        m_render_contexts.reserve(thread_render_context_count);

        for (uint32_t i = 0; i < thread_render_context_count; ++i)
        {
            d3d11::idevicecontext_ptr device_context;

            dx::throw_if_failed<d3d11::create_deferred_context_exception> ( sys_context.m_device->CreateDeferredContext(0, dx::get_pointer(device_context)));

            std::unique_ptr<thread_render_context> ptr(new thread_render_context(this, device_context) );
            m_render_contexts.push_back( std::move(ptr) );
        }

        m_depth_enable_state = create_depth_test_enable_state( sys_context.m_device.get() );
        m_depth_disable_state = create_depth_test_disable_state( sys_context.m_device.get() );

        m_opaque_state = create_opaque_blend_state( sys_context.m_device.get() );
        m_additive_state = create_additive_blend_state( sys_context.m_device.get() );

        m_default_sampler_state = create_default_sampler_state ( sys_context.m_device.get() );
        m_point_sampler_state = create_point_sampler_state ( sys_context.m_device.get() ) ;

        m_cull_back_raster_state = create_cull_back_rasterizer_state( sys_context.m_device.get() );

        create_swap_chain_buffers();
        create_gbuffer_state();
        create_depth_state();

        create_depth_buffer_layout();
        create_screen_space_input_layout();
        create_screen_space_vertex_buffer();

        create_default_render_data();
        create_light_buffer_render_data();
        create_debug_render_data();
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
        
        d3d11::idevicecontext_ptr device_context = m_render_contexts.front()->get_device_context();
        reset_state( device_context.get() );

        device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        device_context->RSSetState(m_cull_back_raster_state.get());
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

        //must be after the depth stencil
        create_debug_buffer();
    }

    void render_context::release_swap_chain_buffers()
    {
        m_back_buffer.reset();
        m_debug_render_data.reset();
    }

    void render_context::create_back_buffer_render_target()
    {
        d3d11::itexture2d_ptr	back_buffer = dxgi::get_buffer( m_system_context.m_swap_chain.get() );

        D3D11_RENDER_TARGET_VIEW_DESC view = {};

        D3D11_TEXTURE2D_DESC texture_description = {};

        back_buffer->GetDesc(&texture_description);

        view.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
        view.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

        m_back_buffer = d3d11::create_render_target_view( m_system_context.m_device.get(), back_buffer.get() );
    }

    void render_context::create_depth_buffer()
    {
        DXGI_SWAP_CHAIN_DESC desc = dxgi::get_desc( m_system_context.m_swap_chain.get() );
        m_depth_buffer = create_depth_resource(  m_system_context.m_device.get(), desc.BufferDesc.Width, desc.BufferDesc.Height ) ;
    }

    void render_context::create_diffuse_buffer()
    {
        DXGI_SWAP_CHAIN_DESC desc = dxgi::get_desc( m_system_context.m_swap_chain.get() );
        m_gbuffer_render_data.m_diffuse = create_diffuse_resource( m_system_context.m_device.get(), desc.BufferDesc.Width, desc.BufferDesc.Height);

    }

    void render_context::create_specular_buffer()
    {
        DXGI_SWAP_CHAIN_DESC desc = dxgi::get_desc( m_system_context.m_swap_chain.get() );
        m_gbuffer_render_data.m_specular = create_specular_resource( m_system_context.m_device.get(), desc.BufferDesc.Width, desc.BufferDesc.Height);
    }

    void render_context::create_normal_depth_buffer()
    {
        DXGI_SWAP_CHAIN_DESC desc = dxgi::get_desc( m_system_context.m_swap_chain.get() );
        m_gbuffer_render_data.m_normal = create_normal_resource( m_system_context.m_device.get(), desc.BufferDesc.Width, desc.BufferDesc.Height);
    }

    void render_context::create_light_buffer()
    {
        DXGI_SWAP_CHAIN_DESC desc = dxgi::get_desc( m_system_context.m_swap_chain.get() );

        m_light_buffer_render_data.m_light_buffer = create_light_buffer_resource(  m_system_context.m_device.get(),  desc.BufferDesc.Width, desc.BufferDesc.Height );

        m_light_buffer_render_data.m_read_depth_dsv = create_read_depth_stencil_view( m_system_context.m_device.get(), m_depth_buffer ) ;
        m_light_buffer_render_data.m_depth_srv = create_depth_resource_view( m_system_context.m_device.get(), m_depth_buffer );
    }

    void render_context::create_debug_buffer()
    {
        DXGI_SWAP_CHAIN_DESC desc = dxgi::get_desc( m_system_context.m_swap_chain.get() );
        m_debug_render_data.m_depth_buffer_copy = create_target_render_resource( m_system_context.m_device.get(), desc.BufferDesc.Width, desc.BufferDesc.Height, DXGI_FORMAT_R32_FLOAT );

        m_debug_render_data.m_read_depth_dsv = create_read_depth_stencil_view( m_system_context.m_device.get(), m_depth_buffer ) ;
        m_debug_render_data.m_depth_srv = create_depth_resource_view( m_system_context.m_device.get(), m_depth_buffer );
    }

    void render_context::clear_buffers(ID3D11DeviceContext* device_context)
    {
        device_context->ClearDepthStencilView( m_depth_buffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

        float clear_color_1[4] = { 0.449019607f, 0.449019607f, 0.449019607f, 1.0f };
        float clear_color_2[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        float clear_color_3[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

        device_context->ClearRenderTargetView( m_back_buffer.get(), clear_color_1);
        device_context->ClearRenderTargetView( m_gbuffer_render_data.m_normal, clear_color_2);
        device_context->ClearRenderTargetView( m_gbuffer_render_data.m_diffuse, clear_color_3);
        device_context->ClearRenderTargetView( m_gbuffer_render_data.m_specular, clear_color_3);

        device_context->ClearRenderTargetView( m_light_buffer_render_data.m_light_buffer, clear_color_3);
    }

    void render_context::reset_state(ID3D11DeviceContext* device_context)
    {
        clear_buffers(device_context);

        reset_shader_resources(device_context);
        reset_render_targets(device_context);
        reset_constant_buffers(device_context);

        device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        device_context->RSSetState(m_cull_back_raster_state.get());
    }

    void render_context::select_gbuffer(ID3D11DeviceContext* device_context, const per_view_data& view)
    {
        profile p(L"select_gbuffer");
        reset_render_targets( device_context );
        reset_shader_resources( device_context );

        ID3D11RenderTargetView* views[3] =
        { 
                                                m_gbuffer_render_data.m_diffuse,
                                                m_gbuffer_render_data.m_specular,
                                                m_gbuffer_render_data.m_normal
        };

        device_context->OMSetRenderTargets( 3, views, m_depth_buffer );
        device_context->OMSetBlendState( m_gbuffer_render_data.m_opaque.get(), nullptr, 0xFFFFFFFF);
        device_context->OMSetDepthStencilState( m_depth_enable_state.get(), 0 );

        ID3D11SamplerState* samplers[] =	{ 
                                                m_default_sampler_state.get(), 
                                                m_default_sampler_state.get(), 
                                                m_default_sampler_state.get(), 
                                                m_default_sampler_state.get()
                                            };

        device_context->PSSetSamplers( 0, sizeof(samplers)/sizeof(samplers[0]), &samplers[0] ); 
        select_view_port(device_context);


        cbuffer_constants_per_pass per_pass = 
        {
            view.m_view_matrix,
            view.m_projection_matrix,
            0.0f, 1.0f, 0.0f, 1.0f
        };

        constant_buffer_update( device_context, m_per_pass_vertex_buffer.get(), per_pass );
        device_context->VSSetConstantBuffers(slot_per_pass, 1, dx::get_pointer(m_per_pass_vertex_buffer));
        device_context->GSSetConstantBuffers(slot_per_pass, 1, dx::get_pointer(m_per_pass_vertex_buffer));
    }

    void render_context::select_light_buffer(ID3D11DeviceContext* device_context, const per_view_data& view)
    {
        profile p(L"select_light_buffer");
        reset_render_targets(device_context);
        reset_shader_resources(device_context);

        ID3D11RenderTargetView* views[] =
        { 
            m_light_buffer_render_data.m_light_buffer
        };

        device_context->OMSetRenderTargets( 1, &views[0], m_light_buffer_render_data.m_read_depth_dsv.get() );
        device_context->OMSetBlendState( m_additive_state.get(), nullptr, 0xFFFFFFFF);
        device_context->OMSetDepthStencilState( m_depth_disable_state.get() , 0 );

        ID3D11ShaderResourceView * shader_resource_view[] =
        {
                         m_gbuffer_render_data.m_normal,  
                         m_gbuffer_render_data.m_diffuse,
                         m_gbuffer_render_data.m_specular,
                         m_light_buffer_render_data.m_depth_srv.get() //depth buffer from the gbuffer pass
        };

        d3d11::ps_set_shader_resources( device_context, sizeof(shader_resource_view) / sizeof( shader_resource_view[0] ) , shader_resource_view);

        float zn = view.m_zn;
        float zf = view.m_zf;

        cbuffer_constants_per_pass per_pass_0 = 
        {
            math::identity_matrix(),
            math::identity_matrix(),
            0.0f, 1.0f, 0.0f, 1.0f,
        };

        constant_buffer_update( device_context, m_per_pass_vertex_buffer.get(), per_pass_0 );
        device_context->VSSetConstantBuffers(slot_per_pass, 1, dx::get_pointer(m_per_pass_vertex_buffer));

        cbuffer_constants_per_pass per_pass_1 = 
        {
            view.m_view_matrix,
            view.m_projection_matrix,
            0.0f, 1.0f, 1.0f / zn - 1.0f / zf , 1.0f / zn
        };

        constant_buffer_update( device_context, m_per_pass_pixel_buffer.get(), per_pass_1 );
        device_context->PSSetConstantBuffers(slot_per_pass, 1, dx::get_pointer(m_per_pass_pixel_buffer));
    }

    void render_context::end_light_buffer(ID3D11DeviceContext* device_context, const per_view_data&)
    {
        reset_render_targets(device_context);
        reset_shader_resources(device_context);
    }

    void render_context::end_gbuffer(ID3D11DeviceContext* device_context, const per_view_data&)
    {
        reset_render_targets(device_context);
        reset_shader_resources(device_context);
    }

    void render_context::select_back_buffer_target(ID3D11DeviceContext* device_context, const per_view_data&)
    {
        profile p(L"select_back_buffer_target");
        reset_render_targets(device_context);
        reset_shader_resources(device_context);

        device_context->OMSetBlendState(m_opaque_state.get(), nullptr, 0xFFFFFFFF);
        device_context->OMSetDepthStencilState(m_depth_enable_state.get(), 0 );

        ID3D11RenderTargetView* views[1] =
        {
            m_back_buffer.get()
        };

        device_context->OMSetRenderTargets( 1, &views[0], m_depth_buffer );
        device_context->RSSetState(m_cull_back_raster_state.get());

        ID3D11SamplerState* samplers[] =	{ 
                                                m_default_sampler_state.get(), 
                                                m_default_sampler_state.get(), 
                                                m_default_sampler_state.get(), 
                                                m_default_sampler_state.get(), 
                                            };

        device_context->PSSetSamplers( 0, sizeof(samplers)/sizeof(samplers[0]), &samplers[0] ); 
        select_view_port(device_context);
    }

    void render_context::select_debug_target(ID3D11DeviceContext* device_context, const per_view_data&)
    {
        profile p(L"select_debug_target");

        reset_render_targets(device_context);
        reset_shader_resources(device_context);

        device_context->OMSetBlendState( m_opaque_state.get(), nullptr, 0xFFFFFFFF );

        //copy the depth buffer after draw passes so we can debug it
        ID3D11RenderTargetView* views_1[1] =
        {
            m_debug_render_data.m_depth_buffer_copy
        };

        device_context->OMSetRenderTargets( 1, &views_1[0], m_debug_render_data.m_read_depth_dsv.get() );
        device_context->OMSetDepthStencilState(m_depth_disable_state.get(), 0 );

        ID3D11ShaderResourceView * shader_resource_view[] =
        {
            m_debug_render_data.m_depth_srv.get()
        };

        cbuffer_constants_per_pass per_pass_0 = 
        {
            math::identity_matrix(),
            math::identity_matrix(),
            0.0f, 1.0f, 0.0f, 1.0f
        };


        constant_buffer_update( device_context, m_per_pass_vertex_buffer.get(), per_pass_0 );
        device_context->VSSetConstantBuffers(slot_per_pass, 1, dx::get_pointer(m_per_pass_vertex_buffer));
        device_context->GSSetConstantBuffers(slot_per_pass, 1, dx::get_pointer(m_per_pass_vertex_buffer));

        d3d11::ps_set_shader_resources( device_context, sizeof(shader_resource_view) / sizeof( shader_resource_view[0] ) , shader_resource_view );
        d3d11::ps_set_shader( device_context, m_shader_database->m_copy_depth_pixel_shader );
        draw_screen_space_quad( device_context, this );
        
        reset_render_targets(device_context);
        reset_shader_resources(device_context);
        

        ID3D11RenderTargetView* views_2[1] =
        {
            m_back_buffer.get()
        };

        ID3D11SamplerState* samplers[] =	{ 
                                                m_default_sampler_state.get(), 
                                                m_default_sampler_state.get(), 
                                                m_default_sampler_state.get(), 
                                                m_default_sampler_state.get(), 
                                            };

        device_context->PSSetSamplers( 0, sizeof(samplers)/sizeof(samplers[0]), &samplers[0] ); 

        device_context->OMSetRenderTargets( 1, &views_2[0],  m_depth_buffer );
        device_context->OMSetDepthStencilState(m_depth_enable_state.get(), 0 );
        device_context->OMSetBlendState(m_opaque_state.get(), nullptr, 0xFFFFFFFF);
    }

    void render_context::select_depth_pass(ID3D11DeviceContext* device_context, const per_view_data&)
    {
        profile p(L"select_depth_pass");
        reset_render_targets(device_context);
        reset_shader_resources(device_context);

        device_context->IASetInputLayout(m_depth_render_data.m_input_layout);

        d3d11::ps_set_shader(device_context, nullptr);

        device_context->OMSetRenderTargets(0, nullptr, m_depth_buffer );
        device_context->OMSetBlendState(m_opaque_state.get(), nullptr, 0xFFFFFFFF);
        device_context->OMSetDepthStencilState(m_depth_enable_state.get(), 0 );

        m_depth_render_data.m_depth_constant_buffer.bind_as_vertex_constant_buffer(device_context);

        select_view_port(device_context);

        device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        device_context->RSSetState(m_cull_back_raster_state.get());
    }

    void render_context::end_depth_pass(ID3D11DeviceContext* device_context, const per_view_data&)
    {
        reset_shader_resources(device_context);
        reset_render_targets(device_context);
    }

    void render_context::compose_light_buffer(ID3D11DeviceContext* device_context, const per_view_data&)
    {
        reset_render_targets(device_context);
        reset_shader_resources(device_context);

        device_context->OMSetBlendState(m_opaque_state.get(), nullptr, 0xFFFFFFFF);
        device_context->OMSetDepthStencilState(m_depth_disable_state.get(), 0 );

        ID3D11RenderTargetView* views[1] =
        {
            m_back_buffer.get()
        };

        device_context->OMSetRenderTargets( 1, &views[0], nullptr );

        ID3D11SamplerState* samplers[] =	{ 
                                                m_point_sampler_state.get(), 
                                                m_point_sampler_state.get(),
                                                m_point_sampler_state.get(), 
                                                m_point_sampler_state.get()
                                            };

        device_context->PSSetSamplers( 0, sizeof(samplers) / sizeof(samplers[0]), &samplers[0] );

        d3d11::ps_set_shader(device_context, m_shader_database->m_encode_for_back_buffer_pixel_shader );
        d3d11::ps_set_shader_resources( device_context,  m_light_buffer_render_data.m_light_buffer );

        cbuffer_constants_per_pass per_pass = 
        {
            math::identity_matrix(),
            math::identity_matrix(),
            0.0f, 1.0f, 0.0, 1.0f
        };


        constant_buffer_update( device_context, m_per_pass_vertex_buffer.get(), per_pass );

        device_context->VSSetConstantBuffers(slot_per_pass, 1, dx::get_pointer(m_per_pass_vertex_buffer) );
        device_context->GSSetConstantBuffers(slot_per_pass, 1, dx::get_pointer(m_per_pass_vertex_buffer) );
        draw_screen_space_quad( device_context, this );
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

    }

    void render_context::create_light_buffer_state()
    {

    }

    void render_context::create_depth_state()
    {

    }

    void render_context::create_depth_buffer_layout()
    {
        m_depth_render_data.m_input_layout = m_shader_database->m_transform_position_input_layout;
    }

    void render_context::create_screen_space_input_layout()
    {
        m_screen_space_render_data.m_screen_space_input_layout = m_shader_database->m_transform_position_uv_input_layout;
    }

    screen_space_quad_render	render_context::create_screen_space_quad_render()
    {
        screen_space_quad_render quad_render  = { 
                                                      m_screen_space_render_data.m_screen_space_vertex_shader
                                                    , m_screen_space_render_data.m_screen_space_vertex_buffer.get()
                                                    , m_screen_space_render_data.m_screen_space_constant_buffer
                                                    , m_screen_space_render_data.m_screen_space_input_layout.get()
                                                };

        return std::move(quad_render);
    }

    void render_context::create_screen_space_vertex_buffer()
    {
        using namespace math;

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
             { {-1.0f,	-1.0f,	0.0f, 1.0f},  {0.0f, 1.0f}},
             { {-1.0f,	 1.0f,	0.0f, 1.0f},  {0.0f, 0.0f}},
             { {1.0f,	 1.0f,	0.0f, 1.0f},  {1.0f, 0.0f}},

             { {1.0f,	 1.0f,	0.0f, 1.0f} , {1.0f, 0.0f}},
             { {1.0f,	-1.0f,	0.0f, 1.0f} , {1.0f, 1.0f}},
             { {-1.0f,	-1.0f,	0.0f, 1.0f} , {0.0f, 1.0f}},

             { {0.0f,	0.0f,	0.0f, 0.0f} , {0.0f,0.0f}},	//padding
             { {0.0f,	0.0f,	0.0f, 0.0f} , {0.0f,0.0f}},	//padding
        };

        __declspec( align(16) ) math::half h_1 [ 40 ];

        math::convert_f32_f16_stream(reinterpret_cast<const float*> (&v_1[0]), static_cast<uint32_t>(40), &h_1[0] );

        m_screen_space_render_data.m_screen_space_vertex_buffer = d3d11::create_immutable_vertex_buffer( m_system_context.m_device.get(), &h_1[0],  6 * sizeof(vertex) );
    }

    void render_context::create_default_render_data()
    {
    }

    void render_context::create_light_buffer_render_data()
    {

    }

    void render_context::create_debug_render_data()
    {

    }
}

