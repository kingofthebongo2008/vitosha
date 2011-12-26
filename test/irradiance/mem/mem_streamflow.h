#ifndef __MEM_STREAMFLOW_H__
#define __MEM_STREAMFLOW_H__

#include <cstdint>
#include <cstddef>

#include <windows.h>

namespace mem
{
	namespace streamflow
	{
		const uint32_t page_size = 4096;
		const uint32_t page_bits = 12;
		const uint32_t size_classes = 512;


		template <typename T>
		class list
		{
			public:

			list() : 
				  m_head(nullptr)
				, m_tail(nullptr)
			{

			}

			void push_front(T* block)
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

			void pop_front()
			{
				if (m_head)
				{
					m_head = m_head->get_next();
					m_head->set_previous(nullptr);
				}

				if (m_head == nullptr)
				{
					m_tail = nullptr;
				}
			}

			void push_back(T* block)
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

			void pop_back()
			{
				if (m_tail)
				{
					m_tail = m_tail->get_previous();
				}

				if (m_tail == nullptr)
				{
					m_head = nullptr;
				}
			}

			T* front()
			{
				return m_head;
			}

			const T* front() const
			{
				return m_head;
			}

			T* back()
			{
				return m_tail;
			}

			const T* back() const
			{
				return m_tail;
			}

			void remove(T* block)
			{
				if (block->get_previous())
				{
					page_block* previous = block->get_previous();
					previous->set_next( block->get_next() );
				}
				else
				{
					m_head = block->get_next();
				}

				if (block->get_next())
				{
						page_block* next = block->get_next();
						next->set_previous( block->get_previous() );
				}
				else
				{
					m_tail = block->get_previous();
				}

			}
			
			bool empty() const
			{
				return (m_head == nullptr && m_head == m_tail);
			}

			private:
			T* m_head;
			T* m_tail;
		};

		class super_page
		{
			super_page* m_previous;
			super_page* m_next;
			void*		m_sp_base;
			uint32_t	m_sp_size;
			uint32_t	m_largest_free_order;
			
			
			uint8_t		m_bitmap[4096];

			public:
			super_page() :
				m_previous(nullptr)
				, m_next(nullptr)
				, m_sp_base(nullptr)
				, m_sp_size(0)
				, m_largest_free_order(0)
			{

			}

			inline super_page* get_next()
			{
				return m_next;
			}

			inline const super_page* get_next() const
			{
				return m_next;
			}

			inline void set_next(super_page* next)
			{
				m_next = next;
			}

			inline super_page* get_previous()
			{
				return m_previous;
			}

			inline const super_page* get_previous() const
			{
				return m_previous;
			}

			inline void set_previous(super_page* previous)
			{
				m_previous = previous;
			}
		};

		class page_manager
		{
			typedef list<super_page> super_page_list;

			private:
			super_page_list		m_super_page_headers;
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
				std::uintptr_t p = reinterpret_cast<uintptr_t>(ptr);
				return m_pages[ ( p & (page_size - 1) ) / page_size]; 
			}

			bibop_object m_pages[ 1024*1024 ];
		};

		class page_block
		{
			private:
			typedef uint16_t object_offset;

			public:
			page_block() : 
				m_previous(nullptr)
				, m_next(nullptr)
				, m_memory(0)
				, m_memory_size(0)
				, m_unallocated_offset(0)
				, m_free_offset(0)
				, m_free_objects(0)
				, m_size_class(0)
				, m_thread_id(0)
				, m_lifo_list(0)
			{

			}

			inline void set_size_class(uint32_t size_class)
			{
					m_size_class = size_class;
			}

			inline page_block* get_next()
			{
				return m_next;
			}

			inline const page_block* get_next() const
			{
				return m_next;
			}

			inline void set_next(page_block* next)
			{
				m_next = next;
			}

			inline page_block* get_previous()
			{
				return m_previous;
			}

			inline const page_block* get_previous() const
			{
				return m_previous;
			}

			inline void set_previous(page_block* previous)
			{
				m_previous = previous;
			}

			inline bool full() const
			{
				return m_free_objects == 0;
			}

			inline bool empty() const
			{
				return m_free_objects == convert_to_object_offset(m_memory_size);
			}

			void* allocate()
			{
				void* result = nullptr;

				if (m_free_offset != 0)
				{
					uint32_t offset = convert_to_bytes(m_free_offset);

					result = reinterpret_cast<void*> ( m_memory + offset ) ;

					m_free_offset = * reinterpret_cast<object_offset*> ( result );
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

			void free(void* pointer)
			{
				uintptr_t pointer_a = m_memory;
				uintptr_t pointer_b = reinterpret_cast<uintptr_t> (pointer);
				uintptr_t offset = pointer_b - pointer_a;

				* reinterpret_cast<object_offset*>(pointer) = m_free_offset;
				m_free_offset = static_cast<uint16_t> (offset);
				++m_free_objects;
			}

			private:

			page_block*		m_previous;
			page_block*		m_next;

			uintptr_t		m_memory;
			uint32_t		m_memory_size;			//memory size in bytes

			object_offset	m_unallocated_offset;	//can support offsets in pages up to 256kb
			object_offset	m_free_offset;			
			
			uint32_t		m_free_objects;
			uint32_t		m_size_class;			

			uint32_t		m_thread_id;
			uint32_t		m_lifo_list;

			inline uint32_t convert_to_bytes(object_offset blocks) const
			{
				return blocks * m_size_class;
			}
			
			inline object_offset convert_to_object_offset(uint32_t bytes) const
			{
				return static_cast<object_offset> ( bytes /  m_size_class );
			}
		};


		class heap
		{
			typedef list<page_block>	page_block_list;
			page_block_list				m_page_blocks[ size_classes ];

			public:

			void* allocate(size_t size, uint32_t size_class)
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

			void local_free(void* pointer, page_block* page_block, uint32_t size_class)
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