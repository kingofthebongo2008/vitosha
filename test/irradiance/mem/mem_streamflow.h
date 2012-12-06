#ifndef __MEM_STREAMFLOW_H__
#define __MEM_STREAMFLOW_H__

#include <atomic>
#include <cstdint>
#include <intrin.h>

#include <sys/sys_spin_lock.h>
#include <mem/mem_alloc.h>

namespace mem
{
    namespace streamflow
    {
        //---------------------------------------------------------------------------------------
        namespace detail
        {
            inline uint32_t log2(uint32_t x) throw()
            {
                unsigned long result;
                _BitScanForward(&result, x);
                return static_cast<uint32_t>(result);
            }

            template<uint32_t x> struct log2_c
            {
                static const uint32_t value = 1 + log2_c< x / 2>::value;
            };

            template<> struct log2_c<1>
            {
                static const uint32_t value = 0;
            };

            template<> struct log2_c<0>
            {

            };
        }

        //---------------------------------------------------------------------------------------
        template <typename T>
        class list_element 
        {
        public:

            list_element() :
              m_previous(nullptr)
                  , m_next(nullptr)
              {

              }

              T* get_next() throw()
              {
                  return m_next;
              }

              const T* get_next() const throw()
              {
                  return m_next;
              }

              void set_next(T* next) throw()
              {
                  m_next = next;
              }

              T* get_previous() throw()
              {
                  return m_previous;
              }

              const T* get_previous() const throw()
              {
                  return m_previous;
              }

              void set_previous(T* previous) throw()
              {
                  m_previous = previous;
              }

              T*  m_previous;
              T*  m_next;
        };

        //---------------------------------------------------------------------------------------
        template <typename T>
        class list
        {
        public:

            list() throw() : 
              m_head(nullptr)
                  , m_tail(nullptr)
              {

              }

              void push_front(T* block) throw()
              {
                  block->set_next(m_head);

                  if (m_head)
                  {
                      m_head->set_previous(block);
                  }

                  m_head = block;

                  if (m_tail == nullptr)
                  {
                      m_tail = m_head;
                  }
              }

              void pop_front() throw()
              {
                  if (m_head)
                  {
                      m_head = m_head->get_next();
                  }

                  if (m_head == nullptr)
                  {
                      m_tail = nullptr;
                  }
                  else
                  {
                      m_head->set_previous(nullptr);
                  }
              }

              void push_back(T* block) throw()
              {
                  if (m_tail != nullptr)
                  {
                      m_tail->set_next(nullptr);
                  }

                  block->set_previous(m_tail);
                  m_tail = block;

                  if (m_head == nullptr)
                  {
                      m_head = m_tail;
                  }
              }

              void pop_back() throw()
              {
                  if (m_tail)
                  {
                      m_tail = m_tail->get_previous();
                  }

                  if (m_tail == nullptr)
                  {
                      m_head = nullptr;
                  }
                  else
                  {
                      m_tail->set_next(nullptr);
                  }
              }

              T* front() throw()
              {
                  return m_head;
              }

              const T* front() const throw()
              {
                  return m_head;
              }

              T* back() throw()
              {
                  return m_tail;
              }

              const T* back() const throw()
              {
                  return m_tail;
              }

              void remove(T* block) throw()
              {
                  if (block->get_previous())
                  {
                      T* previous = block->get_previous();
                      previous->set_next( block->get_next() );
                  }
                  else
                  {
                      m_head = block->get_next();
                  }

                  if (block->get_next())
                  {
                      T* next = block->get_next();
                      next->set_previous( block->get_previous() );
                  }
                  else
                  {
                      m_tail = block->get_previous();
                  }

              }

              bool empty() const throw()
              {
                  return (m_head == nullptr && m_head == m_tail);
              }

        private:
            T* m_head;
            T* m_tail;
        };


        //---------------------------------------------------------------------------------------
		namespace details
		{
			namespace details1
			{
				//128 bit aligned pointer on windows with 43 bits used in it
				static inline uintptr_t pack_pointer( uintptr_t pointer) throw()
				{
					//const size_t packed_pointer_size = 36;
					const size_t lo_bits = 7;

					//const uintptr_t lo_mask = ((1ull << 7) - 1);
					//const uintptr_t hi_mask = ~((1ull << 43) - 1);

					return pointer >> lo_bits;
				}

				//128 bit aligned pointer on windows with 43 bits used in it
				static inline uintptr_t unpack_pointer( uintptr_t pointer) throw()
				{
					//const size_t packed_pointer_size = 36;
					const size_t lo_bits = 7;

					//const uintptr_t lo_mask = ((1ull << 7) - 1);
					const uintptr_t hi_mask = ~((1ull << 43) - 1);

					return (pointer << lo_bits) & ~hi_mask;
				}

				//encodes 128bit aligned 43 bit pointer, count and a version in 64 bits
				inline static uintptr_t encode_pointer(uintptr_t pointer, size_t count, size_t version) throw()
				{
					uintptr_t packed_pointer = pack_pointer(pointer);
					return   count << (36 + 9) | (version << 36) | ( packed_pointer );
				}

				inline static uintptr_t encode_pointer(void* pointer, size_t count, size_t version) throw()
				{
					return encode_pointer( reinterpret_cast<uintptr_t>(pointer), count, version);
				}

				inline static size_t get_version(uintptr_t pointer) throw()
				{
					const uint64_t  mask	=  ~((1ull << (36 + 9)) - 1 );

					//128 bit aligned pointer on windows with 43 bits used in it
					return static_cast<size_t> ( (pointer & ~mask) >> 36);
				}

				inline static size_t get_version(void* pointer) throw()
				{
					return get_version( reinterpret_cast<uintptr_t> (pointer) );
				}

				inline static size_t get_counter(uintptr_t pointer) throw()
				{
					const uint64_t  mask	=  ~((1ull << (36 + 9)) - 1 );

					//128 bit aligned pointer on windows with 43 bits used in it
					return static_cast<size_t> ( (pointer & mask) >> (36 + 9) );
				}

				inline static size_t get_counter(void* pointer) throw()
				{
					return get_counter(reinterpret_cast<uintptr_t>(pointer));
				}

				inline static void* decode_pointer(uintptr_t pointer) throw()
				{
					//128 bit aligned pointer on windows with 43 bits used in it
					const uint64_t  mask	=  ~((1ull << (36 )) - 1 );
					return reinterpret_cast<void*> (unpack_pointer( pointer & ~mask )) ;
				}

				inline static void* decode_pointer(void* pointer) throw()
				{
					return decode_pointer( reinterpret_cast<uintptr_t>(pointer) ) ;
				}
			}

			class __declspec( align(64) ) concurrent_stack
			{
				public:

				concurrent_stack() : m_top(nullptr)
				{

				}

				template <typename t> void push( t* pointer) throw()
				{
					push_( reinterpret_cast<concurrent_stack_element*> (pointer) );
				}

				//pointer must point to memory of 64 bytes at least
				void push(uintptr_t pointer) throw()
				{
					push_( reinterpret_cast<concurrent_stack_element*> (pointer) );
				}

				uintptr_t pop() throw()
				{
					return reinterpret_cast<uintptr_t> ( pop_() );
				}

				template <typename t> t* pop() throw()
				{
					return reinterpret_cast<t*> ( pop_() );
				}

				size_t size() const throw()
				{
					auto versioned_top = std::atomic_load(&m_top);
					auto top =  reinterpret_cast<concurrent_stack_element*> (details1::decode_pointer(versioned_top));

					if (top == nullptr)
					{
						return 0;
					}
					else
					{
						return details1::get_counter(versioned_top);
					}
				}

				bool empty() const throw()
				{
					return (size() == 0);
				}

				private:

				struct __declspec( align(64) ) concurrent_stack_element
				{
					concurrent_stack_element() : m_next(nullptr)
					{

					}

					std::uint8_t				m_opaque_data[8];
					concurrent_stack_element*	m_next;
					uint8_t m_pad[ 64 - sizeof(concurrent_stack_element*)  - 8 * sizeof(std::uint8_t) ];
				};


				void push_(concurrent_stack_element* pointer) throw()
				{
					auto	top		= std::atomic_load(&m_top);

					pointer->m_next = reinterpret_cast<concurrent_stack_element*> (details1::decode_pointer(top));
					
					auto	new_top	= reinterpret_cast<concurrent_stack_element*> (details1::encode_pointer(pointer, details1::get_counter(top) + 1, details1::get_version( top ) + 1 ) );
					auto	delay_value = 2;

					while (!std::atomic_compare_exchange_weak( &m_top, &top, new_top  ) )
					{
						delay_value			= delay(delay_value);
						pointer->m_next		= reinterpret_cast<concurrent_stack_element*> (details1::decode_pointer(top));
						new_top				=  reinterpret_cast<concurrent_stack_element*> (details1::encode_pointer(pointer, details1::get_counter(top) + 1, details1::get_version( top ) + 1 ) );
					}
				}

				concurrent_stack_element* pop_() throw()
				{
					auto versioned_top = std::atomic_load(&m_top);
					auto top =  reinterpret_cast<concurrent_stack_element*> (details1::decode_pointer(versioned_top));

					if (top == nullptr)
					{
						return 0;
					}

					auto new_top = reinterpret_cast<concurrent_stack_element*> ( details1::encode_pointer( top->m_next, details1::get_counter(versioned_top) - 1, details1::get_version( versioned_top ) + 1) );

					auto delay_value = 2;
					while( !std::atomic_compare_exchange_weak( &m_top, &versioned_top, new_top  ))
					{
						delay_value = delay(delay_value);
						top =  reinterpret_cast<concurrent_stack_element*> (details1::decode_pointer(versioned_top));
						new_top = reinterpret_cast<concurrent_stack_element*> ( details1::encode_pointer( top->m_next, details1::get_counter(versioned_top) - 1, details1::get_version( versioned_top ) + 1) );
					}

					return top;
				}

				//perform exponential backoff
				static uint32_t delay(uint32_t value) throw()
				{
					volatile uint32_t i;

					for (i = 0; i < value; ++i)
					{
						;
					}

					return value * value;
				}

				std::atomic<concurrent_stack_element*>	m_top;
				uint8_t									m_pad[ 64 - sizeof(std::atomic<concurrent_stack_element*>) ];
			};
		}

		typedef details::concurrent_stack concurrent_stack;

		//---------------------------------------------------------------------------------------
        class __declspec( align(64) ) stack
        {
            public:

            stack() : m_top(nullptr)
            {

            }

			//pointer must point to memory of at least 16 bytes
            void push(uintptr_t pointer) throw()
            {
				stack_element* element = reinterpret_cast<stack_element*>  (pointer);
				element->m_next = m_top;
				m_top = element;
            }

			uintptr_t pop() throw()
            {
				stack_element* element = m_top;

				if (element == nullptr)
				{
					return 0;
				}

				m_top = element->m_next;
				return reinterpret_cast<uintptr_t> ( element );
            }

			struct __declspec( align(64) ) stack_element
            {
                stack_element() : m_next(nullptr)
                {

                }

				std::uint8_t		m_opaque_data[8];
                stack_element*		m_next;
                uint8_t				m_pad[ 64 - sizeof(stack_element*)  - 8 * sizeof(std::uint8_t) ];
            };

			private:
			stack_element*			m_top;
            uint8_t                 m_pad[ 64 - sizeof(stack_element*) ];
        };

		
		template <typename t, typename op> class counted_stack : protected op
		{
			private:
			typedef typename t stack;

			public:
			counted_stack() : m_items(0)
			{

			}

			void push(uintptr_t pointer) throw()
            {
				op::increment(m_items);
				m_stack.push(pointer);
			}

			uintptr_t pop() throw()
            {
				op::decrement(m_items);
				return m_stack.pop();
			}

			int32_t size() const throw()
			{
				return m_items;
			}

			private:
			stack		m_stack;
			int32_t		m_items;
		};

        class super_page;

		typedef std::uint16_t	size_class;

        //---------------------------------------------------------------------------------------
        //page_block are the basic elements for allocations 
        class __declspec( align(128) ) page_block : public list_element<page_block>
        {
        public:
            page_block(super_page* super_page, uintptr_t memory, uint32_t memory_size) throw() : 
                    m_super_page(super_page)
                  , m_memory(memory)
                  , m_memory_size(memory_size)
                  , m_unallocated_offset(0)
                  , m_free_objects(0)
				  ,	m_thread_id(0xFFFFFFFF)
                  , m_lifo_list(0)
                  , m_free_offset(0)
                  , m_size_class(0)
              {
              }

		      size_class get_size_class() const throw()
			  {
				  return m_size_class;
			  }

			  void set_size_class(size_class size_class) throw()
              {
                  m_size_class = size_class;
              }

              bool full() const throw()
              {
                  return m_free_objects == 0;
              }

              bool empty() const throw()
              {
                  return m_free_objects == convert_to_object_offset(m_memory_size);
              }

              uint32_t get_memory_size() const throw()
              {
                  return static_cast<uint32_t> (m_memory_size);
              }

			  void reset(size_class size_class) throw()
			  {
				  size_class = size_class;
				  m_free_objects = convert_to_object_offset(m_memory_size);
				  m_unallocated_offset = 0;
			  }

              void* allocate() throw()
              {
                  void* result = nullptr;

                  if (m_free_offset != 0)
                  {
                      uint32_t offset = convert_to_bytes(m_free_offset);

                      result = reinterpret_cast<void*> ( m_memory + offset ) ;

                      m_free_offset = * reinterpret_cast<uint16_t*> ( result );
                      --m_free_objects;
                  }
                  else
                  {
                      uint32_t offset = convert_to_bytes(m_unallocated_offset);

                      result = reinterpret_cast<void*> ( m_memory + offset ) ;

                      m_unallocated_offset++;

                      if ( convert_to_bytes(m_unallocated_offset) > m_memory_size)
                      {
                          m_unallocated_offset = 0;
                      }

                      --m_free_objects;
                  }

                  return result;
              }

              void free(void* pointer) throw()
              {
                  uintptr_t pointer_a = m_memory;
                  uintptr_t pointer_b = reinterpret_cast<uintptr_t> (pointer);
                  uintptr_t offset = pointer_b - pointer_a;

                  * reinterpret_cast<uint16_t*>(pointer) = m_free_offset;
                  m_free_offset = convert_to_object_offset(offset);
                  ++m_free_objects;
              }

              super_page*     get_super_page() const throw()
              {
                  return m_super_page;
              }

        private:

            page_block();
            page_block(const page_block&);
            const page_block operator=(const page_block&);

            uint32_t        m_buddy_order;          //buddy order from super page allocation information. aliased memory, not used in page_block, must be the first member
			uint32_t		m_free_objects;

			uintptr_t		m_opaque_ptr;			//for free page lists

			super_page*     m_super_page;
            uintptr_t		m_memory;
            uintptr_t		m_memory_size;			//memory size in bytes


			uint32_t		m_thread_id;
			uint32_t		m_lifo_list;

            uint16_t	    m_unallocated_offset;	//can support offsets in pages up to 256kb
            uint16_t	    m_free_offset;			

			size_class		m_size_class;

			uint8_t			m_pad[58];

            uint32_t convert_to_bytes(uint16_t blocks) const throw()
            {
                return blocks * m_size_class;
            }

            uint16_t convert_to_object_offset(uintptr_t bytes) const throw()
            {
                return static_cast<uint16_t> ( bytes /  m_size_class );
            }
        };

        //---------------------------------------------------------------------------------------
        // super_page manages blocks of pages between 16kb and 256kb
        // it uses buddy allocation scheme, which splits 2^k blocks in half on allocation and merges them back on free
        // the overhead is 1 bit per allocated block. we pack it in the allocated data as a header.
        class super_page : public list_element<super_page>
        {
            static const uint32_t page_size = 4096;
            static const uint32_t super_page_size = 4 * 1024 * 1024;
            static const uint32_t page_count = super_page_size / page_size;
            static const uint32_t buddy_max_order = detail::log2_c<page_count>::value;

            typedef void (*free_super_page_callback)(super_page*, uintptr_t, void*);

            public:
            super_page(void* sp_base, free_super_page_callback free_callback, void* callback_parameter) throw() :
                m_sp_base(reinterpret_cast<uintptr_t> (sp_base) )
                , m_free_callback(free_callback)
                , m_callback_parameter(callback_parameter)
                , m_largest_free_order( static_cast<uint32_t> (buddy_max_order) )
            {
                //make the memory as one big block and mark it as free
                buddy_element* block = new (m_sp_base) buddy_element(buddy_max_order);
                m_buddies[ buddy_max_order ].m_buddy_elements.push_front(block);
                block->set_tag();
            }

            page_block* alllocate(std::size_t size) throw()
            {
                uint32_t        size_in_pages   =   static_cast<uint32_t> ( size / page_size) ;
                uint32_t        order           =   detail::log2( size_in_pages );
                buddy_element*  buddy           =   nullptr;
                uint32_t        k;

                //find block
                for(k = order; k < buddy_max_order + 1 ; ++k )
                {
                    buddy_block_list* buddies = &m_buddies[k].m_buddy_elements;

                    if ( !buddies->empty() )
                    {
                        buddy = buddies->front();
                        buddies->pop_front();
                        buddy->clear_tag();
                        break;
                    }
                }

                if (buddy != nullptr)
                {
                    //split the block in pieces until we get the correct size
                    while ( k > order)
                    {
                        --k;

                        uintptr_t  buddy_address        =   reinterpret_cast<uintptr_t>(buddy);     
                        uintptr_t  right_half_address   =   buddy_address + ( page_size * ( 1 << k ) ) ;
                        buddy_element* right_half       =   new (right_half_address) buddy_element(k);
                        buddy_block_list* buddies       =   &m_buddies[k].m_buddy_elements;

                        buddies->push_front(right_half);
                        right_half->set_tag();
                    }


                    //the page_block is a meta information before the memory base
                    uintptr_t memory_base = reinterpret_cast<uintptr_t> (buddy) + sizeof(page_block);
                    uint32_t  memory_size = static_cast<uint32_t> ( size - sizeof(page_block) );

                    update_largest_free_order();

                    //convert the buddy to page_block
                    return new (buddy) page_block(this, memory_base, memory_size);
                }
                else
                {
                    return nullptr;
                }
            }

            void free(page_block* block) throw()
            {
                uintptr_t       block_address   = reinterpret_cast<uintptr_t>(block); 
                uint32_t        block_size      = block->get_memory_size() + sizeof(page_block); // alignment?
                uint32_t        size_in_pages   = block_size / page_size;
                uint32_t        k               = detail::log2(size_in_pages);
                uintptr_t       buddy_address   = buddy(block_address, k, m_sp_base );
                buddy_element*  p               = reinterpret_cast<buddy_element*>(buddy_address);


                block->~page_block();

                //find buddy and merge the blocks if needed
                //The order of these checks is important, since p can point to invalid memory
                while ( 
                        !
                        (
                            ( k == buddy_max_order || p->get_tag() == 0 ) ||
                            ( p->get_tag() == 1 && p->get_order() != k )
                        )
                      )
                {
                    buddy_block_list*   buddies     = &m_buddies[k].m_buddy_elements;
                    buddies->remove(p);

                    ++k;

                    if ( buddy_address < block_address )
                    {
                        block_address = buddy_address;
                    }

                    buddy_address   = buddy(block_address, k, m_sp_base);
                    p               = reinterpret_cast<buddy_element*>(buddy_address);
                }

                buddy_block_list*   buddies = &m_buddies[k].m_buddy_elements;
                buddy_element*      element = new (block_address) buddy_element(k);
                buddies->push_front(element);
                element->set_tag();

                //if all pages are freed return this super_page to the os
                buddy_block_list*   buddies_max_order = &m_buddies[buddy_max_order].m_buddy_elements;
                if ( !buddies_max_order->empty())
                {
                    m_free_callback(this, m_sp_base, m_callback_parameter);
                }
                else
                {
                    update_largest_free_order();
                }
            }

            uint16_t get_largest_free_order() const throw()
            {
                    return m_largest_free_order;
            }

        private:

            struct buddy_element : public list_element<buddy_element>
            {
                explicit buddy_element(uint32_t order) : m_order(order)
                {

                }

                uint32_t get_order() const throw()
                {
                    return m_order & 0x7FFFFFFF;
                }

                uint32_t get_tag() const throw()
                {
                   return _bittest( (long*)&m_order, 31);
                }

                void set_tag() throw()
                {
                    _bittestandset( (long*) &m_order, 31);
                }

                void clear_tag() throw()
                {
                    _bittestandreset( (long*) &m_order, 31);
                }

                static void* operator new(size_t, uintptr_t where) throw()
                {
                    return reinterpret_cast<void*> (where);
                }

                static void operator delete(void*, uintptr_t) throw()
                {

                }

                //todo
                //operator new []

                private:
                buddy_element();
                uint32_t    m_order;    //1 bit for tag
            };

            typedef list<buddy_element>     buddy_block_list;

            struct buddy
            {
                buddy_block_list    m_buddy_elements;
            };

            uintptr_t                   m_sp_base;
            free_super_page_callback    m_free_callback;
            void*                       m_callback_parameter;

            //buddy system allocation is described by Knuth in The Art of Computer Programming vol 1.
            buddy                       m_buddies[buddy_max_order + 1];             
            uint16_t                    m_largest_free_order;

            //get the buddy of a member address with given order.
            static uintptr_t buddy(uintptr_t pointer, uint32_t order, uintptr_t base) throw()
            {
                // x + 2^k if x mod 2^(k+1) == 0
                // x - 2^k if x mod 2^(k+1) == 2^k
                //if we are at the even buddy, this code returns the odd buddy and vice versa
                //note the calculations are in bytes, that is why we need the page_size

                uintptr_t pointer_1 = pointer - base;
                const uintptr_t value_1 = page_size * ( 1 << (order) );
                const uintptr_t value_2 = pointer_1 ^ value_1;
                return value_2 + base;
            }

            void update_largest_free_order() throw()
            {
                uint32_t result = 0;

                for(int32_t k = buddy_max_order; k >=0 ; --k )
                {
                    buddy_block_list* buddies = &m_buddies[k].m_buddy_elements;

                    if ( !buddies->empty() )
                    {
                        result = k;
                        break;
                    }
                }
                m_largest_free_order = static_cast<uint16_t>(result);
            }
        };

        inline void free_page_block(page_block* block) throw()
        {
            super_page* page = block->get_super_page();
            page->free(block);
        }

        //---------------------------------------------------------------------------------------
        //super pages have meta information for the memory they manage. this allocator manages the memory for them
        //it uses chunk allocator which allocates chunks of memory. they are never freed back
        //the freed headers form a queue in the freed memory and can be taken back.
        template <uint32_t super_page_header_size>
        class super_page_header_allocator
        {
            static const uint32_t chunk_size = 4096 - 8; // 8 is the size of the internal headers of the chunk heap
            typedef virtual_alloc_heap_fixed<1024*1024, 1>       virtual_alloc_heap;
            typedef chunk_heap< chunk_size, virtual_alloc_heap > super_page_headers;

        public:
            explicit super_page_header_allocator(virtual_alloc_heap* virtual_alloc_heap) : 
                  m_chunk_heap(virtual_alloc_heap)
                , m_free(0)
                , m_memory(0)
                , m_free_objects(0)
                , m_unallocated_offset(0)
            {

            }

            void* allocate() throw()
            {
                super_page* result = nullptr;

                if (m_free !=0 )
                {
                    result = reinterpret_cast<super_page*> ( m_free ) ;

                    uintptr_t* free_content = reinterpret_cast<uintptr_t*>(m_free);
                    m_free = *free_content ;
                    --m_free_objects;
                }
                else
                {
                    if (m_free_objects == 0 || m_memory == 0)
                    {
                        m_memory = reinterpret_cast<uintptr_t> ( m_chunk_heap.allocate(chunk_size) );

                        if (m_memory != 0)
                        {
                            const uint16_t super_page_size = static_cast<uint16_t>(super_page_header_size);
                            m_unallocated_offset = 0;
                            m_free_objects +=  chunk_size  / super_page_size;
                        }
                    }

                    if (m_memory != 0)
                    {
                        result = reinterpret_cast<super_page*> ( m_memory + m_unallocated_offset  ) ;
                        m_unallocated_offset += sizeof(super_page);
                        --m_free_objects;
                    }
                }

                return result;
            }

            void free(void* page) throw()
            {
                uintptr_t* free_content = reinterpret_cast<uintptr_t*>(page);
                *free_content = m_free;
                m_free = reinterpret_cast<uintptr_t>(page);
                ++m_free_objects;
            }

        private:
            super_page_headers      m_chunk_heap;           

            uintptr_t               m_free;			        
            uintptr_t               m_memory;               

            uint32_t                m_free_objects;         
            uint32_t                m_unallocated_offset;	

        };

        //---------------------------------------------------------------------------------------
        //bibop tables are used as a technique to reduce per allocation headers and battle overhead of small allocations
        class bibop_table
        {
            public:

            struct bibop_object
            {
                typedef enum _type
                {
                    tiny = 0x0,
                    large = 0x1
                } type;

                type get_type() const throw()
                {
                    return static_cast<type> (m_value >> 7 & 0x01);
                }

                uint8_t get_disposition() const throw()
                {
                    return m_value & 0x7f;
                }

                void set_value(uint8_t value) throw()
                {
                    m_value = value;
                }

                private:
                // in 1 bit encodes the type of the memory in a page and in 7 bits encodes offset to the page block header in pages
                uint8_t m_value;
            };

            explicit bibop_table(uintptr_t memory_base) : m_memory_base(memory_base)
            {

            }

            explicit bibop_table(void* memory_base) : m_memory_base(reinterpret_cast<uintptr_t> (memory_base) )
            {

            }

            void register_tiny_pages(uintptr_t start_page, uint32_t page_count, uintptr_t page_block)  throw()
            {
                const uint32_t  page_size   = 4096;
                uint32_t        start_index = get_index(m_memory_base, start_page);
                uintptr_t       offset      = ( start_page - page_block ) / page_size; //result should be [0;127]

                for(uint32_t i = start_index; i < start_index + page_count; ++i, ++offset )
                {
                    m_pages[i].set_value( static_cast<uint8_t> (offset) ) ;
                }
            }

            void register_large_pages(uintptr_t start_page, uint32_t page_count) throw()
            {
                uint32_t start_index = get_index(m_memory_base, start_page);

                //mark all pages in this range as large pages;
                uint8_t* t = reinterpret_cast<uint8_t*>(&m_pages[start_index]);
                std::memset( t , page_count, 0x80 );
            }

            page_block* decode(const void* pointer) const throw()
            {
                bibop_object obj = decode_pointer(pointer);
                return decode(pointer, obj);
            }

            static page_block* decode(const void* pointer, bibop_object obj) throw()
            {
                const uint32_t  page_size   = 4096;
                uintptr_t   p               = reinterpret_cast<uintptr_t>(pointer);
                uintptr_t   p_              = ( p - obj.get_disposition() ) * page_size;
                return reinterpret_cast<page_block*> (p_);
            }

            bibop_object decode_pointer(const void* pointer) const throw()
            {
                uint32_t index = get_index(m_memory_base, pointer);
                return m_pages[ index ]; 
            }

            private:
            
            bibop_object m_pages[ 1024*1024 ];  // suitable for 4gb address space and page size 4096
            uintptr_t    m_memory_base;         //the beginning of the page table, used for addresses higher than 4gb

            static uint32_t get_index(uintptr_t memory_base, uintptr_t address) throw()
            {
                const uint32_t page_size = 4096;
                const uintptr_t p = address - memory_base;
                return ( p & (page_size - 1) )  >> detail::log2_c<page_size>::value;
            }

            static uint32_t get_index(uintptr_t memory_base, const void* pointer) throw()
            {
                return get_index( memory_base, reinterpret_cast<uintptr_t>(pointer) );
            }
        };


        //---------------------------------------------------------------------------------------
        class super_page_manager
        {
            static const uint32_t       super_page_size   =   4 * 1024 * 1024;
            typedef list<super_page>    super_page_list;
            typedef list<page_block>	page_block_list;

        public:
            super_page_manager() throw() : 
              m_header_allocator(&m_os_heap_header)
              , m_bibop(m_os_heap_header.get_heap_base())
            {

            }

            super_page* allocate_super_page() throw()
            {
                //1. allocate memory for the header
                void* super_page_header = m_header_allocator.allocate();

                if (super_page_header)
                {
                    //2. allocate memory for the pages
                    void* sp_base = m_os_heap_pages.allocate();

                    if (sp_base)
                    {
                        super_page* page = new 
							 (super_page_header) super_page(sp_base, free_super_page_callback, this);

						//lock?
                        m_super_pages.push_front(page);

                        return page;
                    }
                    else
                    {
						//free the header
						m_header_allocator.free(super_page_header);
						return nullptr;
                    }
                }
                else
                {
                    return nullptr;
                }
            }

			page_block*	get_page_block( std::uint32_t page_size ) throw()
			{
				//scan super page list for a block

			}

        private:
            virtual_alloc_heap_fixed<1024*1024, 1024>           m_os_heap_pages;
            virtual_alloc_heap_fixed<1024*1024, 1>              m_os_heap_header;
            super_page_header_allocator< sizeof(super_page)>    m_header_allocator;

            super_page_list                                     m_super_pages;          //super pages, that manage page_blocks
            page_block_list                                     m_free_page_blocks;     //global cache of free page blocks. orphaned threads put here,
            page_block_list                                     m_orphaned_page_blocks; //global cache of orphaned page blocks. orphaned threads put here,
            bibop_table                                         m_bibop;



            void free(super_page* header, void* super_page_base) throw()
            {
				//lock?
                m_super_pages.remove(header);
                header->~super_page();

                m_os_heap_pages.free(super_page_base);
                m_header_allocator.free(header);
            }

            static void free_super_page_callback(super_page* header, uintptr_t super_page_base, void* callback_parameter) throw()
            {
                super_page_manager* page_manager    = reinterpret_cast<super_page_manager*> (callback_parameter);
                void*               base            = reinterpret_cast<void*> (super_page_base);
                page_manager->free(header, base);
            }
        };

        
        //---------------------------------------------------------------------------------------
        //heap allocated in the thread local storage
        class thread_local_heap
        {

            public:

			void* allocate(size_t, uint32_t size_class) throw()
            {
				/*
                page_block_list* list = &m_page_blocks[size_class];

                void* result = nullptr;

                if ( list && !list->empty() )
                {
                    page_block* block = list->front();

                    if (block->full())
                    {
                        //garbage collect
                    }

                    if (block->full())
                    {
                        //allocate block
                        block = m_page_manager->allocate_page_block(16384);
                    }

                    result = block->allocate();

                    if (block->full())
                    {
                        list->pop_front();
                        list->push_back(block);
                    }

                }
                else
                {
                    //allocate block
                    page_block* block = m_page_manager->allocate_page_block(16384);

                    if (block)
                    {
                        result = block->allocate();

                        if (block->full())
                        {
                            list->push_front(block);
                        }
                    }
                }

                return result;
				*/
            }

            void local_free(void* pointer, page_block* page_block, uint32_t size_class) throw()
            {
					
				/*
                page_block_list* list = &m_page_blocks[size_class];

                page_block->free(pointer);

                if (!page_block->empty())
                {
                    if (list -> front() != page_block)
                    {
                        list->remove(page_block);
                        list->push_front(page_block);
                    }
                }
                else
                {
                    //todo
                    //return block to global structures for reusing
                }
				*/
            }

            private:

            super_page_manager*                 m_page_manager;

        };
    }
}


#endif
