#ifndef __MEM_SSMALLOC_H__
#define __MEM_SSMALLOC_H__

#include <atomic>
#include <cstdint>
#include <numeric>

#include <sys/sys_spin_lock.h>
#include <mem/mem_alloc.h>

//mimic keywords which are not in visual studio 2012 yet
#if !defined(ALIGNAS)

#define ALIGNAS(x)  __declspec( align( x ) )

#endif

#if !defined(THREAD_LOCAL)

#define THREAD_LOCAL __declspec(thread)

#endif

#if defined( MEM_SSMALLOC_DLL_IMPORT )
    #define MEM_SSMALLOC_DLL __declspec(dllimport)
#elif defined( MEM_SSMALLOC_DLL_EXPORT )
    #define MEM_SSMALLOC_DLL __declspec(dllexport)
#else
    #define MEM_SSMALLOC_DLL 
#endif

namespace mem
{
    namespace ssmalloc
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

            inline uintptr_t round_up( uintptr_t value, uintptr_t alignment)
            {
                return ( value / alignment + 1 ) * alignment;
            }

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

            uint8_t m_opaque_data[8];
            T*      m_previous;
            T*      m_next;
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
        typedef uint16_t    size_class;

        typedef uint32_t    thread_id;
        typedef uint32_t    remote_free_queue;

        const std::uint32_t size_classes = 256;

        class remote_free_info
        {
            public:

            remote_free_info() : m_memory_reference(0)
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

            static uint16_t get_version( uint64_t reference  ) throw()
            {
                return static_cast<uint16_t> ( reference >> 32 );
            }

            static uint64_t set_version( uint64_t reference, uint16_t version  ) throw()
            {
                return (static_cast<uint64_t> (version) << 32 ) | ( reference & 0xFFFFFFFF);
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

            std::atomic<uint64_t>   m_memory_reference;
        };


        //---------------------------------------------------------------------------------------
        //page_block are the basic elements for allocations 
        class ALIGNAS(128) memory_chunk : public list_element<memory_chunk>
        {

        public:
            explicit memory_chunk( uintptr_t memory ) throw() : 
                    m_memory( memory )
                  , m_unallocated_offset(1)
                  , m_free_objects(0)
                  , m_free_offset(0)
                  , m_size_class( 0 )
              {
               
              }

              uint32_t get_size_class() const throw()
              {
                  return m_size_class;
              }

              bool full() const throw()
              {
                  return (m_free_objects <= 0);
              }

              bool empty() const throw()
              {
                  return ( m_free_objects == convert_to_object_offset( get_memory_size() ) );
              }

              uintptr_t get_memory() const throw()
              {
                return m_memory;
              }

              uintptr_t get_memory_size() const throw()
              {
                  return 64 * 1024;
              }

              void reset(uint32_t size_class, thread_id thread_id) throw()
              {
                m_size_class = size_class;
                m_free_objects = convert_to_object_offset( get_memory_size() );
                m_unallocated_offset = 1;
                m_free_offset = 0;

                uint64_t new_reference = 0;
                uint64_t reference = 0;


                //todo: check if need to 
                do
                {
                    reference = get_block_info();

                    //create new reference and try to set it
                    new_reference = remote_free_info::set_thread_next_count( thread_id, 0, 0 );
                }
                while (! try_set_block_info_weak( reference, new_reference) );
              }

              void* allocate() throw()
              {
                void* result = nullptr;

                if (m_free_offset != 0)
                {
                    auto offset = convert_to_bytes(m_free_offset - 1);

                    result = reinterpret_cast<void*> ( m_memory + offset ) ;

                    m_free_offset = * reinterpret_cast<uint16_t*> ( result );
                    --m_free_objects;
                }
                else
                {
                    auto offset = convert_to_bytes(m_unallocated_offset - 1);

                    result = reinterpret_cast<void*> ( m_memory + offset ) ;

                    m_unallocated_offset++;

                    if ( convert_to_bytes(m_unallocated_offset) > get_memory_size() )
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

              thread_id get_owning_thread_id() const throw()
              {
                return 0;
              }

              bool	try_set_thread(thread_id)  throw()
              {
                auto reference = m_block_info.m_memory_reference.load();	
                auto new_reference = 0; //todo
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
                auto queue = remote_free_info::get_free_queue(reference);
                auto count = remote_free_info::get_count( queue );
                auto next = remote_free_info::get_next ( queue );

                m_free_offset = next;
                m_free_objects += count;
            }

        private:

            memory_chunk(const memory_chunk&);
            const memory_chunk operator=(const memory_chunk&);

            int32_t             m_free_objects;

            uintptr_t           m_memory;

            remote_free_info    m_block_info;   //frees from other threads go here

            uint16_t            m_unallocated_offset;   //can support offsets in pages up to 256kb
            uint16_t            m_free_offset;
            uint32_t            m_size_class;

            uint8_t             m_pad[68];

            uint32_t convert_to_bytes(uint16_t blocks) const throw()
            {
                return blocks * m_size_class;
            }
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

                    concurrent_stack_element*   m_next;
                    uint8_t m_pad[ 64 - sizeof(concurrent_stack_element*) ];
                };


                void push_(concurrent_stack_element* pointer) throw()
                {
                    auto    top = std::atomic_load(&m_top);

                    pointer->m_next = reinterpret_cast<concurrent_stack_element*> (details1::decode_pointer(top));
                    
                    auto    new_top	= reinterpret_cast<concurrent_stack_element*> (details1::encode_pointer(pointer, details1::get_counter(top) + 1, details1::get_version( top ) + 1 ) );
                    auto    delay_value = 2;

                    while (!std::atomic_compare_exchange_weak_explicit( &m_top, &top, new_top, std::memory_order_seq_cst, std::memory_order_seq_cst ) )
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

                std::atomic<concurrent_stack_element*>  m_top;
                uint8_t                                 m_pad[ 64 - sizeof(std::atomic<concurrent_stack_element*>) ];
            };
        }

        typedef details::concurrent_stack concurrent_stack;

        //---------------------------------------------------------------------------------------
        class stack
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
            struct stack_element
            {
                stack_element() : m_next(nullptr)
                {

                }

                stack_element*      m_next;
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


        inline uint64_t chunk_size(uint64_t chunk_count ) 
        {
            return chunk_count * (sizeof(memory_chunk) + 64 * 1024);
        }

        inline uint64_t chunk_size() 
        {
            return chunk_size( 1 ); 
        }

        class global_pool : private detail::noncopyable
        {
            public:
            global_pool ( uint64_t reserve_size, uint64_t initial_chunk_count ) 
            : m_start(0)
            , m_end(0)
            , m_clean(0)
            , m_size( chunk_size ( std::max<uint64_t>( initial_chunk_count, 1LL ) ) )
            , m_reserve_size(reserve_size)
            {
                m_pool_start = reinterpret_cast<uintptr_t> ( ::VirtualAlloc( 0, reserve_size, MEM_RESERVE, PAGE_READWRITE ) ) ;

                if (m_pool_start == 0)
                {
                   //todo: throw exception
                }

                //align to chunk size the beginning
                m_start = detail::round_up ( m_pool_start, chunk_size() ); 
                const uint64_t size = detail::round_up( m_size, chunk_size() );

                void* start = reinterpret_cast<void*> ( ::VirtualAlloc ( reinterpret_cast<void*> (m_start), size, MEM_COMMIT, PAGE_READWRITE ) );

                if (start == nullptr)
                {
                    //todo: throw exception
                }

                m_start += m_size;

                m_end = m_start + m_size;

                if (m_end == 0 )
                {
                    //todo: cleanup + exception
                }

                m_clean.store(m_start);
            }

            ~global_pool()
            {
                ::VirtualFree( reinterpret_cast<void*> ( m_pool_start ), 0, MEM_RELEASE );
            }

            memory_chunk* allocate_chunk() throw()
            {
                auto result = m_free.pop();
                if (result == 0 )
                {
                    result = m_clean.fetch_add( chunk_size() ) ;

                    if (m_clean >= m_end )
                    {
                        sys::lock<sys::spinlock_fas> l(m_lock);

                        auto clean = m_clean.load();

                        while  ( m_end <= clean )
                        {
                            uintptr_t allocate_size = chunk_size(4);
                            void* end = ::VirtualAlloc ( reinterpret_cast<void*> ( m_end ), allocate_size  , MEM_COMMIT, PAGE_READWRITE ) ;

                            if ( end != nullptr )
                            {
                                m_end = m_end  +  allocate_size;
                            }
                            else
                            {
                                //todo: error
                                return nullptr;
                            }
                        }
                    }
                }

                return new ( reinterpret_cast<void*> ( result ) ) memory_chunk( result + sizeof ( memory_chunk ) );
            }

            void    free_chunk( memory_chunk* chunk) throw()
            {
                m_free.push ( reinterpret_cast<uintptr_t> ( chunk ) );
            }

            private:

            global_pool ();

            sys::spinlock_fas       m_lock;
            concurrent_stack        m_free;

            uintptr_t               m_pool_start;           //virtual memory begin
            uintptr_t               m_start;                //pool begin
            uintptr_t               m_end;                  //pool end of commited chunk memory
            std::atomic<uintptr_t>  m_clean;                //pool begin of uncommited chunk memory

            uint64_t                m_reserve_size;         //pool size : 4GB, 8GB, 16GB, etc..
            uint64_t                m_size;                 //current commited pages
        };

        //remote frees are buffered here before atomic operations. buffering remote frees reduces contention
        class remote_free_buffer
        {
            memory_chunk*   m_memory_chunk;
            stack           m_remote_frees;
            uint32_t        count;
        };

        class private_heap : private detail::noncopyable
        {
            public:

            explicit private_heap( global_pool* global_pool ) : 
                m_dummy_chunk(0)
                , m_global_pool(global_pool)
            {
                for ( uint32_t i = 0; i < size_classes; ++i)
                {
                    m_foreground_chunks[i] = &m_dummy_chunk;
                }
            }

            void* allocate( size_t size ) throw();
            void  free(void* pointer) throw();

            private:
            memory_chunk                    m_dummy_chunk;
            concurrent_stack                m_remote_free_chunks[size_classes];
                    
            memory_chunk*                   m_foreground_chunks[size_classes];
            global_pool*                    m_global_pool;

            list<memory_chunk>              m_background_chunks[size_classes];

            list<memory_chunk>              m_local_free_chunks;

            inline bool is_dummy_chunk( const memory_chunk* chunk ) const
            {
                return (chunk == &m_dummy_chunk);
            }

            memory_chunk* private_heap::get_new_chunk(size_class cls ) throw();
        };

        class heap : private detail::noncopyable
        {
            public:

            heap ( uint64_t reserve_size, uint64_t initial_size ) : m_global_pool( reserve_size, initial_size)
            {

            }

            global_pool         m_global_pool;
            list<private_heap>  m_terminated_heaps;

            private:
            heap();
        };

        MEM_SSMALLOC_DLL void test_ssmalloc();
    }
}

#endif
