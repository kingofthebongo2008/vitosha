#ifndef __MEM_STREAMFLOW_H__
#define __MEM_STREAMFLOW_H__

#include <cstdint>
#include <cstddef>
#include <intrin.h>

#include <windows.h>
#include <mem/mem_alloc.h>

namespace mem
{
    namespace streamflow
    {
        namespace detail
        {
            inline uint32_t log2(uint32_t value)
            {
                unsigned long result;
                _BitScanForward(&result, value);
                return static_cast<uint32_t>(result);
            }
        }


        template <typename T>
        class list_element 
        {
        public:

            list_element() :
              m_previous(nullptr)
                  , m_next(nullptr)
              {

              }

              inline T* get_next() throw()
              {
                  return m_next;
              }

              inline const T* get_next() const throw()
              {
                  return m_next;
              }

              inline void set_next(T* next) throw()
              {
                  m_next = next;
              }

              inline T* get_previous() throw()
              {
                  return m_previous;
              }

              inline const T* get_previous() const throw()
              {
                  return m_previous;
              }

              inline void set_previous(T* previous) throw()
              {
                  m_previous = previous;
              }

              T*  m_previous;
              T*  m_next;
        };

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

        class page_block : public list_element<page_block>
        {
        public:
            page_block() throw() : 
              m_memory(0)
                  , m_memory_size(0)
                  , m_unallocated_offset(0)
                  , m_free_offset(0)
                  , m_free_objects(0)
                  , m_size_class(0)
                  , m_thread_id(0)
                  , m_lifo_list(0)
              {

              }

              inline void set_size_class(uint32_t size_class) throw()
              {
                  m_size_class = size_class;
              }

              inline bool full() const throw()
              {
                  return m_free_objects == 0;
              }

              inline bool empty() const throw()
              {
                  return m_free_objects == convert_to_object_offset(m_memory_size);
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

        private:

            uintptr_t		m_memory;
            uint32_t		m_memory_size;			//memory size in bytes

            uint16_t	    m_unallocated_offset;	//can support offsets in pages up to 256kb
            uint16_t	    m_free_offset;			

            uint32_t		m_free_objects;
            uint32_t		m_size_class;			

            uint32_t		m_thread_id;
            uint32_t		m_lifo_list;

            inline uint32_t convert_to_bytes(uint16_t blocks) const throw()
            {
                return blocks * m_size_class;
            }

            inline uint16_t convert_to_object_offset(uint32_t bytes) const throw()
            {
                return static_cast<uint16_t> ( bytes /  m_size_class );
            }
        };

        class super_page : public list_element<super_page>
        {
            static const uint32_t page_size = 4096;
            static const uint32_t super_page_size = 4 * 1024 * 1024;
            static const uint32_t page_count = super_page_size / page_size;
            static const uint32_t buddy_max_order = 11;

        public:
            template <uint32_t order> struct buddy_bitmap_size
            {
                static const uint32_t min_size_bytes = sizeof(unsigned long);
                static const uint32_t bytes  = page_count / ( (1 << order) * 8) ;
                static const uint32_t value = (bytes < min_size_bytes ? min_size_bytes : bytes) + buddy_bitmap_size<order-1>::value;
            };

            template <> struct buddy_bitmap_size<0>
            {
                static const uint32_t min_size_bytes = sizeof(unsigned long);
                static const uint32_t bytes  = page_count / ( (1 << 0) * 8 );
                static const uint32_t value = (bytes < min_size_bytes ? min_size_bytes : bytes);
            };



            super_page(void* sp_base) throw() :
            m_sp_base(sp_base)
                , m_largest_free_order( buddy_max_order - 1 )
            {
                //mark all blocks as free
                std::fill(&m_buddies_bitmap[0], &m_buddies_bitmap[0] + sizeof(m_buddies_bitmap), 0xFF);


                //set up pointer for tags
                uint32_t address = 0;
                for(uint32_t i=0; i < buddy_max_order; ++i)
                {
                    m_buddies[i].m_buddy_bitmap = reinterpret_cast<unsigned long*> (&m_buddies_bitmap[address]);

                    const uint32_t bits = page_count / ( (1 << i) * 8);
                    const uint32_t value = (bits <  sizeof(unsigned long) ?  sizeof(unsigned long) : bits);
                    address += value;
                }

                //make the memory as one big block
                buddy_element* block = new (m_sp_base) buddy_element();
                m_buddies[buddy_max_order - 1 ].m_buddy_elements.push_front(block);


            }

            page_block* alllocate(std::size_t size) throw()
            {
                uint32_t order = detail::log2( size / page_size );

                buddy_element* buddy = nullptr;

                for(uint32_t i = order; i < buddy_max_order;++i)
                {
                    buddy_block_list* buddies = &m_buddies[i].m_buddy_elements;

                    if ( !buddies->empty() )
                    {
                        buddy = buddies->front();



                        buddies->pop_front();
                    }
                }

                //buddy allocation scheme
                return nullptr;
            }

            void free(page_block* block) throw()
            {

            }

            void*   get_sp_base()
            {
                return m_sp_base;
            }

        private:

            struct buddy_element : public list_element<buddy_element>
            {

            };

            typedef list<buddy_element>     buddy_block_list;

            struct buddy
            {
                buddy_block_list    m_buddy_elements;
                unsigned long*      m_buddy_bitmap;
            };

            void*		    m_sp_base;
            uint32_t	    m_largest_free_order;

            //buddy system allocation is described by Knuth in The Art of Computer Programming vol 1.

            buddy           m_buddies[buddy_max_order];             
            uint8_t         m_buddies_bitmap[ buddy_bitmap_size<buddy_max_order>::value ];

            public:

            static inline uint32_t buddy_index(uintptr_t buddy, uint32_t order, uintptr_t memory_base)
            {
                uintptr_t offset = buddy - memory_base;
                uintptr_t page_index = offset / ( page_size * (1 << order) );
                return static_cast<uint32_t>(page_index);
            }

            static inline uintptr_t buddy(uintptr_t pointer, uint32_t order)
            {
                // x + 2^k if x mod 2^(k+1) == 0
                // x - 2^k if x mod 2^(k+1) == 2^k
                //if we are at the even buddy, this code returns the odd buddy and vice versa

                const uintptr_t value_1 = 1 << order;
                const uintptr_t value_2 = pointer ^ value_1;
                return value_2;
            }

            inline void clear_bit(unsigned long* address, uint32_t bit)
            {
                _bittestandreset( (long*) address, bit);
            }

            inline void set_bit(unsigned long* address, uint32_t bit)
            {
                _bittestandset( (long*) address, bit);
            }

            inline uint32_t test_bit(unsigned long* address, uint32_t bit)
            {
                return _bittest( (long*)address, bit);
            }
        };


        class super_page_header_allocator
        {
            static const uint32_t chunk_size = 4096 - 8; // 8 is the size of the internal headers of the chunk heap
            typedef chunk_heap< chunk_size,  virtual_alloc_heap > super_page_headers;

        public:
            explicit super_page_header_allocator(virtual_alloc_heap* virtual_alloc_heap) : 
            m_virtual_alloc_heap(virtual_alloc_heap)
                , m_chunk_heap(m_virtual_alloc_heap)
                , m_free(0)
                , m_memory(0)
                , m_free_objects(0)
                , m_unallocated_offset(0)
            {

            }

            super_page* allocate() throw()
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
                            const uint16_t super_page_size = static_cast<uint16_t>(sizeof(super_page));
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

            void free(super_page* page) throw()
            {
                uintptr_t* free_content = reinterpret_cast<uintptr_t*>(page);
                *free_content = m_free;
                m_free = reinterpret_cast<uintptr_t>(page);
                ++m_free_objects;
            }

        private:
            virtual_alloc_heap*     m_virtual_alloc_heap;   
            super_page_headers      m_chunk_heap;           

            uintptr_t               m_free;			        
            uintptr_t               m_memory;               

            uint32_t                m_free_objects;         
            uint32_t                m_unallocated_offset;	

        };

        class super_page_manager
        {
            static const uint32_t super_page_size = 4 * 1024 * 1024;
            typedef list<super_page> super_page_list;


        public:
            super_page_manager() throw() : m_header_allocator(&m_virtual_alloc_heap)
            {

            }

            super_page* allocate() throw()
            {
                //1. allocate memory for the header
                void* super_page_memory = m_header_allocator.allocate();

                if (super_page_memory)
                {
                    //2. allocate memory for the pages
                    void* sp_base = m_virtual_alloc_heap.allocate(super_page_size);

                    if (sp_base)
                    {
                        super_page* page = new (super_page_memory) super_page(sp_base);
                        m_super_pages.push_front(page);

                        return page;
                    }
                    else
                    {
                        return nullptr;
                    }
                }
                else
                {
                    return nullptr;
                }
            }

            void free(super_page* pointer) throw()
            {
                void* sp_base = pointer->get_sp_base();
                m_virtual_alloc_heap.free(pointer);
                m_header_allocator.free(pointer);
            }

        private:
            virtual_alloc_heap              m_virtual_alloc_heap;
            super_page_header_allocator     m_header_allocator;
            super_page_list                 m_super_pages;
        };

        class bibop_table
        {
            struct bibop_object
            {
                uint8_t m_type : 1;
                uint8_t m_disposition : 7;
            };

            bibop_object decode(const void* ptr) const
            {
                const uint32_t page_size = 4096;
                std::uintptr_t p = reinterpret_cast<uintptr_t>(ptr);
                return m_pages[ ( p & (page_size - 1) ) / page_size]; 
            }

            bibop_object m_pages[ 1024*1024 ];
        };



        const uint32_t size_classes = 512;

        class heap
        {
            typedef list<page_block>	page_block_list;
            page_block_list				m_page_blocks[ size_classes ];

        public:

            void* allocate(size_t size, uint32_t size_class) throw()
            {
                page_block_list* list = &m_page_blocks[size_class];

                void* result = nullptr;

                if ( list && !list->empty() )
                {
                    page_block* block = list->front();
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

                }

                return nullptr;
            }

            void local_free(void* pointer, page_block* page_block, uint32_t size_class) throw()
            {
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
                    //return block to global structures for reusing
                }
            }
        };
    }
}


#endif
