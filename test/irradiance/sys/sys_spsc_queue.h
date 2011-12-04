#ifndef __SYS_SPSC_QUEUE_H__
#define __SYS_SPSC_QUEUE_H__

#include <intrin.h>

#include <cstdint>
#include <array>
#include <utility>

namespace sys
{
	template <typename std::uint32_t size>
	class __declspec( align(64) )  fast_forward_spsc_queue
	{
		public:

		fast_forward_spsc_queue() : m_read_ptr(0), m_write_ptr(0), m_queue()
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

		inline bool flush()
		{
			return true;
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
	class __declspec( align(64) )  mpush_fast_forward_spsc_queue
	{
		public:

		mpush_fast_forward_spsc_queue() : m_read_ptr(0), m_write_ptr(0), m_local_queue_ptr(0), m_local_queue(), m_queue()
		{

		}

		inline bool enqueue(void* const data)
		{
			//2. if the buffer was full, the consumer inserted into the local queue and now tries to flush
			if (m_local_queue_ptr == local_buffer_size)
			{
				return flush_local_queue(local_buffer_size);
			}

			m_local_queue[ m_local_queue_ptr++] = data;

			//1. if the buffer is full, this will return false and the producer will retry
			if (m_local_queue_ptr == local_buffer_size)
			{
				return flush_local_queue(local_buffer_size);
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

		inline bool flush()
		{
			compiler_read_memory_barrier();
			if (m_local_queue_ptr != 0)
			{
				return flush_local_queue(m_local_queue_ptr);
			}
			else
			{
				return true;
			}
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

		inline bool flush_local_queue(uint32_t len)
		{
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
						m_queue[i] = m_local_queue[r];
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

	template <typename std::uint32_t size, std::uint32_t write_buffer_size, std::uint32_t read_buffer_size >
	class __declspec( align(64) )  mc_ring_buffer
	{
		public:

		mc_ring_buffer() : m_read_ptr(0)
			, m_write_ptr(0)
			, m_local_write_ptr(0)
			, m_next_read_ptr(0)
			, m_read_batch(0)
			, m_local_read_ptr(0)
			, m_next_write_ptr(0)
			, m_write_batch(0)
			, m_queue()
		{

		}

		inline bool enqueue(void* const data)
		{
			uint32_t next_write = next(m_next_write_ptr);

			//If we are about to write where we read
			if (next_write == m_local_read_ptr)
			{
				//We are full, should wait
				if (next_write == m_read_ptr)
				{
					return false;
				}
				
				//Update the cached copy of the read pointer
				m_local_read_ptr = m_read_ptr;
			}

			m_queue[m_next_write_ptr] = data;
			m_next_write_ptr = next_write;

			m_write_batch++;

			//Update shared copy after several iterations
			if (m_write_batch >= write_buffer_size)
			{
				m_write_ptr = m_next_write_ptr;
				m_write_batch = 0;
			}

			return true;
		}

		inline bool dequeue(void** data)
		{
			//If we are about to read from a place, where we are writing
			if ( m_next_read_ptr == m_local_write_ptr)
			{
				if (m_next_read_ptr == m_write_ptr)
				{
					return false;
				}

				m_local_write_ptr = m_write_ptr;
			}

			*data = m_queue[m_next_read_ptr];
			m_next_read_ptr = next(m_next_read_ptr);
			m_read_batch++;

			if (m_read_batch >= read_buffer_size)
			{
				m_read_ptr = m_next_read_ptr;
				m_read_batch = 0;
			}
	
			return true;
		}

		template <typename T> bool dequeue(T** data)
		{
			return dequeue( reinterpret_cast<void**> (data) );
		}

		inline bool flush()
		{
			return true;
		}

		private:
		static const uint32_t		cache_line_size = 64;

		//control variables
		volatile uint32_t			m_read_ptr;		
		volatile uint32_t			m_write_ptr;
		uint8_t						m_padding0[ cache_line_size - 2  * sizeof(uint32_t) ];

		//consumer
		uint32_t					m_local_write_ptr;
		uint32_t					m_next_read_ptr;
		uint32_t					m_read_batch;
		uint8_t						m_padding1[ cache_line_size - 3  * sizeof(uint32_t) ];

		//producer
		uint32_t					m_local_read_ptr;
		uint32_t					m_next_write_ptr;
		uint32_t					m_write_batch;
		uint8_t						m_padding2[ cache_line_size - 3  * sizeof(uint32_t) ];

		std::tr1::array<void*,size>	m_queue;

		static inline uint32_t next(uint32_t value)
		{
			uint32_t next_value = ++value;
			return next_value < size ? next_value : 0 ;
		}
	};
}


#endif