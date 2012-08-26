#ifndef __IO_STD_CONSOLE_NOTIFIER_H__
#define __IO_STD_CONSOLE_NOTIFIER_H__

#include <io/io_console.h>

namespace io
{
	namespace console
	{
		class std_notifier : public notifier
		{
			public:


			private:

			virtual void on_handle_messages(const message* message_begin, const message* message_end);
		};

		
	}
};


#endif