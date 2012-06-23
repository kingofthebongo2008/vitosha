#include "precompiled.h"

#include <win32/window.h>

#include <memory>

#include <math/math_graphics.h>

#include <fnd/fnd_universe.h>

#include <gx/gx_create_draw_calls_pipeline_node.h>
#include <gx/gx_execute_draw_calls_pipeline_node.h>
#include <gx/gx_final_pipeline_node.h>
#include <gx/gx_pinhole_camera.h>
#include <gx/gx_pipeline.h>
#include <gx/gx_render_context.h>
#include <gx/gx_scene.h>
#include <gx/gx_scene_pipeline_node.h>

#include <gx/gx_view.h>
#include <gx/gx_view_pipeline_node.h>

#include <gxu/gxu_pinhole_camera_dispatcher.h>

#include <win32/application.h>

#include <math/xnamath.h>

namespace wnd
{
	window::window(application& application, dx11::idxgiswapchain_ptr swap_chain, gx::render_context* render_context ) : 
			m_application(application)
			, m_swap_chain(swap_chain)
			, m_render_context(render_context)
			, m_occluded_by_another_window(false)
	{

		math::float4  view_position_ws = math::set( 0.0f, 0.0f,  -5.0f, 0.0f ); //meters
		math::float4  view_direction_ws  = math::set( 0.0f, 0.0f,  1.0f, 0.0f ); //look along the z
		math::float4  view_up_ws = math::set( 0.0f, 1.0f, 0.0f, 0.0f );  //up vector

		m_main_camera.set_view_position(view_position_ws);
		m_main_camera.set_view_direction(view_direction_ws);
		m_main_camera.set_view_up(view_up_ws);

		m_main_camera.set_aspect_ratio(16.0f / 9.0f);
		m_main_camera.set_fov(3.1415f / 4.0f );
		m_main_camera.set_near(1.0f);
		m_main_camera.set_far(200.f); //meters

	}

	window::~window()
	{

	}

	void window::destroy()
	{
		m_application.delete_window(this);
		delete this;
	}

	void window::render()
	{
		if (m_occluded_by_another_window)
		{
			HRESULT hr = m_swap_chain->Present(0, DXGI_PRESENT_TEST );

			if ( hr == S_OK)
			{
				m_occluded_by_another_window = false;
			}

			if (hr != DXGI_STATUS_OCCLUDED)
			{
				dx11::throw_if_failed<dx11::d3d11_exception>(hr);		
			}
		}
		else
		{
			render_frame();

			HRESULT hr = m_swap_chain->Present(0,0);

			if (hr == DXGI_STATUS_OCCLUDED)
			{
				m_occluded_by_another_window = true;
			}
			else
			{
				dx11::throw_if_failed<dx11::d3d11_exception>(hr);		
			}
		}
	}

	void window::render_frame()
	{
		gx::pipeline    pipeline;
		
		math::float4x4 view_matrix = gx::create_view_matrix(&m_main_camera);
		math::float4x4 perspective_matrix = gx::create_perspective_matrix(&m_main_camera);

		gx::view			 view( view_matrix, perspective_matrix );

		pipeline.add_node( std::make_shared< gx::scene_pipeline_node>(m_scene.get()) );

		pipeline.add_node( std::make_shared< gx::view_pipeline_node>(&view) );
		pipeline.add_node( std::make_shared< gx::create_draw_calls_pipeline_node>() );
		pipeline.add_node( std::make_shared< gx::execute_draw_calls_pipeline_node>(m_render_context) );

		pipeline.add_node( std::make_shared< gx::final_pipeline_node>() );
		pipeline.process();
	}

	void window::resize_window(uint32_t width, uint32_t height)
	{
		using namespace dx11;
		DXGI_SWAP_CHAIN_DESC desc = {};

		//disable dxgi errors
		width = std::max(width, (uint32_t)(8));
		height = std::max(height, (uint32_t)(8));

		//release buffers that are window size
		m_render_context->release_swap_chain_buffers();

		throw_if_failed<d3d11_exception>(m_swap_chain->GetDesc(&desc));
		throw_if_failed<d3d11_exception>(m_swap_chain->ResizeBuffers(desc.BufferCount, width, height,  desc.BufferDesc.Format, desc.Flags));

		//create a new viewport
		gx::view_port view_port ( 0, 0, width, height );
		m_render_context->set_view_port(view_port);
		m_view_port = view_port;

		m_main_camera.set_aspect_ratio ( static_cast<float>(width) / static_cast<float>(height) );

		m_render_context->create_swap_chain_buffers();
	}

	void window::process_user_input()
	{
        const float movement_camera_speed = 0.002f;
        const float rotation_camera_speed = 0.0001f * 3.1415f;

		if (m_pad_state.is_button_down<io::pad_state::button_6>())
		{
			io::mouse_state::difference	differences = m_mouse_state.get_difference();
			
			math::float4x4		p = gx::create_perspective_matrix(&m_main_camera);
			math::float4x4		m = math::inverse( p );
			std::tuple<uint32_t, uint32_t> mouse_coordinates = m_mouse_state.get_coordinates();
			math::float4		point = math::set( std::get<0>( mouse_coordinates ),  std::get<1>( mouse_coordinates ) , 0.0f, 1.0f );
			math::float4		point_vs = math::unproject( point, m, m_view_port );

			XMMATRIX p_1;
			p_1.r[0] = p.r[0];
			p_1.r[1] = p.r[1];
			p_1.r[2] = p.r[2];
			p_1.r[3] = p.r[3];

			XMMATRIX m_1 = XMMatrixIdentity();

			math::float4		p_2 = XMVector3Unproject(point,
				m_view_port.get_left(),
				m_view_port.get_top(),
				m_view_port.get_width(),
				m_view_port.get_height(),
				m_view_port.get_min_z(),
				m_view_port.get_max_z(),
				p_1,
				m_1,
				m_1);




			static int k=0;
			k++;


		}
		else
		if (m_pad_state.is_button_down<io::pad_state::button_0>())
		{
			gxu::camera_command command = gxu::create_move_forward_command(movement_camera_speed);
			gxu::pinhole_camera_command_dispatcher procesor(&m_main_camera);
			procesor.process(&command);
		}
		else
		if (m_pad_state.is_button_down<io::pad_state::button_1>())
		{
			gxu::camera_command command = gxu::create_move_backward_command(movement_camera_speed);
			gxu::pinhole_camera_command_dispatcher procesor(&m_main_camera);
			procesor.process(&command);
		}
		else
		if (m_pad_state.is_button_down<io::pad_state::button_2>())
		{
			gxu::camera_command command = gxu::create_turn_camera_left_command(rotation_camera_speed);
			gxu::pinhole_camera_command_dispatcher procesor(&m_main_camera);
			procesor.process(&command);
		}
		else
		if (m_pad_state.is_button_down<io::pad_state::button_3>())
		{
			gxu::camera_command command = gxu::create_turn_camera_right_command(rotation_camera_speed);
			gxu::pinhole_camera_command_dispatcher procesor(&m_main_camera);
			procesor.process(&command);
		}
		else
		if (m_pad_state.is_button_down<io::pad_state::button_4>())
		{
			gxu::camera_command command = gxu::create_aim_camera_up_command(rotation_camera_speed);
			gxu::pinhole_camera_command_dispatcher procesor(&m_main_camera);
			procesor.process(&command);
		}
		else
		if (m_pad_state.is_button_down<io::pad_state::button_5>())
		{
			gxu::camera_command command = gxu::create_aim_camera_down_command(rotation_camera_speed);
			gxu::pinhole_camera_command_dispatcher procesor(&m_main_camera);
			procesor.process(&command);
		}

		//save history for the previous frame
		m_mouse_state.swap();
		m_pad_state.swap();
	}

	void window::on_mouse_down(uint32_t windows_mouse_state)
	{
		uint32_t mouse_state = io::create_mouse_state( windows_mouse_state );
		m_mouse_state.set_mask(mouse_state);
	}

	void window::on_mouse_up(uint32_t windows_mouse_state)
	{
		uint32_t mouse_state = io::create_mouse_state( windows_mouse_state );
		m_mouse_state.reset_mask(mouse_state);
	}

	void window::on_mouse_move(uint32_t windows_mouse_state, uint16_t x, uint16_t y)
	{
		uint32_t mouse_state = io::create_mouse_state( windows_mouse_state );

		m_mouse_state.set_coordinates(x, y);
		m_mouse_state.set_state(mouse_state);
	}

	void  window::on_button_0_down()
	{
		m_pad_state.button_down<io::pad_state::button_0>();
	}

	void  window::on_button_1_down()
	{
		m_pad_state.button_down<io::pad_state::button_1>();
	}

	void  window::on_button_2_down()
	{
		m_pad_state.button_down<io::pad_state::button_2>();
	}

	void  window::on_button_3_down()
	{
		m_pad_state.button_down<io::pad_state::button_3>();
	}

	void  window::on_button_4_down()
	{
		m_pad_state.button_down<io::pad_state::button_4>();
	}

	void  window::on_button_5_down()
	{
		m_pad_state.button_down<io::pad_state::button_5>();
	}

	void  window::on_button_6_down()
	{
		m_pad_state.button_down<io::pad_state::button_6>();
	}

	void  window::on_button_0_up()
	{
		m_pad_state.button_up<io::pad_state::button_0>();
	}

	void  window::on_button_1_up()
	{
		m_pad_state.button_up<io::pad_state::button_1>();
	}

	void  window::on_button_2_up()
	{
		m_pad_state.button_up<io::pad_state::button_2>();
	}

	void  window::on_button_3_up()
	{
		m_pad_state.button_up<io::pad_state::button_3>();
	}

	void  window::on_button_4_up()
	{
		m_pad_state.button_up<io::pad_state::button_4>();
	}

	void  window::on_button_5_up()
	{
		m_pad_state.button_up<io::pad_state::button_5>();
	}

	void  window::on_button_6_up()
	{
		m_pad_state.button_up<io::pad_state::button_6>();
	}
}