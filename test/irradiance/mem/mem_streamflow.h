#ifndef __MEM_STREAMFLOW_H__
#define __MEM_STREAMFLOW_H__

#include <atomic>
#include <cstdint>
#include <limits>
#include <new>
#include <mutex>
#include <exception>

#include <intrin.h>

#include <mem/mem_alloc.h>
#include <sys/sys_spin_lock.h>


//Paper: Scalable Locality-Conscious Multithreaded Memory Allocation

//mimic keywords which are not in visual studio 2012 yet
#if !defined(ALIGNAS)

#define ALIGNAS(x)  __declspec( align( x ) )

#endif

#if !defined(THREAD_LOCAL)

#define THREAD_LOCAL __declspec(thread)

#endif

#if defined( MEM_STREAMFLOW_DLL_IMPORT )
    #define MEM_STREAMFLOW_DLL __declspec(dllimport)
#elif defined( MEM_STREAMFLOW_DLL_EXPORT )
    #define MEM_STREAMFLOW_DLL __declspec(dllexport)
#else
    #define MEM_STREAMFLOW_DLL 
#endif

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
                _BitScanReverse(&result, x);
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

            class noncopyable
            {
                protected:
                noncopyable()
                {
                
                }

                ~noncopyable()
                {
                }

                private:
                noncopyable(const noncopyable&);
                const noncopyable& operator=(const noncopyable&);
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

            uint8_t	m_opaque_data[8]; //buddy order and tag go here
            T*		m_previous;
            T*		m_next;
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
                auto head = m_head;

                if (head == nullptr)
                {
                    m_tail = block;                   
                }
                else
                {
                    head->set_previous(block);
                }

                block->set_next(head);
                block->set_previous(nullptr);
                m_head = block;
            }

            T* front() throw()
            {
                return m_head;
            }

            const T* front() const throw()
            {
                return m_head;
            }

            void remove(T* block) throw()
            {
                T* previous = block->get_previous();
                T* next = block->get_next();

                if ( previous != nullptr)
                {
                    previous->set_next(next);
                }
                else
                {
                    m_head = next;
                }

                if ( next != nullptr)
                {
                    next->set_previous(previous);
                }
                else
                {
                    m_tail = previous;
                }

                if ( m_head != nullptr && m_head->get_next() == nullptr)
                {
                    m_tail = m_head;
                }

                if ( m_tail !=nullptr && m_tail->get_previous() == nullptr)
                {
                    m_head = m_tail;
                }
            }

            bool empty() const throw()
            {
                return (m_head == nullptr && m_head == m_tail);
            }

            void rotate_back() throw()
            {
                T* head = m_head;
                T* tail = m_tail;

                if ( head != tail )
                {
                    auto new_head = head->get_next();
                    new_head->set_previous(nullptr);

                    tail->set_next(head);
                    head->set_previous(tail);
                    head->set_next(nullptr);

                    m_head = new_head;
                    m_tail = head;
                }
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

            class ALIGNAS(64) concurrent_stack
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

                struct ALIGNAS(64) concurrent_stack_element
                {
                    concurrent_stack_element() : m_next(nullptr)
                    {

                    }

                    std::uint8_t                m_opaque_data[8];   //buddy data goes here
                    concurrent_stack_element*   m_next;
                    uint8_t m_pad[ 64 - sizeof(concurrent_stack_element*)  - 8 * sizeof(std::uint8_t) ];
                };


                void push_(concurrent_stack_element* pointer) throw()
                {
                    auto    top = std::atomic_load(&m_top);

                    pointer->m_next = reinterpret_cast<concurrent_stack_element*> (details1::decode_pointer(top));
                    
                    auto    new_top	= reinterpret_cast<concurrent_stack_element*> (details1::encode_pointer(pointer, details1::get_counter(top) + 1, details1::get_version( top ) + 1 ) );
                    auto    delay_value = 2;

                    while (!std::atomic_compare_exchange_weak( &m_top, &top, new_top  ) )
                    {
                        delay_value         = delay(delay_value);
                        pointer->m_next     = reinterpret_cast<concurrent_stack_element*> (details1::decode_pointer(top));
                        new_top             =  reinterpret_cast<concurrent_stack_element*> (details1::encode_pointer(pointer, details1::get_counter(top) + 1, details1::get_version( top ) + 1 ) );
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
        class ALIGNAS(64) stack
        {
            public:

            stack() : m_top(nullptr)
            {

            }


            template <typename t> void push( t* pointer) throw()
            {
                push_( reinterpret_cast<stack_element*> (pointer) );
            }

            //pointer must point to memory of 64 bytes at least
            void push(uintptr_t pointer) throw()
            {
                push_( reinterpret_cast<stack_element*> (pointer) );
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
                return m_counter;
            }
            


            private:
            struct ALIGNAS(64) stack_element
            {
                stack_element() : m_next(nullptr)
                {

                }

                std::uint8_t        m_opaque_data[8];   //buddy data goes here
                stack_element*      m_next;
                uint8_t             m_pad[ 64 - sizeof(stack_element*)  - 8 * sizeof(std::uint8_t) ];
            };


            //pointer must point to memory of at least 16 bytes
            void push_(stack_element* pointer) throw()
            {
                stack_element* element = reinterpret_cast<stack_element*>  (pointer);
                element->m_next = m_top;
                m_top = element;
                m_counter++;
            }

            stack_element* pop_() throw()
            {
                stack_element* element = m_top;

                if (element == nullptr)
                {
                    return 0;
                }

                m_counter--;
                m_top = element->m_next;
                return element;
            }

            private:
            stack_element*      m_top;
            uint32_t            m_counter;
            uint8_t             m_pad[ 64 - sizeof(stack_element*) - sizeof(uint32_t) ];
        };

        class super_page;
        typedef uint16_t    size_class;
        typedef uint32_t    page_block_size_class;
        typedef uint32_t    page_block_size;

        typedef uint32_t    thread_id;
        typedef uint32_t    remote_free_queue;
        const thread_id	    thread_id_orphan = std::numeric_limits<uint32_t>::max();

        inline bool thread_is_orphan(thread_id id) throw()
        {
            return (id == thread_id_orphan);
        }

        class remote_page_block_info
        {
            public:

            remote_page_block_info() : m_memory_reference(0)
            {
                
            }

            static uint16_t get_next(remote_free_queue queue) throw()
            {
                return ( (queue >>16 ) & 0xFFFF);
            }

            static uint16_t get_count(remote_free_queue queue) throw()
            {
                return ( queue  & 0xFFFF );
            }

            static remote_free_queue set_next_count( uint16_t next, uint16_t count) throw()
            {
                return ( (next<< 16 ) | count );
            }

            static thread_id get_thread_id( uint64_t reference  ) throw()
            {
                return static_cast<thread_id> ( reference >> 32 );
            }

            static uint64_t set_thread_id( uint64_t reference, thread_id thread_id  ) throw()
            {
                return (static_cast<uint64_t> (thread_id) << 32 ) | ( reference & 0xFFFFFFFF);
            }

            static remote_free_queue get_free_queue( uint64_t reference ) throw()
            {
                return static_cast<remote_free_queue> ( reference );
            }

            static uint64_t set_free_queue( uint64_t reference, remote_free_queue free_queue  ) throw()
            {
                return ( free_queue ) | ( reference & 0xFFFFFFFF00000000L );
            }

            static uint64_t set_thread_next_count( thread_id thread_id, uint16_t next, uint16_t count) throw()
            {
                return ( static_cast<uint64_t> ( (thread_id) ) << 32 |  (static_cast<uint64_t> ( next ) << 16 ) | static_cast<uint64_t> (count) );
            }

            std::atomic<uint64_t> 	m_memory_reference;
        };

        //---------------------------------------------------------------------------------------
        //page_block are the basic elements for allocations 
        class ALIGNAS(128) page_block : public list_element<page_block>
        {
        public:
            page_block(super_page* super_page, uintptr_t memory, uint32_t memory_size) throw() : 
                    m_super_page(super_page)
                  , m_memory(memory)
                  , m_memory_size(memory_size)
                  , m_unallocated_offset(1)
                  , m_free_objects(0)
                  , m_free_offset(0)
                  , m_size_class( std::numeric_limits<uint32_t>().infinity() )
              {

              }

              uint32_t get_size_class() const throw()
              {
                  return m_size_class;
              }

              bool full() const throw()
              {
                  return (m_free_objects == 0);
              }

              bool empty() const throw()
              {
                  return ( m_free_objects == convert_to_object_offset(m_memory_size) );
              }

              uintptr_t get_memory() const throw()
              {
                return m_memory;
              }

              uintptr_t get_memory_size() const throw()
              {
                  return m_memory_size;
              }

              void reset(uint32_t size_class,thread_id thread_id) throw()
              {
                  m_size_class = size_class;
                  m_free_objects = convert_to_object_offset(m_memory_size);
                  m_unallocated_offset = 1;
                  m_free_offset = 0;

                  uint64_t new_reference = 0;
                  uint64_t reference = 0;


                //todo: check if need to 
                do
                {
                    reference = get_block_info();

                    //create new reference and try to set it
                    new_reference = remote_page_block_info::set_thread_next_count( thread_id, 0, 0 );
                }
                while (! try_set_block_info_weak( reference, new_reference) );
              }

              void* allocate() throw()
              {
                  void* result = nullptr;

                  if (m_free_offset != 0)
                  {
                      auto offset = convert_to_bytes(m_free_offset - 1 );

                      result = reinterpret_cast<void*> ( m_memory + offset ) ;

                      m_free_offset = * reinterpret_cast<uint16_t*> ( result );
                      --m_free_objects;
                  }
                  else
                  {
                      auto offset = convert_to_bytes(m_unallocated_offset - 1);

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
                  auto pointer_a = m_memory;
                  auto pointer_b = reinterpret_cast<uintptr_t> (pointer);
                  auto offset = pointer_b - pointer_a;

                  * reinterpret_cast<uint16_t*>(pointer) = m_free_offset;
                  m_free_offset = convert_to_object_offset(offset) + 1;
                  ++m_free_objects;
              }

              void thread_local_free(void* pointer) throw()
              {
                  free(pointer);
              }

              thread_id		 get_owning_thread_id() const throw()
              {
                  return m_block_info.get_thread_id( m_block_info.m_memory_reference.load() );
              }

              bool	try_set_thread(thread_id thread_id) throw()
              {
                  auto reference = m_block_info.m_memory_reference.load();	
                  auto new_reference = remote_page_block_info::set_thread_id(reference, thread_id);
                  return std::atomic_compare_exchange_weak(&m_block_info.m_memory_reference, &reference, new_reference);
              }

              uint64_t get_block_info() const throw()
              {
                  return m_block_info.m_memory_reference.load();	
              }

              bool try_set_block_info_weak(uint64_t old_value, uint64_t value) throw()
              {
                  return std::atomic_compare_exchange_weak(&m_block_info.m_memory_reference, &old_value, value);
              }

              bool try_set_block_info_strong(uint64_t old_value, uint64_t value) throw()
              {
                  return std::atomic_compare_exchange_weak(&m_block_info.m_memory_reference, &old_value, value);
              }

              uint16_t convert_to_object_offset(uintptr_t bytes) const throw()
              {
                  return static_cast<uint16_t> ( bytes /  m_size_class );
              }

              uint16_t convert_to_object_offset(const void* pointer) const throw()
              {
                  auto pointer_a = m_memory;
                  auto pointer_b = reinterpret_cast<uintptr_t> (pointer);
                  auto offset = pointer_b - pointer_a;
                  return convert_to_object_offset(offset);
              }

            //---------------------------------------------------------------------------------------
            void garbage_collect() throw()
            {
                //reference holds in one 64 bit variable, counter, next pointer and thread id
                auto reference = get_block_info();

                //fetch the old head and version
                auto queue = remote_page_block_info::get_free_queue(reference);
                auto count = remote_page_block_info::get_count( queue );
                auto next = remote_page_block_info::get_next ( queue );

                m_free_offset = next;
                m_free_objects += count;
            }

        private:

            page_block();
            page_block(const page_block&);
            const page_block operator=(const page_block&);
            uint8_t         m_opaque_buddy_data[4];
            uint32_t        m_free_objects;

            super_page*     m_super_page;
            uintptr_t       m_memory;
            uintptr_t       m_memory_size;          //memory size in bytes

            
            remote_page_block_info  m_block_info;   //frees from other threads go here

            uint16_t    m_unallocated_offset;   //can support offsets in pages up to 256kb
            uint16_t    m_free_offset;
            uint32_t    m_size_class;

            uint8_t     m_pad[50];

            uint32_t convert_to_bytes(uint16_t blocks) const throw()
            {
                return blocks * m_size_class;
            }

            super_page*     get_super_page() const throw()
            {
                return m_super_page;
            }

            friend void free_page_block(page_block* block) throw();
            friend void free_page_block_mt_safe(page_block* block) throw();
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
            super_page(void* sp_base, free_super_page_callback free_callback, void* callback_parameter, sys::spinlock_fas* lock ) throw() :
                m_sp_base(reinterpret_cast<uintptr_t> (sp_base) )
                , m_free_callback(free_callback)
                , m_callback_parameter(callback_parameter)
                , m_lock(lock)
                , m_largest_free_order( static_cast<uint32_t> (buddy_max_order) )
            {
                create_new_buddy ( m_sp_base, buddy_max_order);
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
                    buddy_block_list* buddies = &m_buddies[k];

                    if ( !buddies->empty() )
                    {
                        buddy = buddies->front();
                        buddies->remove(buddy);

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
                        uintptr_t  size                 =   ( page_size * ( 1 << k ) ) ;  
                        uintptr_t  right_half_address   =   buddy_address + size;
                        
                        create_new_buddy ( right_half_address, k );
                    }


                    //the page_block is a meta information before the memory base
                    uintptr_t memory_base = reinterpret_cast<uintptr_t> (buddy) + sizeof(page_block);
                    uint32_t  memory_size = static_cast<uint32_t> ( size - sizeof(page_block) );

                    update_largest_free_order();

                    buddy->set_order(order);
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
                uint32_t        block_size      = static_cast<uint32_t> ( block->get_memory_size() + sizeof(page_block) ); // alignment?
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
                            k == buddy_max_order || p->get_tag() == 0 ||
                            ( p->get_tag() == 1 && p->get_order() != k ) 
                        )
                      )
                {
                    buddy_block_list*   buddies     = &m_buddies[k];
                    buddies->remove(p);

                    ++k;

                    if ( buddy_address < block_address )
                    {
                        block_address = buddy_address;
                    }

                    buddy_address   = buddy(block_address, k, m_sp_base);
                    p               = reinterpret_cast<buddy_element*>(buddy_address);
                }

                create_new_buddy( block_address, k );

                //if all pages are freed return this super_page to the os
                buddy_block_list*   buddies_max_order = &m_buddies[buddy_max_order];
                if ( !buddies_max_order->empty())
                {
                    m_free_callback(this, m_sp_base, m_callback_parameter);
                }
                else
                {
                    update_largest_free_order();
                }
            }

            void free_mt_safe(page_block* block) throw()
            {
                sys::lock<sys::spinlock_fas> guard(*m_lock);
                free(block);
            }

            bool has_enough_free_space(uint32_t size) const throw()
            {
                uint32_t        size_in_pages   =   static_cast<uint32_t> ( size / page_size) ;
                uint32_t        order           =   detail::log2( size_in_pages );
                return ( m_largest_free_order < buddy_max_order &&  order <= m_largest_free_order );
            }

        private:

            struct buddy_element : public list_element<buddy_element>
            {
                explicit buddy_element(uint32_t order) : m_order(order)
                {
                    set_tag();
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

                void set_order( uint32_t order)
                {
                    //set the order and mark the memory as used
                    m_order = order;
                }

                private:
                buddy_element();
                uint32_t    m_order;    //1 bit for tag, tag == 1 if the memory is free
            };

            typedef list<buddy_element>     buddy_block_list;


            uintptr_t                   m_sp_base;
            free_super_page_callback    m_free_callback;
            void*                       m_callback_parameter;
            sys::spinlock_fas*          m_lock;

            //buddy system allocation is described by Knuth in The Art of Computer Programming vol 1.
            buddy_block_list            m_buddies[buddy_max_order + 1];             
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
                    buddy_block_list* buddies = &m_buddies[k];

                    if ( !buddies->empty() )
                    {
                        result = k;
                        break;
                    }
                }
                m_largest_free_order = static_cast<uint16_t>(result);
            }


            buddy_block_list* get_buddy_list( uint32_t order )
            {
                return &m_buddies[order];
            }

            buddy_element* create_buddy ( uintptr_t address, uint32_t order )
            {
               return new (reinterpret_cast<void*>  (address) ) buddy_element(order);
            }

            void create_new_buddy( uintptr_t address, uint32_t order )
            {
                buddy_block_list*   buddies = get_buddy_list(order);
                buddy_element*      element = create_buddy( address, order);
                buddies->push_front(element);
            }

        };

        inline void free_page_block(page_block* block) throw()
        {
            super_page* page = block->get_super_page();
            page->free(block);
        }

        inline void free_page_block_mt_safe(page_block* block) throw()
        {
            super_page* page = block->get_super_page();
            page->free_mt_safe(block);
        }

        //---------------------------------------------------------------------------------------
        //super pages have meta information for the memory they manage. this allocator manages the memory for them
        //it uses chunk allocator which allocates chunks of memory. they are never freed back
        //the freed headers form a queue in the freed memory and can be taken back.
        template <uint32_t super_page_header_size>
        class chunked_free_list
        {
            static const uint32_t chunk_size = 4096 - 8; // 8 is the size of the internal headers of the chunk heap
            typedef chunk_heap< chunk_size, virtual_alloc_heap > super_page_headers;

        public:
            explicit chunked_free_list(virtual_alloc_heap* virtual_alloc_heap) : 
                  m_chunk_heap(virtual_alloc_heap)
                , m_free(0)
                , m_memory(0)
                , m_free_objects(0)
                , m_unallocated_offset(0)
            {

            }

            void* allocate() throw()
            {
                void* result = nullptr;

                if (m_free !=0 )
                {
                    result = reinterpret_cast<void*> ( m_free ) ;

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
                        m_unallocated_offset += super_page_header_size;

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
                uintptr_t       offset      = ( start_page - page_block )>> detail::log2_c<page_size>::value; //result should be [0;127]

                for(uint32_t i = start_index; i < start_index + page_count; ++i, ++offset )
                {
                    m_pages[i].set_value( static_cast<uint8_t> (offset) ) ;
                }
            }

            void register_tiny_pages(uintptr_t start, uintptr_t size,  uintptr_t page_block) throw()
            {
                const uint32_t  page_size   = 4096;
                register_tiny_pages(start, static_cast<uint32_t> ( align(size, page_size) >> detail::log2_c<page_size>::value ), page_block);
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
                uintptr_t   p               = reinterpret_cast<uintptr_t>(pointer) >> detail::log2_c<page_size>::value;
                uintptr_t   p_              = ( p - obj.get_disposition() )  << detail::log2_c<page_size>::value;
                return reinterpret_cast<page_block*> (p_);
            }

            bibop_object decode_pointer(const void* pointer) const throw()
            {
                uint32_t index = get_index(m_memory_base, pointer);
                return m_pages[ index ]; 
            }

            bool is_large_object( const void* pointer) const throw()
            {
               bibop_object obj = decode_pointer(pointer);
               return (obj.get_type() == bibop_object::large);
            }

            private:
            
            bibop_object m_pages[ 1024*1024 ];  //suitable for 4gb address space and page size 4096
            uintptr_t    m_memory_base;         //the beginning of the page table, used for addresses higher than 4gb

            static uint32_t get_index(uintptr_t memory_base, uintptr_t address) throw()
            {
                const uint32_t page_size = 4096;
                const uintptr_t p = memory_base - address;
                const uintptr_t result = ( p & ~(page_size - 1))  >> detail::log2_c<page_size>::value;

                return ~result & 0xFFFFF;
            }

            static uint32_t get_index(uintptr_t memory_base, const void* pointer) throw()
            {
                return get_index( memory_base, reinterpret_cast<uintptr_t>(pointer) );
            }
        };

        //---------------------------------------------------------------------------------------
        //on 32 bit platforms bibop tables are very useful, however on 64 bits, there are too big
        //radix page map replaces bibops. setup is number of valid bits, stripped of page bits ( 43 (windows) - 12 ) = 31
        template <uint32_t bits>
        class radix_page_map : private detail::noncopyable
        {
            // How many bits should we consume at each interior level
            static const uint32_t interior_bits = (bits + 2) / 3; // Round-up
            static const uint32_t interior_length = 1 << interior_bits;

            // How many bits should we consume at leaf level
            static const uint32_t leaf_bits = bits - ( 2 * interior_bits);
            static const uint32_t leaf_length = 1 << leaf_bits;

            public:
            explicit radix_page_map(virtual_alloc_heap* heap) : 
                m_allocator(heap)
                , m_root( allocate_node() )
            {

            }

            ~radix_page_map()
            {
                //todo
                //free nodes
            }

            struct leaf
            {
                uintptr_t m_data [ leaf_length ] ;

                leaf() : m_data()
                {

                }
            };

            struct node
            {
                std::atomic<void*>   m_pointers [  interior_length ];

                node() : m_pointers()
                {

                }
            };



            private:

            virtual_alloc_heap*                 m_allocator;
            node*                               m_root;

            public:

            node* allocate_node() throw()
            {
                return reinterpret_cast<node*> ( m_allocator->allocate( sizeof(node) ) );
            }

            void free_node(node* node)  throw()
            {
                m_allocator->free(node);
            }

            leaf*   allocate_leaf()  throw()
            {
                return reinterpret_cast<leaf*> ( m_allocator->allocate( sizeof(leaf) ) );
            }

            void free_leaf(leaf* leaf)  throw()
            {
                m_allocator->free(leaf);
            }

            bool    register_pages(uintptr_t start, uintptr_t page_count,  uintptr_t data) throw()
            {
                const uint32_t page_size = 4096;
                start = start >> detail::log2_c<page_size>::value;

                for ( uintptr_t i = start; i < start + page_count; ++i )
                {
                    const uintptr_t i_1 = i >> ( leaf_bits + interior_bits );
                    const uintptr_t i_2 = (i >> leaf_bits ) & (interior_length - 1 ); 
                    const uintptr_t i_3 = i & ( leaf_length - 1 );

                    std::atomic<void*>* __restrict atomic = &m_root->m_pointers[i_1];
                    auto old_node = atomic->load();

                    if ( old_node == nullptr)
                    {
                        node* __restrict n = allocate_node();

                        if (n == nullptr) 
                        {
                            return false;
                        }

                        //another thread wrote here?
                        if ( ! atomic->compare_exchange_strong( old_node, n ) )
                        {
                            //yes, delete
                            free_node(n);
                            //and load again
                            old_node = atomic->load();
                        }
                        else
                        {
                            old_node =  n;
                        }
                    }

                    node* __restrict  n = reinterpret_cast<node*> ( old_node );
                    std::atomic<void*>* __restrict atomic2 = &n->m_pointers[i_2];
                    auto    old_leaf = atomic2->load();

                    if ( old_leaf == nullptr)
                    {
                        leaf* __restrict l = allocate_leaf();

                        if (l == nullptr) 
                        {
                            return false;
                        }

                        //another thread wrote here?
                        if ( ! atomic2->compare_exchange_strong( old_leaf, l ) )
                        {
                            //yes, delete
                            free_leaf(l);

                            //and load again
                            old_leaf = atomic2->load();
                        }
                        else
                        {
                            old_leaf = l;
                        }
                    }

                    leaf* __restrict l = reinterpret_cast<leaf*> ( old_leaf );
                    l->m_data[i_3] = data; 
                }

                return true;
            }

            void register_tiny_pages(uintptr_t start, uintptr_t size, uintptr_t page_block_address) throw()
            {
                const uint32_t  page_size   = 4096;
                register_pages(start, static_cast<uint32_t> ( align(size, page_size) >> detail::log2_c<page_size>::value ), page_block_address);
            }

            void register_large_pages(uintptr_t start, uintptr_t size, uintptr_t block_size) throw()
            {
                const uint32_t  page_size   = 4096;
                register_pages(start, static_cast<uint32_t> ( align(size, page_size) >> detail::log2_c<page_size>::value ), encode_large_object(block_size) );
            }

            uintptr_t get_data( uintptr_t address ) const throw()
            {
                const uint32_t page_size = 4096;
                const uintptr_t start = address >> detail::log2_c<page_size>::value;
                const uintptr_t i_1 = start >> ( leaf_bits + interior_bits );
                const uintptr_t i_2 = (start >> leaf_bits ) & (interior_length - 1 ); 
                const uintptr_t i_3 = start & ( leaf_length - 1 );

                node* n = reinterpret_cast<node*> (  m_root->m_pointers[i_1].load() ) ;
                leaf* l = reinterpret_cast<leaf*> (  n->m_pointers[i_2].load() );

                return l->m_data[i_3];
            }

            static bool is_large_object ( uintptr_t value ) throw()
            {
                return value != decode_large_object(value);
            }

            static uintptr_t   encode_large_object( uintptr_t block_size) throw()
            {
                return  (~0x7fffffffffffffffUL) | block_size;
            }

            static uintptr_t   decode_large_object( uintptr_t address) throw()
            {
                return (address & 0x7fffffffffffffffUL);
            }
        };
        //---------------------------------------------------------------------------------------
        class super_page_manager
        {
            static const uint32_t       super_page_size   =   4 * 1024 * 1024;
            typedef list<super_page>    super_page_list;

        public:
            super_page_manager() throw() : 
              m_header_allocator(&m_os_heap_header)
              , m_page_map(&m_os_heap_pages)
            {

            }

            super_page* allocate_super_page() throw();
            page_block*	allocate_page_block( std::uint32_t page_size ) throw();

            page_block* decode_pointer(const void* pointer) const throw()
            {
                return reinterpret_cast<page_block*> ( m_page_map.get_data( reinterpret_cast<uintptr_t> ( pointer )));
            }

            uintptr_t decode_large_object( const void* pointer ) const throw()
            {
                return m_page_map.decode_large_object( m_page_map.get_data( reinterpret_cast<uintptr_t> ( pointer ) ) ) ;
            }

            bool is_large_object(const void* pointer) const throw()
            {
                return m_page_map.is_large_object( m_page_map.get_data( reinterpret_cast<uintptr_t> ( pointer ) ) );
            }

            void*       allocate_large_block( size_t size ) throw();
            void        free_large_block( void* pointer) throw();

        private:
            sys::spinlock_fas                                   m_super_pages_lock;
            virtual_alloc_heap                                  m_os_heap_pages;
            virtual_alloc_heap                                  m_os_heap_header;
            chunked_free_list< sizeof(super_page) >             m_header_allocator;

            super_page_list                                     m_super_pages;          //super pages, that manage page_blocks
            radix_page_map<31>                                  m_page_map;


            super_page* get_super_page( std::uint32_t page_size ) throw();

            void free(super_page* header, void* super_page_base) throw()
            {
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
        class internal_heap;

        //---------------------------------------------------------------------------------------
        //heap allocated in the thread local storage
        class thread_local_heap
        {
            public:
            thread_local_heap() throw()
            {

            }
            

            bool empty() const throw()
            {
                return m_active_blocks.empty();
            }
            
            void push_front(page_block* block) throw()
            {
                m_active_blocks.push_front(block);
            }

            void remove(page_block* block) throw()
            {
                m_active_blocks.remove(block);
            }

            page_block* front() throw()
            {
                return m_active_blocks.front();
            }

            void rotate_back() throw()
            {
                m_active_blocks.rotate_back();
            }

            private:
            thread_local_heap(const thread_local_heap&);
            const thread_local_heap& operator=(const thread_local_heap&);
            list<page_block>					m_active_blocks;
        };

        const std::uint32_t      size_classes = 256;
        const std::uint32_t      page_block_size_classes = 5; //16kb, 32kb, 64kb, 128kb, 256kb

        class internal_heap
        {
            public:
            explicit internal_heap(uint32_t index) : m_index(index)
            {

            }

            void* allocate(uint32_t size) throw();
            void free(void* pointer) throw();

            uint32_t    get_index() const throw()
            {
                return m_index;
            }

            void push_orphaned_block(page_block* block, uint32_t size_class)  throw()
            {
                m_page_blocks_orphaned[size_class].push(block);
            }

            void free_page_block(page_block* block, uint32_t size_class) throw();

            void free_page_blocks() throw();
            

            private:

            super_page_manager              m_super_page_manager;

            concurrent_stack                m_page_blocks_orphaned[size_classes];						//freed on thread finalize, partially free
            concurrent_stack                m_page_blocks_free[page_block_size_classes];				//global cache of free page blocks goes here up to 1

            uint32_t                        m_index;                                                    //index of the heap in thread local storage

            page_block*                     get_free_page_block( uint32_t size, thread_id thread_id ) throw();

            thread_local_heap*              get_thread_local_heap(uint32_t size) throw();

            internal_heap(const internal_heap&);
            const internal_heap& operator=(const internal_heap&);

            void local_free(void* pointer, page_block* block, thread_local_heap* local_heap, stack* stack1, concurrent_stack* stack2) throw();
            
        };

        class thread_local_info
        {
            public:
            thread_local_info() throw()
            {

            }

            thread_local_heap               t_local_heaps[ size_classes ];                              // per size class heap with blocks
            stack                           t_local_inactive_page_blocks[ page_block_size_classes ];    //local cache of free page blocks, //completely free (on free) goes here up to 4
        };


        class thread_local_heap_info
        {

            public:
            thread_local_heap_info() {}

            thread_local_info* get_thread_local_info(uint32_t heap_index) throw()
            {
                return &m_infos[heap_index];
            }

            private:

            thread_local_info   m_infos[8];
        };

        class MEM_STREAMFLOW_DLL heap
        {
            public:

            void*   allocate(size_t size) throw();
            void    free(void* pointer) throw();
            void*   reallocate(void* pointer, size_t size) throw();
            
            private:
            void*   m_implementation;
        };


        enum initialization_code : uint32_t
        {
            success = 0,
            no_memory = 1
        };

        //called once per application before all threads start to allocate
        initialization_code initialize() throw();

        //called once per application before all threads stop to allocate
        void                finalize()  throw();

        //called on every thread creation before the thread starts to allocate
        initialization_code thread_initialize() throw();

        //called on every thread creation after the thread stops to allocate
        void                thread_finalize()  throw();

        //returns one of 8 heaps to be used; default heap is 0
        MEM_STREAMFLOW_DLL heap*  get_heap(uint32_t index) throw();


        class exception : public std::exception
        {

        };


        class initializer
        {
            public:
            initializer()
            {
                initialization_code code = initialize();

                if ( code != initialization_code::success )
                {
                    throw exception();
                }
            }

            ~initializer()
            {
                finalize();
            }
        };

        class thread_initializer
        {
            public:
            thread_initializer()
            {
                initialization_code code = thread_initialize();

                if ( code != initialization_code::success )
                {
                    throw exception();
                }
            }

            ~thread_initializer()
            {
                thread_finalize();
            }
        };
    }
}

#endif
