#ifndef __GX_RENDER_CONTEXT_H__
#define __GX_RENDER_CONTEXT_H__

#include <memory>
#include <vector>
#include <cstdint>
#include <boost/noncopyable.hpp>

#include <dx11/dx11_pointers.h>
#include <dx11/dx11_error.h>
#include <dx11/dx11_system.h>

#include <gx/gx_blinn_phong_shift_invariant_pixel_shader.h>

#include <gx/gx_color_pixel_shader.h>
#include <gx/gx_color_texture_pixel_shader.h>
#include <gx/gx_color_texture_channel_3_pixel_shader.h>

#include <gx/gx_debug_view_space_depth_pixel_shader.h>

#include <gx/gx_lambert_pixel_shader.h>
#include <gx/gx_lambert_shift_invariant_pixel_shader.h>
#include <gx/gx_screen_space_quad.h>

#include <gx/gx_screen_space_uv_vertex_shader.h>

#include <gx/gx_transform_position_vertex_shader.h>
#include <gx/gx_transform_position_uv_vertex_shader.h>
#include <gx/gx_transform_position_normal_uv_vertex_shader.h>

#include <gx/gx_view_port.h>

namespace gx
{
	struct gbuffer_render_set
	{
		inline void reset()
		{
			m_normal_depth_target.reset();
			m_diffuse_target.reset();
			m_specular_target.reset();

			m_depth_stencil_target.reset();
			m_depth_stencil.reset();
			
			m_normal_depth.reset();
			m_diffuse.reset();
			m_specular.reset();

			m_normal_depth_view.reset();
			m_diffuse_view.reset();
			m_specular_view.reset();
		}

		dx11::id3d11rendertargetview_ptr		m_normal_depth_target;
		dx11::id3d11rendertargetview_ptr		m_diffuse_target;
		dx11::id3d11rendertargetview_ptr		m_specular_target;

		dx11::id3d11texture2d_ptr				m_depth_stencil;
		dx11::id3d11depthstencilview_ptr		m_depth_stencil_target;

		dx11::id3d11texture2d_ptr				m_normal_depth;
		dx11::id3d11texture2d_ptr				m_diffuse;
		dx11::id3d11texture2d_ptr				m_specular;

		dx11::id3d11shaderresourceview_ptr		m_normal_depth_view;
		dx11::id3d11shaderresourceview_ptr		m_diffuse_view;
		dx11::id3d11shaderresourceview_ptr		m_specular_view;
	};


    struct light_buffer_render_set
	{
		inline void reset()
		{
			m_normal_depth.reset();
			m_diffuse.reset();
			m_specular.reset();

			m_depth_stencil_target.reset();
			m_depth_stencil.reset();
            m_depth_stencil_view.reset();
			
			m_normal_depth.reset();
			m_diffuse.reset();
			m_specular.reset();

            m_light_buffer_target.reset();
            m_light_buffer.reset();
            m_light_buffer_view.reset();
		}

        //accumulate lights here
		dx11::id3d11rendertargetview_ptr		m_light_buffer_target;
        dx11::id3d11shaderresourceview_ptr		m_light_buffer_view;
        dx11::id3d11texture2d_ptr				m_light_buffer;

        //depth from the gbuffer pass
        dx11::id3d11texture2d_ptr				m_depth_stencil;
		dx11::id3d11depthstencilview_ptr		m_depth_stencil_target;
        dx11::id3d11shaderresourceview_ptr		m_depth_stencil_view;

        //sampled light buffer parameters
		dx11::id3d11texture2d_ptr				m_normal_depth;
		dx11::id3d11texture2d_ptr				m_diffuse;
		dx11::id3d11texture2d_ptr				m_specular;

		dx11::id3d11shaderresourceview_ptr		m_normal_depth_view;
		dx11::id3d11shaderresourceview_ptr		m_diffuse_view;
		dx11::id3d11shaderresourceview_ptr		m_specular_view;

	};

	struct depth_render_set
	{
		void reset()
		{
			m_depth_stencil_target.reset();
			m_depth_stencil.reset();
		}

		dx11::id3d11texture2d_ptr				m_depth_stencil;
		dx11::id3d11depthstencilview_ptr		m_depth_stencil_target;
	};

	struct render_state
	{
		dx11::id3d11samplerstate_ptr			m_sampler;
		dx11::id3d11depthstencilstate_ptr		m_depth;
		dx11::id3d11blendstate_ptr				m_blend_opaque;
		dx11::id3d11rasterizerstate_ptr			m_rasterizer;
	};

    typedef render_state gbuffer_state;
    typedef render_state depth_state;
    typedef render_state light_buffer_state;

	struct gbuffer_render_data
	{
		gbuffer_state           m_state;
		gbuffer_render_set      m_render_set;
	};

    struct light_buffer_render_data
    {
        light_buffer_state      m_state;
        light_buffer_render_set m_render_set;
    };

	struct depth_render_data
	{
		explicit depth_render_data  ( ID3D11Device* device ) :
			m_depth_vertex_shader(device)
		,   m_depth_constant_buffer(device)
        ,   m_input_layout(device, &m_depth_vertex_shader )
		{

		}

		depth_state												m_state;
		depth_render_set										m_render_set;
		transform_position_vertex_shader						m_depth_vertex_shader;
        transform_position_vertex_shader_constant_buffer		m_depth_constant_buffer;
        transform_position_input_layout							m_input_layout;
	};

	struct screen_space_render_data
	{
		explicit screen_space_render_data (ID3D11Device* device) :
			m_screen_space_vertex_shader(device)
			, m_screen_space_constant_buffer(device)
		{

		}

		dx11::id3d11inputlayout_ptr								m_screen_space_input_layout;
		transform_position_uv_vertex_shader						m_screen_space_vertex_shader;
		transform_position_uv_vertex_shader_constant_buffer		m_screen_space_constant_buffer;
        dx11::id3d11buffer_ptr	                                m_screen_space_vertex_buffer;
	};

	struct default_render_state : public render_state
	{

	};

	struct default_render_set
	{
		inline void reset()
		{
			m_back_buffer_render_target.reset();
			m_depth_stencil.reset();
			m_depth_stencil_target.reset();
		}

		dx11::id3d11rendertargetview_ptr		m_back_buffer_render_target;
		dx11::id3d11texture2d_ptr				m_depth_stencil;
		dx11::id3d11depthstencilview_ptr		m_depth_stencil_target;
	};

	struct default_render_data
	{
		default_render_state					m_state;
		default_render_set						m_render_set;
	};

    class thread_render_context;

	//performs management of surfaces, gpu memory buffers, shaders, textures on a higher level than a 3d device
	//has knowledge about a fixed rendering method and material system
    class render_context : public boost::noncopyable
    {
        public:
		typedef  std::vector< std::unique_ptr<thread_render_context> > thread_render_context_container;

		render_context(dx11::system_context sys_context, uint32_t render_context_count, view_port view_port);
		~render_context();

        void begin_frame();
		void end_frame();

		void create_swap_chain_buffers();
		void release_swap_chain_buffers();

		void clear_state( ID3D11DeviceContext* device_context );
		void clear_buffers( ID3D11DeviceContext* device_context);
		void select_depth_pass(ID3D11DeviceContext* device_context);
		void select_gbuffer(ID3D11DeviceContext* device_context);
		void select_back_buffer_target(ID3D11DeviceContext* device_context);
        void select_light_buffer(ID3D11DeviceContext* device_context);

        void end_depth_pass(ID3D11DeviceContext* device_context);
		void end_light_buffer(ID3D11DeviceContext* device_context);
        void end_gbuffer(ID3D11DeviceContext* device_context);

        screen_space_quad_render	create_screen_space_quad_render();

		inline thread_render_context_container::iterator begin()
		{
			return m_render_contexts.begin();
		}

		inline thread_render_context_container::const_iterator begin() const
		{
			return m_render_contexts.begin();
		}

		inline thread_render_context_container::iterator end()
		{
			return m_render_contexts.end();
		}

		inline thread_render_context_container::const_iterator end() const
		{
			return m_render_contexts.end();
		}

		inline std::unique_ptr<thread_render_context>& front()
		{
			return m_render_contexts.front();
		}

		inline const std::unique_ptr<thread_render_context>& front() const
		{
			return m_render_contexts.front();
		}

		inline void set_view_port ( view_port value )
		{
			m_view_port = value;
		}

		inline ID3D11DeviceContext* get_immediate_context()
		{
			return m_system_context.m_immediate_context.get();
		}

		inline const ID3D11DeviceContext* get_immediate_context() const
		{
			return m_system_context.m_immediate_context.get();
		}

		inline ID3D11Device* get_device()
		{
			return m_system_context.m_device.get();
		}

		inline const ID3D11Device* get_device() const
		{
			return m_system_context.m_device.get();
		}

		private:

		render_context();

		void create_back_buffer_render_target();
		void create_depth_buffer();

		void create_diffuse_buffer();
		void create_specular_buffer();
		void create_normal_depth_buffer();
        void create_light_buffer();

		void create_gbuffer_state();
		void create_depth_state();
        void create_light_buffer_state();
       

        void create_depth_buffer_layout();
		void create_screen_space_input_layout();

		void create_screen_space_vertex_buffer();

		void create_default_render_data();
        void create_light_buffer_render_data();

		void select_view_port(ID3D11DeviceContext* device_context);

		void reset_shader_resources(ID3D11DeviceContext* device_context);
		void reset_render_targets(ID3D11DeviceContext* device_context);

        dx11::system_context									m_system_context;
        thread_render_context_container							m_render_contexts;

		dx11::id3d11rendertargetview_ptr						m_back_buffer_render_target;

		dx11::id3d11texture2d_ptr								m_depth_stencil;
		dx11::id3d11depthstencilview_ptr						m_depth_stencil_target;

		public:

		gbuffer_render_data										m_gbuffer_render_data;
		depth_render_data										m_depth_render_data;
		default_render_data										m_default_render_data;
        light_buffer_render_data                                m_light_buffer_render_data;

		view_port												m_view_port;
		screen_space_render_data								m_screen_space_render_data;

        transform_position_vertex_shader						m_transform_position_vertex_shader;
        transform_position_vertex_shader_constant_buffer        m_transform_position_vertex_shader_cbuffer;
		transform_position_input_layout                         m_transform_position_input_layout;

        transform_position_uv_vertex_shader						m_transform_position_uv_vertex_shader;
        transform_position_uv_vertex_shader_constant_buffer     m_transform_position_uv_vertex_shader_cbuffer;
		transform_position_uv_input_layout                      m_transform_position_uv_input_layout;

        transform_position_normal_uv_vertex_shader						m_transform_position_normal_uv_vertex_shader;
        transform_position_normal_uv_vertex_shader_constant_buffer      m_transform_position_normal_uv_vertex_shader_cbuffer;
		transform_position_normal_uv_input_layout                       m_transform_position_normal_uv_input_layout;


		color_pixel_shader										    m_color_pixel_shader;
		color_pixel_shader_constant_buffer						    m_color_pixel_shader_cbuffer;

		color_texture_pixel_shader								    m_color_texture_pixel_shader;
        color_texture_channel_3_pixel_shader					    m_color_texture_channel_3_pixel_shader;

        debug_view_space_depth_pixel_shader                         m_debug_view_space_depth_pixel_shader;
        debug_view_space_depth_pixel_shader_constant_buffer         m_debug_view_space_depth_pixel_shader_cbuffer;

        screen_space_uv_vertex_shader                               m_screen_space_uv_vertex_shader;            
        screen_space_uv_vertex_shader_constant_buffer               m_screen_space_uv_vertex_shader_cbuffer;
        
		lambert_shift_invariant_pixel_shader					    m_lambert_shift_invariant_pixel_shader;
		lambert_shift_invariant_pixel_shader_constant_buffer	    m_lambert_pixel_cbuffer;

        blinn_phong_shift_invariant_pixel_shader				    m_blinn_phong_shift_invariant_pixel_shader;
		blinn_phong_shift_invariant_pixel_shader_constant_buffer	m_blinn_phong_pixel_cbuffer;

    };
}



#endif

