#include "precompiled.h"

#include <gxu/gxu_camera_command.h>

#include <algorithm>


namespace gxu
{
	void camera_command_dispatcher::process( const camera_command* command)
	{
		switch (command->m_command.m_description.m_command_id)
		{
			case move_camera_forward::id:
			{
				on_move_forward( reinterpret_cast<const move_camera_forward*>(command));
				break;
			}

			case move_camera_backward::id:
			{
				on_move_backward( reinterpret_cast<const move_camera_backward*>(command));
				break;
			}

			default:

				break;
		}

	}

	void camera_command_dispatcher::process( const camera_command* begin, const camera_command* end)
	{
		std::for_each( begin, end , [=] (const camera_command& command) { process(&command); } );
	}

}