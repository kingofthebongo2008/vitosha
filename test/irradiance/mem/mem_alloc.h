#ifndef __MEM_ALLOC_H__
#define __MEM_ALLOC_H__

#include <cstdint>
#include <cstddef>

#include <windows.h>

namespace mem
{
	inline uintptr_t align(uintptr_t size, size_t alignment)
	{
		return size + (alignment - 1)  & ~(alignment - 1);
	}

	class virtual_alloc_heap
	{
		public:

		void* allocate(std::size_t size, std::size_t alignment) __restrict
		{
			alignment;
			return ::VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN , PAGE_READWRITE);
		}

		void free(void* __restrict pointer, std::size_t size, std::size_t alignment) __restrict
		{
			alignment;
			::VirtualFree(pointer, size, MEM_RELEASE);
		}
	};

	class large_page_virtual_alloc_heap
	{
		public:

		void* allocate(std::size_t size, std::size_t alignment) __restrict
		{
			alignment;
			return ::VirtualAlloc(0, size, MEM_LARGE_PAGES | MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN , PAGE_READWRITE);
		}

		void free(void* __restrict pointer, std::size_t size, std::size_t alignment) __restrict
		{
			alignment;
			::VirtualFree(pointer, size, MEM_RELEASE);
		}
	};

	template <class super_heap> class free_list_heap
    {
    private:
        typedef typename super_heap super;

    public:
        explicit free_list_heap(super_heap* __restrict super_heap)  : m_super_heap(super_heap), m_list(nullptr)
        {

        }

		~free_list_heap() __restrict
        {
            auto pointer = m_list;

            while (pointer != nullptr)
            {
                auto oldPointer = pointer;
                pointer = pointer->m_next;
                m_super_heap->free(oldPointer, oldPointer->m_size, oldPointer->m_alignment);
            }
        }

		void* allocate(std::size_t size, std::size_t alignment) __restrict throw()
        {
			auto data = m_list;

            if (data != nullptr)
            {
				m_list = m_list->m_next;
				return data;
            }
            else
            {
				std::size_t size_allocation = std::max(size, sizeof(free_object));
				return m_super_heap->allocate(size_allocation, alignment);
            }
        }

        void free(void* __restrict pointer, std::size_t size, std::size_t alignment) __restrict throw()
        {
			internal_free(pointer, size, alignment);
        }

    private:
        free_list_heap(const free_list_heap&);
        const free_list_heap& operator=(const free_list_heap&);

        struct free_object
        {
            free_list_heap* __restrict m_next;
			std::size_t m_size;
			std::size_t m_alignment;
        };

		super_heap*		__restrict	m_superHeap;
        free_object*	__restrict	m_list;

		inline void internal_free(void* __restrict pointer, std::size_t size, std::size_t alignment) __restrict throw()
		{
			//super heap must allocate minimum sizeof(free_object)
			auto ptr = reinterpret_cast<free_object* __restrict> (pointer);
			ptr->m_size = size;
			ptr->m_alignment = alignment;
			ptr->m_next = m_list;
			m_list = ptr;
		}
    };


	template <uint32_t chunk_size, class super_heap> class chunk_heap
    {
		public:
		explicit chunk_heap(super_heap* __restrict super_heap)  : m_super_heap(super_heap), m_list(nullptr)
        {

        }

		~chunk_heap() __restrict
        {
            auto pointer = m_list;

            while (pointer != nullptr)
            {
                auto old_pointer = pointer;
                pointer = pointer->m_next;
                m_super_heap->free(old_pointer, chunk_size, old_pointer->m_alignment);
            }
        }

		void* allocate(std::size_t size, std::size_t alignment) __restrict throw()
		{
			alignment;

			size_t size_to_allocate = align(size, align);

			if ( chunk_ptr + size_to_allocate < chunk_size)
			{
				chunk_ptr +=size_to_allocate;
				return chunk_ptr;
			}
			else
			{
				size_t size_to_allocate = align(chunk_size + sizeof(free_object), align);
				m_chunk = m_superHeap->allocate(size_to_allocate, alignment);

				free_object* object = reinterpret_cast<free_object*> (m_chunk);
				object->m_next = m_list;
				m_list = m_chunk;

				m_chunk_ptr = reintrepret_cast<uintptr_t> (m_chunk);
				m_chunk_ptr = align(m_chunk_ptr, align);
				return m_chunk_ptr;
			}
		}

		void free(void* __restrict pointer, std::size_t size, std::size_t alignment) __restrict throw()
		{
			pointer;
			size;
			alignment;
		}

		private:

		struct free_object
        {
            uint8_t* __restrict m_next;
        };

		super_heap*		__restrict	m_superHeap;
		uint8_t*					m_chunk;
		uintptr_t					m_chunk_ptr;
		free_object					m_free_objects;
	};

	typedef chunk_heap< 4096 - 8, virtual_alloc_heap> chunk_heap_1;
}


#endif