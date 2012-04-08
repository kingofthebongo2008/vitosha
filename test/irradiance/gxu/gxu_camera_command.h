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

	struct turn_camera_left
	{
		static const uint16_t	id = 2;

		uint16_t	m_command_id;
		float		m_angle_radians;
		uint8_t		m_camera_number;
	};

	struct turn_camera_right
	{
		static const uint16_t	id = 3;

		uint16_t	m_command_id;
		float		m_angle_radians;
		uint8_t		m_camera_number;
	};

	struct aim_camera_up
	{
		static const uint16_t	id = 4;

		uint16_t	m_command_id;
		float		m_angle_radians;
		uint8_t		m_camera_number;
	};

	struct aim_camera_down
	{
		static const uint16_t	id = 5;

		uint16_t	m_command_id;
		float		m_angle_radians;
		uint8_t		m_camera_number;
	};

	struct move_camera_up
	{
		static const uint16_t	id = 6;

		uint16_t	m_command_id;
		float		m_distance;
		uint8_t		m_camera_number;
	};

	struct move_camera_down
	{
		static const uint16_t	id = 7;

		uint16_t	m_command_id;
		float		m_distance;
		uint8_t		m_camera_number;
	};

	struct strafe_camera_left
	{
		static const uint16_t	id = 8;

		uint16_t	m_command_id;
		float		m_distance;
		uint8_t		m_camera_number;
	};

	struct strafe_camera_right
	{
		static const uint16_t	id = 9;

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
			turn_camera_left		m_turn_left;
			turn_camera_right		m_turn_right;
			aim_camera_up			m_aim_up;
			aim_camera_down			m_aim_down;
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

	inline camera_command create_turn_camera_left_command(float m_angle_radians)
	{
		camera_command command;
		command.m_command.m_description.m_command_id = turn_camera_left::id;
		command.m_command.m_turn_left.m_angle_radians = m_angle_radians;
		command.m_command.m_turn_left.m_camera_number = 0;
		return command;
	}

	inline camera_command create_turn_camera_right_command(float m_angle_radians)
	{
		camera_command command;
		command.m_command.m_description.m_command_id = turn_camera_right::id;
		command.m_command.m_turn_right.m_angle_radians = m_angle_radians;
		command.m_command.m_turn_right.m_camera_number = 0;
		return command;
	}

	inline camera_command create_aim_camera_up_command(float m_angle_radians)
	{
		camera_command command;
		command.m_command.m_description.m_command_id = aim_camera_up::id;
		command.m_command.m_aim_up.m_angle_radians = m_angle_radians;
		command.m_command.m_aim_up.m_camera_number = 0;
		return command;
	}

	inline camera_command create_aim_camera_down_command(float m_angle_radians)
	{
		camera_command command;
		command.m_command.m_description.m_command_id = aim_camera_down::id;
		command.m_command.m_aim_down.m_angle_radians = m_angle_radians;
		command.m_command.m_aim_down.m_camera_number = 0;
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

		virtual void on_turn_left(const turn_camera_left* command)
		{
			UNUSED_PARAMETER(command);
		}

		virtual void on_turn_right(const turn_camera_right* command)
		{
			UNUSED_PARAMETER(command);
		}

		virtual void on_aim_up(const aim_camera_up* command)
		{
			UNUSED_PARAMETER(command);
		}

		virtual void on_aim_down(const aim_camera_down* command)
		{
			UNUSED_PARAMETER(command);
		}
	};
}

#endif