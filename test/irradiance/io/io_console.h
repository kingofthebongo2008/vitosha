#ifndef __IO_CONSOLE_H__
#define __IO_CONSOLE_H__

#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_set>

#include <boost/noncopyable.hpp>


namespace io
{
	namespace console
	{
		struct message
		{
			std::chrono::system_clock::time_point	m_time;
			std::wstring							m_text;
		};

		inline bool operator<(const message& l, const message& r)
		{
			return (l.m_time < r.m_time);
		}

		class notifier : private boost::noncopyable
		{
			public:

			virtual ~notifier()
			{

			}

			void handle_messages(const message* message_begin, const message* message_end)
			{
				on_handle_messages(message_begin, message_end);
			}

			private:

			virtual void on_handle_messages(const message* message_begin, const message* message_end) = 0;
		};

		class buffer
		{
			public:
			typedef std::vector<message>::iterator iterator;
			typedef std::vector<message>::const_iterator const_iterator;

			buffer()
			{

			}


			iterator begin()
			{
				return m_messages.begin();
			}

			iterator end()
			{
				return m_messages.end();
			}

			const_iterator cbegin() const
			{
				return m_messages.cbegin();
			}

			const_iterator cend() const
			{
				return m_messages.cend();
			}

			void	register_message( const message& m )
			{
				m_messages.push_back(m);
			}


			void	reset()
			{
				m_messages.clear();
			}

			private:

			std::vector < message >	m_messages;
		};

		struct thread_info : private boost::noncopyable
		{
			buffer*					m_pointer;
			std::shared_ptr<buffer> m_buffers[2];

			thread_info(std::shared_ptr<buffer> buffer_0, std::shared_ptr<buffer> buffer_1 ) : m_pointer (buffer_0.get())
			{
				m_buffers[0] = buffer_0;
				m_buffers[1] = buffer_1;
			}
		};

		class updater
		{
			public:

			updater() : m_buffer_index(0)
			{

			}

			void	register_thread_info(std::shared_ptr<thread_info> info);
			void	unregister_thread_info(std::shared_ptr<thread_info> info);

			void	register_notifier(std::shared_ptr<notifier> notifier);
			void	unregister_notifier(std::shared_ptr<notifier> notifier);

			void	update();

			private:

			std::mutex														m_lock;
			std::unordered_set< std::shared_ptr<notifier> >					m_notifiers;
			std::unordered_set< std::shared_ptr<thread_info> >				m_infos;
			std::uint32_t													m_buffer_index;
	
			void															sort();
			void															swap();
		};


		//public interfaces
		class runner : private boost::noncopyable
		{
			public:
			
			runner();
			~runner();


			private:

			void stop();
			volatile bool				m_stop_thread;
			std::thread					m_thread;
		};


		std::shared_ptr<thread_info>	make_thread_info();

		void	register_thread_info( std::shared_ptr<thread_info>	info);
		void	unregister_thread_info(std::shared_ptr<thread_info> info);

		void	register_notifier( std::shared_ptr<notifier>	n);
		void	unregister_notifier( std::shared_ptr<notifier> n) ;

		void	write(const wchar_t* string);

		class register_thread_info_helper : private boost::noncopyable
		{
			public:

			register_thread_info_helper() : m_info( make_thread_info() )
			{
				register_thread_info(m_info);
			}

			~register_thread_info_helper()
			{
				unregister_thread_info(m_info);
			}

			private:
			std::shared_ptr<thread_info> m_info;
		};

		class register_notifier_helper : private boost::noncopyable
		{
			public:

			explicit register_notifier_helper( std::shared_ptr<notifier> n)
			{
				register_notifier(n);
			}

			~register_notifier_helper()
			{
				unregister_notifier(m_n);
			}

			private:
			std::shared_ptr<notifier> m_n;
			register_notifier_helper();

		};
	}
};


#endif