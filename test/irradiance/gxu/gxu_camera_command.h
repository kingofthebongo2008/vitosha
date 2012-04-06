#ifndef __GXU_CAMERA_COMMAND_H__
#define __GXU_CAMERA_COMMAND_H__

#include <cstdint>
#include <boost/noncopyable.hpp>

#include <sys/sys_base.h>

namespace gxu
{
	struct roll_camera
	{
		float m_angle;
	};

	struct yaw_camera
	{
		float m_angle;
	};

	struct pitch_camera
	{
		float m_angle;
	};

	struct move_camera_forward
	{
		static const uint16_t	id = 0;

		uint16_t	m_command_id;
		float		m_distance;
		uint8_t		m_camera_number;
	};

	struct move_camera_backward
	{
		static const uint16_t	id = 1;

		uint16_t	m_command_id;
		float		m_distance;
		uint8_t		m_camera_number;
	};

	struct camera_command_holder
	{
		uint16_t m_command_id;
		uint8_t  m_parameters[62];
	};

	struct camera_command
	{
		union
		{
			camera_command_holder	m_description;
			move_camera_forward		m_forward;
			move_camera_backward	m_backward;
		} m_command;
	};

	inline camera_command create_move_forward_command(float distance)
	{
		camera_command command;
		command.m_command.m_description.m_command_id = move_camera_forward::id;
		command.m_command.m_forward.m_distance = distance;
		command.m_command.m_forward.m_camera_number = 0;
		return command;
	}

	inline camera_command create_move_backward_command(float distance)
	{
		camera_command command;
		command.m_command.m_description.m_command_id = move_camera_backward::id;
		command.m_command.m_backward.m_distance = distance;
		command.m_command.m_backward.m_camera_number = 0;
		return command;
	}

	//processes stream of camera commands and dispatches them
	//suitable for recording and playback
	class camera_command_dispatcher : private boost::noncopyable
	{
		protected:

		virtual ~camera_command_dispatcher()
		{

		}

		public:

		void process( const camera_command* begin, const camera_command* end);
		void process( const camera_command* command);

		private:

		virtual void on_move_forward(const move_camera_forward* command)
		{
			UNUSED_PARAMETER(command);

		}

		virtual void on_move_backward(const move_camera_backward* command)
		{
			UNUSED_PARAMETER(command);
		}
	};

}

#endif