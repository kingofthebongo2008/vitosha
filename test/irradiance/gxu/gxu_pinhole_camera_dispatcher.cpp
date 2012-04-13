#include "precompiled.h"

#include <gxu/gxu_pinhole_camera_dispatcher.h>

namespace gxu
{
	namespace
	{
		void turn_pinhole_camera(gx::pinhole_camera* camera, float angle_in_radians)
		{
			//use matrix computations, since we do not have quaternion library still
			auto	view_direction_vs			= math::set(0.0f, 0.0f, 1.0f, 0.0f);
			auto	up_direction_vs				= math::set(0.0f, 1.0f, 0.0f, 0.0f);
			auto	rotation					= math::rotation_y( angle_in_radians );
			auto	rotated_view_direction_vs	= math::mul( rotation, math::negate(view_direction_vs) );
			auto	rotated_up_direction_vs		= math::mul( rotation,up_direction_vs );
			auto	view_matrix					= gx::create_view_matrix(camera);
			auto	inverse_view_matrix			= math::inverse( view_matrix );
						
			auto	view_direction_ws			= math::mul ( rotated_view_direction_vs, inverse_view_matrix );
			auto	up_direction_ws				= math::mul ( rotated_up_direction_vs, inverse_view_matrix );

			camera->set_view_direction( view_direction_ws );
			camera->set_view_up( up_direction_ws );
		}

		void aim_pinhole_camera(gx::pinhole_camera* camera, float angle_in_radians)
		{
			//rotate view_direction and up direction, since they can get aligned
			auto	view_direction_vs			= math::set(0.0f, 0.0f, 1.0f, 0.0f);
			auto	up_direction_vs				= math::set(0.0f, 1.0f, 0.0f, 0.0f);
			auto	rotation					= math::rotation_x( angle_in_radians );
			auto	rotated_view_direction_vs	= math::mul( view_direction_vs, rotation );
			auto	rotated_up_direction_vs		= math::mul( rotation, up_direction_vs );
			auto	view_matrix					= gx::create_view_matrix(camera);
			auto	inverse_view_matrix			= math::inverse( view_matrix );
						
			auto	view_direction_ws			= math::mul ( rotated_view_direction_vs, inverse_view_matrix );
			auto	up_direction_ws				= math::mul ( rotated_up_direction_vs, inverse_view_matrix );

			camera->set_view_direction( view_direction_ws );
			camera->set_view_up( up_direction_ws );
		}

	}

	void pinhole_camera_command_dispatcher::on_turn_left(const turn_camera_left* command)
	{
		turn_pinhole_camera(m_pinhole_camera, 1.0f * fabsf( command->m_angle_radians ) );
	}

	void pinhole_camera_command_dispatcher::on_turn_right(const turn_camera_right* command)
	{
		turn_pinhole_camera(m_pinhole_camera, -1.0f * fabsf( command->m_angle_radians ) );
	}

	void pinhole_camera_command_dispatcher::on_aim_up(const aim_camera_up* command)
	{
		aim_pinhole_camera(m_pinhole_camera, -1.0f * fabsf( command->m_angle_radians ) );
	}

	void pinhole_camera_command_dispatcher::on_aim_down(const aim_camera_down* command)
	{
		aim_pinhole_camera(m_pinhole_camera, 1.0f * fabsf( command->m_angle_radians ) );
	}

	void pinhole_camera_command_dispatcher::on_move_forward(const move_camera_forward* command)
	{
		auto	view_direction_ws				= m_pinhole_camera->get_view_direction();
		auto	normalized_view_direction_ws	= math::normalize3(view_direction_ws);

		auto	distance						= fabsf(command->m_distance);
		auto	distance_ws						= math::splat(distance);
		auto	displacement_ws					= math::mul( distance_ws, normalized_view_direction_ws);

		m_pinhole_camera->m_view_position_ws = math::add(m_pinhole_camera->m_view_position_ws, displacement_ws);
	}

	void pinhole_camera_command_dispatcher::on_move_backward(const move_camera_backward* command)
	{
		auto	view_direction_ws				= m_pinhole_camera->get_view_direction();
		auto	normalized_view_direction_ws	= math::normalize3(view_direction_ws);

		auto	distance						= -1.0f * fabsf(command->m_distance);
		auto	distance_ws						= math::splat(distance);
		auto	displacement_ws					= math::mul( distance_ws, normalized_view_direction_ws);

		m_pinhole_camera->m_view_position_ws = math::add(m_pinhole_camera->m_view_position_ws, displacement_ws);
	}

}