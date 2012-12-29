#include "precompiled.h"

#include <win32/window.h>

#include <memory>

#include <dx/dxgi_pointers.h>
#include <d3d11/dxgi_helpers.h>

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
	window::window(HWND hwnd, application& application, dxgi::iswapchain_ptr swap_chain, gx::render_context* render_context, gx::target_render_resource d2d_resource  ) : 
			m_hwnd(hwnd)
			, m_application(application)
			, m_swap_chain(swap_chain)
			, m_render_context(render_context)
			, m_occluded_by_another_window(false)
			, m_d2d_resource(d2d_resource)
	{

		math::float4  view_position_ws = math::set( 0.0f, 5.0f,  -5.0f, 0.0f ); //meters
		math::float4  view_direction_ws  = math::set( 0.0f, 0.0f,  1.0f, 0.0f ); //look along the z
		math::float4  view_up_ws = math::set( 0.0f, 1.0f, 0.0f, 0.0f );  //up vector

		m_main_camera.set_view_position(view_position_ws);
		m_main_camera.set_view_direction(view_direction_ws);
		m_main_camera.set_view_up(view_up_ws);

		m_main_camera.set_aspect_ratio(16.0f / 9.0f);
		m_main_camera.set_fov(3.1415f / 4.0f );
		m_main_camera.set_near(1.0f);
		m_main_camera.set_far(200.f); //meters


		m_d2d_factory = d2d::create_d2d_factory();
		m_dwrite_factory = dwrite::create_dwrite_factory();
		m_text_format = dwrite::create_text_format(m_dwrite_factory);
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
				dx::throw_if_failed<d3d11::d3d11_exception>(hr);		
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
				dx::throw_if_failed<d3d11::d3d11_exception>(hr);		
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


		/*
		//render text
		m_d2d_render_target->BeginDraw();
		m_d2d_render_target->Clear();

		RECT r;
		::GetClientRect(m_hwnd, &r);
		::InflateRect(&r, -10, -10 );
	
		D2D1_RECT_F rf = {static_cast<float> (r.left), static_cast<float>(r.top), static_cast<float>(r.right), static_cast<float>(r.bottom)};
	

		m_d2d_render_target->DrawTextW(L"Testo", 4, m_text_format.get(), &rf, m_d2d_brush.get());

		m_d2d_render_target->EndDraw();

		ID3D11DeviceContext* device_context = m_render_context->get_immediate_context();
		d3d11::ps_set_shader(device_context, m_render_context->m_color_texture_pixel_shader );
        d3d11::ps_set_shader_resources( device_context,  m_d2d_resource );
		gx::draw_screen_space_quad(device_context, m_render_context, math::identity_matrix());
		*/
	}

	void window::resize_window(uint32_t width, uint32_t height)
	{
		using namespace d3d11;
		DXGI_SWAP_CHAIN_DESC desc = {};

		//disable dxgi errors
		width = std::max(width, (uint32_t)(8));
		height = std::max(height, (uint32_t)(8));

		//release buffers that are window size
		m_render_context->release_swap_chain_buffers();

		dx::throw_if_failed<d3d11_exception>(m_swap_chain->GetDesc(&desc));
		dx::throw_if_failed<d3d11_exception>(m_swap_chain->ResizeBuffers(desc.BufferCount, width, height,  desc.BufferDesc.Format, desc.Flags));

		//create a new viewport
		gx::view_port view_port ( 0, 0, width, height );
		m_render_context->set_view_port(view_port);
		m_view_port = view_port;

		m_main_camera.set_aspect_ratio ( static_cast<float>(width) / static_cast<float>(height) );

		m_render_context->create_swap_chain_buffers();

		ID3D11Device*	device = m_render_context->get_device();
		m_d2d_resource	= gx::create_target_render_resource( device, width, height, DXGI_FORMAT_B8G8R8A8_UNORM );

		dxgi::isurface_ptr surface;
		ID3D11Texture2D* texture = m_d2d_resource;
		dx::throw_if_failed<dx::exception>( texture->QueryInterface( IID_IDXGISurface, reinterpret_cast<void**> (&surface) ) );
		m_d2d_render_target = d2d::create_render_target(m_d2d_factory, surface);

		m_d2d_brush = d2d::create_solid_color_brush(m_d2d_render_target);
	}

	void window::process_user_input()
	{
        const float movement_camera_speed = 0.06f;
        const float rotation_camera_speed = 0.002f * 3.1415f;

		if (m_pad_state.is_button_down<io::pad_state::button_6>())
		{
			const io::mouse_state::difference	differences = m_mouse_state.get_difference();
			
			auto		m = gx::create_inverse_perspective_matrix(&m_main_camera);
			auto		mouse_coordinates = m_mouse_state.get_coordinates();
			auto		point = math::set( static_cast<float> (std::get<0>( mouse_coordinates ) ),  -static_cast<float> ( std::get<1>( mouse_coordinates ) ) , 0.0f, 1.0f );
			auto		point_vs = math::unproject( point, m, m_view_port );
			auto		radius = 0.25f;
			auto		center = math::unproject( math::identity_r3(), m, m_view_port );

			if ( m_mouse_state.is_left_button_down() &&
				io::has_difference<io::mouse_state::left_button>(differences) )
			{
				//down
				auto	down = math::arc_ball_point_on_unit_sphere(point_vs, radius, center );

				m_arcball_state.dragging = true;
				
				m_arcball_state.initial_rotation = math::identity_r3();
				m_arcball_state.initial_point = down;

				m_arcball_state.initial_camera_up	= m_main_camera.get_up();
				m_arcball_state.initial_camera_direction = m_main_camera.get_view_direction();
				m_arcball_state.initial_center = center;

				m_arcball_state.initial_rotation = math::identity_r3();

			}
			else
			if ( !m_mouse_state.is_left_button_down() &&
				io::has_difference<io::mouse_state::left_button>(differences) )
			{
				//up
				m_arcball_state.dragging = false;
			}
			else
			{
				//move
				if (m_arcball_state.dragging)
				{
					auto center = m_arcball_state.initial_center;
					auto down = math::arc_ball_point_on_unit_sphere(point_vs, radius, center );
					
					//create rotation from 2 points on the sphere
					auto rotation = math::arc_ball_quaternion(m_arcball_state.initial_point, down);

					auto new_rotation = math::quaternion_mul(m_arcball_state.initial_rotation, rotation);

					//rotate
					m_arcball_state.current_rotation = new_rotation;

					auto	up = math::rotate_vector3(m_arcball_state.initial_camera_up, new_rotation);
					auto	direction = math::rotate_vector3(m_arcball_state.initial_camera_direction, new_rotation);

					m_main_camera.set_view_up(up);
					m_main_camera.set_view_direction(direction);
					
				}
			}
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