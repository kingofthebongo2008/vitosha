#include "precompiled.h"
#include <iostream>

#include <io/io_std_console_notifier.h>


namespace io
{
namespace console
{
	void std_notifier::on_handle_messages(const message* message_begin, const message* message_end)
	{
		const message* m = message_begin;
		while ( m != message_end)
		{
			std::wcout << m->m_text;
			m++;
		}
	}
}
}
