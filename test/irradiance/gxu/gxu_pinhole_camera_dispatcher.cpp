#include "precompiled.h"

#include <gxu/gxu_pinhole_camera_dispatcher.h>

#include <math/math_quaternion.h>
#include <math/math_graphics.h>

namespace gxu
{
    namespace
    {
        void turn_pinhole_camera(gx::pinhole_camera* camera, float angle_in_radians)
        {
            auto    view_direction_ws           = camera->get_view_direction();
            auto    up_direction_ws             = camera->get_up();

            auto    quaternion                  = math::quaternion_axis_angle( up_direction_ws, angle_in_radians);
            auto    view_direction_ws_2         = math::rotate_vector3( view_direction_ws, quaternion );

            camera->set_view_direction( view_direction_ws_2 );
        }

        void aim_pinhole_camera(gx::pinhole_camera* camera, float angle_in_radians)
        {
            auto    view_direction_ws_1         = camera->get_view_direction();
            auto    up_direction_ws_1           = camera->get_up();
            auto    cross                       = math::cross3(view_direction_ws_1, up_direction_ws_1) ;

            auto    quaternion                  = math::quaternion_axis_angle( cross, angle_in_radians);
            auto    view_direction_ws_2         = math::rotate_vector3( view_direction_ws_1, quaternion );
            auto    up_direction_ws_2           = math::rotate_vector3( up_direction_ws_1, quaternion );

            camera->set_view_direction( view_direction_ws_2 );
            camera->set_view_up( up_direction_ws_2 );
        }

    }

    void pinhole_camera_command_dispatcher::on_turn_left(const turn_camera_left* command)
    {
        turn_pinhole_camera(m_pinhole_camera, -1.0f * fabsf( command->m_angle_radians ) );
    }

    void pinhole_camera_command_dispatcher::on_turn_right(const turn_camera_right* command)
    {
        turn_pinhole_camera(m_pinhole_camera, 1.0f * fabsf( command->m_angle_radians ) );
    }

    void pinhole_camera_command_dispatcher::on_aim_up(const aim_camera_up* command)
    {
        aim_pinhole_camera(m_pinhole_camera, 1.0f * fabsf( command->m_angle_radians ) );
    }

    void pinhole_camera_command_dispatcher::on_aim_down(const aim_camera_down* command)
    {
        aim_pinhole_camera(m_pinhole_camera, -1.0f * fabsf( command->m_angle_radians ) );
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