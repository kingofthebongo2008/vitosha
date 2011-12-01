#ifndef __SYS_SPSC_QUEUE_H__
#define __SYS_SPSC_QUEUE_H__

#include <intrin.h>

#include <cstdint>
#include <array>
#include <utility>

namespace sys
{
	template <typename std::uint32_t size>
	class __declspec( align(64) )  spsc_queue
	{
		public:

		spsc_queue() : m_read_ptr(0), m_write_ptr(0), m_queue()
		{
		}

		inline bool enqueue(void* const data)
		{
			compiler_read_memory_barrier();
			if ( !full() )
			{
				m_queue[ m_write_ptr ] = data;
				m_write_ptr += ( m_write_ptr + 1 < size ) ? 1 : ( 1 - size ) ;
				return true;
			}
			else
			{
				return false;
			}
		}

		inline bool dequeue(void** data)
		{
			compiler_read_memory_barrier();
			if (!empty())
			{
				*data = m_queue[ m_read_ptr ];
				m_queue[ m_read_ptr ] = nullptr;
				m_read_ptr += ( m_read_ptr + 1 < size ) ? 1 : ( 1 - size ) ;
				return true;
			}
			else
			{
				return false;
			}
		}

		template <typename T> bool dequeue(T** data)
		{
			return dequeue( reinterpret_cast<void**> (data) );
		}


		private:
		static const uint32_t cache_line_size = 64;
		static const uint32_t cache_line_padding = cache_line_size / sizeof(uint32_t) ;

		__declspec( align(64) ) uint32_t m_read_ptr;
		__declspec( align(64) ) uint32_t m_write_ptr;

		__declspec( align(64) ) std::tr1::array<void*,size>	m_queue;		

		inline void compiler_read_memory_barrier()
		{
			_ReadBarrier();
		}

		inline bool empty() const
		{
			return (m_queue[m_read_ptr] == nullptr);
		}

		inline bool full() const
		{
			return (m_queue[m_write_ptr] != nullptr);
		}

	};


	template <typename std::uint32_t size, std::uint32_t local_buffer_size>
	class __declspec( align(64) )  mpush_spsc_queue
	{
		public:

		mpush_spsc_queue() : m_read_ptr(0), m_write_ptr(0), m_local_queue_ptr(0), m_local_queue(), m_queue()
		{

		}

		inline bool enqueue(void* const data)
		{
			//2. if the buffer was full, the consumer inserted into the local queue and now tries to flush
			if (m_local_queue_ptr == local_buffer_size)
			{
				return flush_local_queue();
			}

			m_local_queue[ m_local_queue_ptr++] = data;

			//1. if the buffer is full, this will return false and the producer will retry
			if (m_local_queue_ptr == local_buffer_size)
			{
				return flush_local_queue();
			}

			return true;
		}

		inline bool dequeue(void** data)
		{
			compiler_read_memory_barrier();
			if (!empty())
			{
				*data = m_queue[ m_read_ptr ];
				m_queue[ m_read_ptr ] = nullptr;
				m_read_ptr += ( m_read_ptr + 1 < size ) ? 1 : ( 1 - size ) ;
				return true;
			}
			else
			{
				return false;
			}
		}

		template <typename T> bool dequeue(T** data)
		{
			return dequeue( reinterpret_cast<void**> (data) );
		}

		private:
		static const uint32_t cache_line_size = 64;
		static const uint32_t cache_line_padding = cache_line_size / sizeof(uint32_t) ;

		__declspec( align(64) ) uint32_t m_read_ptr;
		__declspec( align(64) )	uint32_t m_write_ptr;
		__declspec( align(64) ) uint32_t m_local_queue_ptr;
		
		__declspec( align(64) ) std::tr1::array<void*,local_buffer_size>	m_local_queue;
		__declspec( align(64) ) std::tr1::array<void*,size>					m_queue;

		inline void compiler_read_memory_barrier()
		{
			_ReadBarrier();
		}

		inline bool empty() const
		{
			return (m_queue[m_read_ptr] == nullptr);
		}

		inline bool full() const
		{
			return (m_queue[m_write_ptr] != nullptr);
		}

		inline bool flush_local_queue()
		{
			uint32_t len = local_buffer_size ;

			uint32_t last = m_write_ptr + ( (m_write_ptr + --len < size) ? len : (len - size) );

			uint32_t r = len - (last + 1);
			
			uint32_t l = last;


			compiler_read_memory_barrier();
			//if the queue is not full
			if ( m_queue[last] == nullptr)
			{
				if (last < m_write_ptr)
				{
					for (auto i = len; i > r; --i, --l)
					{
						m_queue[l] = m_local_queue[i];
					}

					for (auto i = size - 1; i >= m_write_ptr;--i,--r)
					{
						m_queue[r] = m_local_queue[r];
					}
				}
				else
				{
					for (int32_t i = len; i >=0; --i)
					{
						m_queue[ m_write_ptr + i] = m_local_queue[i];
					}
				}

				m_write_ptr = (last + 1 >= size ) ? 0 : (last + 1 );
				m_local_queue_ptr = 0;
				return true;
			}
			else
			{
 				return false;
			}
		}

	};
}


#endif