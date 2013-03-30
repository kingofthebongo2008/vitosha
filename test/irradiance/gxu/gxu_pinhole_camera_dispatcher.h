#ifndef __GXU_PINHOLE_CAMERA_DISPATCHER_H__
#define __GXU_PINHOLE_CAMERA_DISPATCHER_H__

#include <gx/gx_pinhole_camera.h>
#include <gxu/gxu_camera_command.h>

namespace gxu
{
    //processes stream of camera commands and dispatches them
    //suitable for recording and playback
    class pinhole_camera_command_dispatcher : public camera_command_dispatcher
    {
        public:
        explicit pinhole_camera_command_dispatcher(gx::pinhole_camera*	pinhole_camera) :
        m_pinhole_camera(pinhole_camera)
        {

        }

        private:

        void on_move_forward(const move_camera_forward* command);
        void on_move_backward(const move_camera_backward* command);
        void on_turn_left(const turn_camera_left* command);
        void on_turn_right(const turn_camera_right* command);
        void on_aim_up(const aim_camera_up* command);
        void on_aim_down(const aim_camera_down* command);
        

        gx::pinhole_camera*	m_pinhole_camera;
    };
}

#endif