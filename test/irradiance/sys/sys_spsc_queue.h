#ifndef __SYS_SPSC_QUEUE_H__
#define __SYS_SPSC_QUEUE_H__

#include <cstdint>
#include <array>

namespace sys
{
	namespace details
	{
		template <typename non_blocking_queue>
		class blocking_queue : public non_blocking_queue
		{
			public:
			using non_blocking_queue::enqueue;
			using non_blocking_queue::dequeue;
			using non_blocking_queue::flush;

			template <typename Functor> inline void enqueue(void* const data, Functor f) throw()
			{
				non_blocking_queue* queue = static_cast<non_blocking_queue*>(this);
				while(!queue->enqueue(data))
				{
					f();
				}
			}

			template <typename Functor> inline void dequeue(void** data, Functor f) throw()
			{
				non_blocking_queue* queue = static_cast<non_blocking_queue*>(this);
				while (!queue->dequeue(data))
				{
					f();
				}
			}

			template <typename Functor> inline bool flush(Functor f) throw()
			{
				non_blocking_queue* queue = static_cast<non_blocking_queue*>(this);
				while (!queue->flush())
				{
					f();
				}
				return true;
			}
		};

		template <typename std::uint32_t size>
		class alignas(64) fast_forward_spsc_queue
		{
			public:
			fast_forward_spsc_queue() throw() : m_read_ptr(0), m_write_ptr(0), m_queue()
			{

			}

			inline bool enqueue(void* const data) throw()
			{
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

			inline bool dequeue(void** data) throw()
			{
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


			inline bool flush() throw()
			{
				return true;
			}

			inline void reset() throw()
			{
				m_read_ptr = 0;
				m_write_ptr = 0;
				std::fill(std::begin(m_queue), std::end(m_queue), nullptr);
			}

			template <typename T> bool dequeue(T** data) throw()
			{
				return dequeue( reinterpret_cast<void**> (data) );
			}

			inline bool empty() const throw()
			{
				return (m_queue[m_read_ptr] == nullptr);
			}

			inline bool full() const throw()
			{
				return (m_queue[m_write_ptr] != nullptr);
			}

			private:
			static const uint32_t cache_line_size = 64;

			volatile alignas(64) uint32_t			m_read_ptr;
			uint8_t									pad0[cache_line_size - sizeof(uint32_t)];
			volatile alignas(64) uint32_t			m_write_ptr;
			uint8_t									pad1[cache_line_size - sizeof(uint32_t)];

			alignas(64) std::tr1::array<void*,size>	m_queue;		
		};

		template <typename std::uint32_t size, std::uint32_t local_buffer_size>
		class alignas(64)  mpush_fast_forward_spsc_queue
		{
			public:

			mpush_fast_forward_spsc_queue() throw() : m_read_ptr(0), m_write_ptr(0), m_local_queue_ptr(0), m_local_queue(), m_queue()
			{

			}

			inline bool enqueue(void* const data) throw()
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

			inline bool dequeue(void** data) throw()
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

			template <typename T> bool dequeue(T** data) throw()
			{
				return dequeue( reinterpret_cast<void**> (data) );
			}

			template <typename T, typename Functor> void dequeue(T** data, Functor f) throw()
			{
				while (!dequeue(data))
				{
					f();
				}
			}


			inline bool flush() throw()
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

			inline bool empty() const throw()
			{
				return (m_queue[m_read_ptr] == nullptr);
			}

			inline bool full() const throw()
			{
				return (m_queue[m_write_ptr] != nullptr);
			}

			inline void reset() throw()
			{
				m_read_ptr = 0;
				m_write_ptr = 0;
				m_local_queue_ptr = 0;

				std::fill(std::begin(m_queue), std::end(m_queue), nullptr);
				std::fill(std::begin(m_local_queue), std::end(m_local_queue), nullptr);
			}

			private:
			static const uint32_t cache_line_size = 64;

			volatile alignas(64) uint32_t m_read_ptr;
			uint8_t				 m_padding0[ cache_line_size - 1  * sizeof(uint32_t) ];
			volatile alignas(64) uint32_t m_write_ptr;
			uint8_t				 m_padding1[ cache_line_size - 1  * sizeof(uint32_t) ];
			alignas(64) uint32_t m_local_queue_ptr;
			uint8_t				 m_padding2[ cache_line_size - 1  * sizeof(uint32_t) ];
		
			alignas(64) std::tr1::array<void*,local_buffer_size>	m_local_queue;
			alignas(64) std::tr1::array<void*,size>					m_queue;

			inline bool flush_local_queue(uint32_t len) throw()
			{
				uint32_t last = m_write_ptr + ( (m_write_ptr + --len < size) ? len : (len - size) );

				uint32_t r = len - (last + 1);
			
				uint32_t l = last;

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
		class alignas(64)  mc_ring_buffer 
		{
			public:

			mc_ring_buffer() throw() : m_read_ptr(0)
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

			inline bool enqueue(void* const data) throw()
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

			inline bool dequeue(void** data) throw()
			{
				//If we are about to read from a place, where we are writing. Check the cached copy of the write pointer
				if ( m_next_read_ptr == m_local_write_ptr)
				{
					//if the cached copy says we are overwriting, then check the volatile pointer
					if (m_next_read_ptr == m_write_ptr)
					{
						return false;
					}

					//update the cached copy with the volatile pointer
					m_local_write_ptr = m_write_ptr;
				}

				*data = m_queue[m_next_read_ptr];
				m_next_read_ptr = next(m_next_read_ptr);
				m_read_batch++;

				//Update shared copy after several iterations
				if (m_read_batch >= read_buffer_size)
				{
					m_read_ptr = m_next_read_ptr;
					m_read_batch = 0;
				}
	
				return true;
			}

			template <typename T> bool dequeue(T** data) throw()
			{
				return dequeue( reinterpret_cast<void**> (data) );
			}

			template <typename T, typename Functor> void dequeue(T** data, Functor f) throw()
			{
				while (!dequeue(data))
				{
					f();
				}
			}

			inline bool flush() throw()
			{
				m_write_ptr = m_next_write_ptr;
				m_write_batch = 0;
				return true;
			}

			inline bool empty() throw()
			{
				if ( m_next_read_ptr == m_local_write_ptr)
				{
					if (m_next_read_ptr == m_write_ptr)
					{
						return true;
					}
					else
					{
						return false;
					}

				}
				else
				{
					return false;
				}

			}

			inline bool full() throw()
			{
				uint32_t next_write = next(m_next_write_ptr);

				//If we are about to write where we read
				if (next_write == m_local_read_ptr)
				{
					//We are full, should wait
					if (next_write == m_read_ptr)
					{
						return true;
					}
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}

			inline void reset()
			{
				m_read_ptr = 0;
				m_write_ptr = 0;
				m_local_write_ptr = 0;
				m_next_read_ptr = 0;
				m_read_batch = 0;
				m_local_read_ptr = 0;
				m_next_write_ptr = 0;
				m_write_batch = 0;

				std::fill(std::begin(m_queue), std::end(m_queue), nullptr);
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

			static inline uint32_t next(uint32_t value) throw()
			{
				uint32_t next_value = ++value;
				return next_value < size ? next_value : 0 ;
			}
		};

		template <typename std::uint32_t size>
		class alignas(64) batch_queue
		{
			public:

			batch_queue() throw() :
				m_read_ptr(0)
				, m_write_ptr(0)
				, m_status(0)
				, m_queue()
			{

			}

			template <typename Functor>
			inline void enqueue(void* const data, Functor f) throw()
			{
				m_queue[m_write_ptr++] = data;
				m_write_ptr %= 2 * size;

				if (m_write_ptr % size == 0)
				{
					while(m_status)
					{
						f();
					}
					m_status = 1;
				}
			}

			template <typename Functor>
			inline void dequeue(void* data[], Functor f) throw()
			{
				while (!m_status)
				{
					f();
				}

				for (uint32_t i = m_read_ptr, j = 0 ; i < m_read_ptr + size; ++i, ++j)
				{
					data[j] = m_queue[i];
				}

				m_read_ptr = (m_read_ptr + size ) % (2 * size);
				m_status = 0;
			}

			template <typename T, typename Functor> bool dequeue(T* data[], Functor f) throw()
			{
					while (!status)
					{
						f();
					}

					for (auto i = 0,  j = 0 ; i < m_read_ptr + size; ++i, ++j)
					{
						data[j] = reinterpret_cast<T*> (m_queue[i]);
					}
					m_read_ptr = (m_read_ptr + size ) % (2 * size);
					status = 0;
					return true;
			}
		
			template <typename Functor> inline bool flush(Functor f) throw()
			{
				while(m_status)
				{
					f();
				}

				m_status = 1;
				return true;
			}

			private:
			static const uint32_t		cache_line_size = 64;

			uint32_t					m_read_ptr;		
			uint8_t						m_padding0[ cache_line_size - 1  * sizeof(uint32_t) ];

			uint32_t					m_write_ptr;
			uint8_t						m_padding1[ cache_line_size - 1  * sizeof(uint32_t) ];

			volatile uint32_t			m_status;
			uint8_t						m_padding2[ cache_line_size - 1  * sizeof(uint32_t) ];

			std::tr1::array<void*, 2 * size>	m_queue;
		};

		template <typename std::uint32_t size, template<typename t> class allocator = std::allocator>
		class alignas(64) spsc_dynamic_buffer
		{
			private:
			static const uint32_t cache_line_size = 64;

			struct node
			{
				void* m_data;
				node* m_next;
			};

			public:
			typedef typename fast_forward_spsc_queue<size>	spsc_buffer;
			typedef typename allocator<node>				allocator_type;

			private:
			volatile node*						m_head;
			uint8_t								m_padding0[ cache_line_size - sizeof(node*)];
			volatile node*						m_tail;
			uint8_t								m_padding1[ cache_line_size - sizeof(node*)];
			spsc_buffer							m_cache;

			allocator_type						m_allocator;

			public:

			spsc_dynamic_buffer(  ) : m_head(nullptr), m_tail(nullptr)
			{
				initialize();
			}

			spsc_dynamic_buffer( const allocator_type& alloc) : m_allocator(allocator)
			{
				initialize();
			}

			~spsc_dynamic_buffer(  )
			{
				while (m_head && m_head->m_next)
				{
					node* n = const_cast<node*> (m_head);
					m_head = m_head->m_next;
					m_allocator.deallocate( n, 1);
				}

				m_allocator.deallocate( const_cast<node*> (m_head), 1);
			}

			inline bool enqueue(void* const data)
			{
				node* node = alloc_node();
				node->m_data = data;
				node->m_next = nullptr;

				m_tail->m_next = node;
				m_tail = node;
				return true;
			}

			inline bool dequeue(void** data)
			{
				if (m_head->m_next)
				{
					volatile node* n = m_head;
				
					*data = m_head->m_next->m_data;
					m_head = m_head->m_next;

					free_node( const_cast<node*> (n) );

					return true;
				}

				return false;
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

			void initialize()
			{
				node* n = m_allocator.allocate(1);
			
				n->m_data = nullptr;
				n->m_next = nullptr;

				m_head = n;
				m_tail = n;

				for (auto i = 0 ; i < size; ++i)
				{
					node* n = m_allocator.allocate(1);
					m_cache.enqueue(n);
				}
			}

			inline node*	alloc_node()
			{
				node* n;

				if (m_cache.dequeue(&n))
				{
					return n;
				}

				n = m_allocator.allocate(1);
				return n;
			}

			inline void	free_node(node* node)
			{
				if (!m_cache.enqueue(node))
				{
					m_allocator.deallocate(node, 1);
				}
			}
		};
	
		template <typename spsc_buffer_t, typename std::uint32_t size, template<typename t> class allocator = std::allocator>
		class alignas(64) buffer_pool
		{
			public:
			typedef spsc_buffer_t													spsc_buffer;

			private:
			typedef typename allocator<spsc_buffer>									allocator_type;
				
			allocator_type															m_allocator;
			spsc_dynamic_buffer<size, allocator>									m_used;
			fast_forward_spsc_queue<size>											m_buffer_cache;
		

			public:

			buffer_pool()
			{

			}

			buffer_pool( const allocator_type& alloc) : m_allocator(allocator), m_used(m_allocator)
			{
		

			}

			spsc_buffer* const next_write()
			{
				spsc_buffer* buffer;

				if (!m_buffer_cache.dequeue(&buffer))
				{
					buffer = m_allocator.allocate(1);
					::new (buffer) spsc_buffer();
				}

				m_used.enqueue(buffer);
				return buffer;
			}

			spsc_buffer* const next_read()
			{
				spsc_buffer* buffer;

				if (m_used.dequeue(&buffer))
				{
					return buffer;
				}
				else
				{
					return nullptr;
				}
			}

			void release(spsc_buffer* buffer)
			{
				buffer->reset();

				if (!m_buffer_cache.enqueue(buffer))
				{
					m_allocator.destroy(buffer);
					m_allocator.deallocate(buffer,1);
				}
			}
		};


		template <typename std::uint32_t size, typename spsc_buffer_t, template<typename t> class allocator = std::allocator>
		class alignas(64) unbounded_spsc_queue
		{
			public:
			typedef spsc_buffer_t										spsc_buffer;
			typedef typename allocator<spsc_buffer>						allocator_type;

			public:

			unbounded_spsc_queue() : m_buffer_read(0), m_buffer_write(0)
			{
				initialize();
			}

			unbounded_spsc_queue(const allocator_type& alloc) : m_buffer_read(0), m_buffer_write(0), m_buffer_pool(alloc), m_allocator(alloc)
			{
				initialize();
			}
		
			~unbounded_spsc_queue()
			{
				if (m_buffer_read)
				{
					m_allocator.destroy((spsc_buffer*)m_buffer_read);
					m_allocator.deallocate((spsc_buffer*) m_buffer_read,1);
				}
			}

			inline bool empty() const throw()
			{
				return ( m_buffer_read->empty() );
			}

			inline bool full() const throw()
			{
				return ( m_buffer_write->full() );
			}

			inline bool enqueue(void* const data)
			{
				if ( full() )
				{
					spsc_buffer* buffer = m_buffer_pool.next_write();

					if (buffer)
					{
						m_buffer_write->flush();
						m_buffer_write = buffer;
					}
					else
					{
						return false;
					}
				}

				return m_buffer_write->enqueue(data);
			}

			inline bool dequeue(void** data)
			{
				if (m_buffer_read->empty())
				{
					if (m_buffer_read == m_buffer_write)
					{
						return false;
					}

					if (m_buffer_read->empty())
					{
						spsc_buffer* buffer = m_buffer_pool.next_read();
						if (buffer)
						{
							m_buffer_pool.release(m_buffer_read);
							m_buffer_read = buffer;
						}
					}
				}

				return m_buffer_read->dequeue(data);
			}

			inline bool flush()
			{
				return m_buffer_write->flush();
			}

			template <typename Functor> inline bool flush(Functor f)
			{
				while (!flush())
				{
					f();
				}
				return true;
			}

			template <typename Functor> inline void enqueue(void* const data, Functor f)
			{
				while(!enqueue(data))
				{
					f();
				}
			}

			template <typename Functor> inline void dequeue(void** data, Functor f)
			{
				while (!dequeue(data))
				{
					f();
				}
			}

			private:
			static const uint32_t cache_line_size = 64;
		

			spsc_buffer*												m_buffer_read;
			uint8_t														m_pad0[cache_line_size - sizeof(void*)];
			spsc_buffer*												m_buffer_write;
			uint8_t														m_pad1[cache_line_size - sizeof(void*)];
			buffer_pool<spsc_buffer, 64,  allocator>					m_buffer_pool;
			allocator_type												m_allocator;

			inline void initialize()
			{
				spsc_buffer* buffer;
				buffer = m_allocator.allocate(1);
				::new (buffer) spsc_buffer();
				m_buffer_read = m_buffer_write = buffer;
			}

		};
	}


	template <typename std::uint32_t size>
	class alignas(64) fast_forward_spsc_queue : public details::blocking_queue< details::fast_forward_spsc_queue<size>>
	{

	};

	template <typename std::uint32_t size, std::uint32_t local_buffer_size>
	class alignas(64)  mpush_fast_forward_spsc_queue :	public details::blocking_queue< details::mpush_fast_forward_spsc_queue<size, local_buffer_size>>
	{
	};

	template <typename std::uint32_t size, std::uint32_t write_buffer_size, std::uint32_t read_buffer_size >
	class alignas(64)  mc_ring_buffer  : public details::blocking_queue< details::mc_ring_buffer<size, write_buffer_size, read_buffer_size> >
	{

	};

	//typedef mpush_fast_forward_spsc_queue<size,16>				spsc_buffer;
	//mc_ring_buffer<size,32,32>
	template <typename std::uint32_t size, template<typename t> class allocator = std::allocator>
	class alignas(64) unbounded_spsc_queue : public details::unbounded_spsc_queue<size,  mc_ring_buffer<size,32,32>, allocator>
	{
		private:
		typedef details::unbounded_spsc_queue<size,  mc_ring_buffer<size,32,32>, allocator> base;

		public:
		unbounded_spsc_queue()
		{
		}

		unbounded_spsc_queue(const allocator_type& alloc) : base(alloc)
		{

		}
	};

}


#endif