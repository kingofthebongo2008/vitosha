#ifndef __GX_RENDER_CONTEXT_H__
#define __GX_RENDER_CONTEXT_H__

#include <memory>
#include <vector>
#include <cstdint>
#include <boost/noncopyable.hpp>

#include <dx11/dx11_pointers.h>
#include <dx11/dx11_error.h>
#include <dx11/dx11_system.h>

#include <gx/gx_color_pixel_shader.h>
#include <gx/gx_depth_shader.h>
#include <gx/gx_lambert_constant_pixel_shader.h>
#include <gx/gx_lambert_vertex_shader.h>
#include <gx/gx_lambert_pixel_shader.h>
#include <gx/gx_phong_vertex_shader.h>
#include <gx/gx_screen_space_quad.h>
#include <gx/gx_screen_space_vertex_shader.h>
#include <gx/gx_screen_space_pixel_shader.h>
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
		}

		dx11::id3d11rendertargetview_ptr		m_normal_depth_target;
		dx11::id3d11rendertargetview_ptr		m_diffuse_target;
		dx11::id3d11rendertargetview_ptr		m_specular_target;

		dx11::id3d11texture2d_ptr				m_depth_stencil;
		dx11::id3d11depthstencilview_ptr		m_depth_stencil_target;

		dx11::id3d11texture2d_ptr				m_normal_depth;
		dx11::id3d11texture2d_ptr				m_diffuse;
		dx11::id3d11texture2d_ptr				m_specular;
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

	struct gbuffer_state : public render_state
	{

	};

	struct depth_state : public render_state
	{
	};

	struct gbuffer_render_data
	{
		gbuffer_state m_state;
		gbuffer_render_set m_render_set;
	};

	struct depth_render_data
	{
		explicit depth_render_data  ( ID3D11Device* device ) :
			m_depth_vertex_shader(device)
		,   m_depth_constant_buffer(device)
		{

		}

		depth_state												m_state;
		depth_render_set										m_render_set;
		dx11::id3d11inputlayout_ptr								m_input_layout;
		depth_vertex_shader										m_depth_vertex_shader;
		depth_vertex_shader_constant_buffer						m_depth_constant_buffer;
	};

	struct screen_space_render_data
	{
		explicit screen_space_render_data (ID3D11Device* device) :
			m_screen_space_vertex_shader(device)
			, m_screen_space_constant_buffer(device)
		{

		}

		dx11::id3d11inputlayout_ptr								m_screen_space_input_layout;
		screen_space_vertex_shader								m_screen_space_vertex_shader;
		screen_space_vertex_shader_constant_buffer				m_screen_space_constant_buffer;
		dx11::id3d11buffer_ptr									m_screen_space_vertex_buffer;
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

    class render_context : public boost::noncopyable
    {
        public:
		typedef  std::vector< std::unique_ptr<thread_render_context> > thread_render_context_container;

		render_context(dx11::system_context sys_context, std::uint32_t render_context_count, view_port view_port);
		~render_context();

        void begin_frame();
		void end_frame();

		void create_swap_chain_buffers();
		void release_swap_chain_buffers();

		void clear_buffers( ID3D11DeviceContext* device_context);
		void select_depth_pass(ID3D11DeviceContext* device_context);
		void select_gbuffer(ID3D11DeviceContext* device_context);
		void select_back_buffer_target(ID3D11DeviceContext* device_context);

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

		void create_gbuffer_state();
		void create_depth_state();

		void create_depth_buffer_layout();
		void create_screen_space_input_layout();
		void create_lambert_input_layout();
		void create_screen_space_vertex_buffer();

		void create_default_render_data();

		void select_view_port(ID3D11DeviceContext* device_context);

        dx11::system_context									m_system_context;
        thread_render_context_container							m_render_contexts;

		dx11::id3d11rendertargetview_ptr						m_back_buffer_render_target;

		dx11::id3d11texture2d_ptr								m_depth_stencil;
		dx11::id3d11depthstencilview_ptr						m_depth_stencil_target;

		public:

		gbuffer_render_data										m_gbuffer_render_data;
		depth_render_data										m_depth_render_data;
		default_render_data										m_default_render_data;

		view_port												m_view_port;
		screen_space_render_data								m_screen_space_render_data;
		
		color_pixel_shader										m_color_pixel_shader;
		color_pixel_shader_constant_buffer						m_color_pixel_shader_cbuffer;

		phong_vertex_shader										m_phong_vertex_shader;
		phong_vertex_shader_constant_buffer						m_phong_vertex_shader_cbuffer;


		lambert_vertex_shader									m_lambert_vertex_shader;
		lambert_vertex_shader_constant_buffer					m_lambert_vertex_shader_cbuffer;

		lambert_constant_pixel_shader							m_lambert_constant_pixel_shader;
		lambert_constant_pixel_shader_constant_buffer			m_lambert_pixel_cbuffer;

		dx11::id3d11inputlayout_ptr								m_lambert_input_layout;

    };
}



#endif

