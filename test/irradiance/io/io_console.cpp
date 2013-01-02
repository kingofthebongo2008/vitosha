#include "precompiled.h"

#include <io/io_console.h>

#include <algorithm>
#include <memory>
#include <chrono>

namespace io
{
namespace console
{
	static std::shared_ptr<updater>							g_updater = std::make_shared<updater>();
	static __declspec(thread) thread_info*					g_thread_info;
	
	void	updater::register_thread_info(std::shared_ptr<thread_info> info)
	{
		std::lock_guard<std::mutex> guard(m_lock);

		//do not register twice a thread
		if (g_thread_info == nullptr)
		{
			m_infos.insert(info);
			info->m_pointer = info->m_buffers[m_buffer_index].get();
			g_thread_info = info.get();
		}
	}

	void	updater::unregister_thread_info(std::shared_ptr<thread_info> info)
	{
		std::lock_guard<std::mutex> guard(m_lock);
		m_infos.erase(info);

		//caller will crash if continues to use the buffer after removal
		info->m_pointer = nullptr;

		if (g_thread_info == info.get())
		{
			g_thread_info = nullptr;
		}
	}

	void	updater::register_notifier(std::shared_ptr<notifier> notifier)
	{
		std::lock_guard<std::mutex> guard(m_lock);
		m_notifiers.insert(notifier);
	}

	void	updater::unregister_notifier(std::shared_ptr<notifier> notifier)
	{
		std::lock_guard<std::mutex> guard(m_lock);
		m_notifiers.erase(notifier);
	}


	void updater::update()
	{
		std::lock_guard<std::mutex> guard(m_lock);

		
		std::vector<message> messages;
		messages.reserve(200);

		std::uint32_t index = m_buffer_index;
		++m_buffer_index;
		m_buffer_index %= 2;

		std::for_each( m_infos.begin(), m_infos.end(), [&](const std::shared_ptr<thread_info>& info)
		{
			//swap the pointers
			info->m_pointer = info->m_buffers[m_buffer_index].get();

			std::shared_ptr<buffer> buffer = info->m_buffers[index];

			std::copy( buffer->cbegin(), buffer->cend(), std::back_inserter(messages));
			buffer->reset();
		});

		if (!messages.empty())
		{
			std::sort( messages.begin(), messages.end() );
		}

		std::for_each( m_notifiers.begin(), m_notifiers.end(), [&](const std::shared_ptr<notifier>& notifier)
		{
			if (!messages.empty())
			{
				notifier->handle_messages( &messages[0], &messages[0] + messages.size() );
			}
		});

	}

	static void runner_thread(volatile bool& stop_thread, std::shared_ptr<updater> updater)
	{
		std::shared_ptr<thread_info> info = make_thread_info();
		updater->register_thread_info(info);

		while (stop_thread == false)
		{
			//sleep 1 second, notify everybody, sleep again
			updater->update();
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	runner::runner() : m_stop_thread(false), m_thread ( runner_thread, std::ref(m_stop_thread), g_updater )
	{
	}

	void runner::stop()
	{
		m_stop_thread = true;
		m_thread.join();
	}

	runner::~runner()
	{
		try
		{
			stop();
		}

		catch(const std::system_error&)
		{

		}
	}

	std::shared_ptr<thread_info>	make_thread_info()
	{
		return std::make_shared<thread_info>(std::make_shared<buffer>(), std::make_shared<buffer>() );
	}

	void	register_thread_info( std::shared_ptr<thread_info>	info)
	{
		g_updater->register_thread_info(info);
	}

	void	unregister_thread_info(std::shared_ptr<thread_info> info)
	{
		g_updater->unregister_thread_info(info);
	}

	void	register_notifier( std::shared_ptr<notifier>	n)
	{
		g_updater->register_notifier(n);
	}

	void	unregister_notifier( std::shared_ptr<notifier> n)
	{
		g_updater->unregister_notifier(n);
	}

	void	write(const wchar_t* string)
	{
		std::wstring s(string);

		std::chrono::system_clock::time_point p = std::chrono::steady_clock::now();
		message m ={ p, s };
		thread_info* info = g_thread_info;
		info->m_pointer->register_message(m);
	}

	void	writeln(const wchar_t* string)
	{

	}
}
}
