#include "precompiled.h"

#include <gxu/gxu_pinhole_camera_dispatcher.h>

namespace gxu
{
	void pinhole_camera_command_dispatcher::on_move_forward(const move_camera_forward* command)
	{
		UNUSED_PARAMETER(command);
	}

	void pinhole_camera_command_dispatcher::on_move_backward(const move_camera_backward* command)
	{
		UNUSED_PARAMETER(command);
	}

	namespace
	{
		void turn_pinhole_camera(gx::pinhole_camera* camera, float angle_in_radians)
		{
			//use matrix computations, since we do not have quaternion library still
			math::vector_float4		view_direction_vs			= math::set(0.0f, 0.0f, 1.0f, 0.0f);
			math::vector_float4		up_direction_vs				= math::set(0.0f, 1.0f, 0.0f, 0.0f);
			math::matrix_float44	rotation					= math::rotation_y( angle_in_radians );
			math::vector_float4		rotated_view_direction_vs	= math::mul( rotation, math::negate(view_direction_vs) );
			math::vector_float4		rotated_up_direction_vs		= math::mul( rotation,up_direction_vs );
			math::matrix_float44	view_matrix					= gx::create_view_matrix(camera);
			math::matrix_float44	inverse_view_matrix			= math::inverse( view_matrix );
						
			math::vector_float4		view_direction_ws			= math::mul ( rotated_view_direction_vs, inverse_view_matrix );
			math::vector_float4		up_direction_ws				= math::mul ( rotated_up_direction_vs, inverse_view_matrix );

			camera->set_view_direction( view_direction_ws );
			camera->set_view_up( up_direction_ws );
		}

		void aim_pinhole_camera(gx::pinhole_camera* camera, float angle_in_radians)
		{

			//rotate view_direction and up direction, since they can get aligned
			math::vector_float4		view_direction_vs			= math::set(0.0f, 0.0f, 1.0f, 0.0f);
			math::vector_float4		up_direction_vs				= math::set(0.0f, 1.0f, 0.0f, 0.0f);
			math::matrix_float44	rotation					= math::rotation_x( angle_in_radians );
			math::vector_float4		rotated_view_direction_vs	= math::mul( view_direction_vs, rotation );
			math::vector_float4		rotated_up_direction_vs		= math::mul( rotation, up_direction_vs );
			math::matrix_float44	view_matrix					= gx::create_view_matrix(camera);
			math::matrix_float44	inverse_view_matrix			= math::inverse( view_matrix );
						
			math::vector_float4		view_direction_ws			= math::mul ( rotated_view_direction_vs, inverse_view_matrix );
			math::vector_float4		up_direction_ws				= math::mul ( rotated_up_direction_vs, inverse_view_matrix );

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
}