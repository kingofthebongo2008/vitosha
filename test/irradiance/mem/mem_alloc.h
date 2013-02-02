#ifndef __MEM_ALLOC_H__
#define __MEM_ALLOC_H__

#include <cstdint>
#include <cstddef>

#include <windows.h>

namespace mem
{
    inline uintptr_t align(uintptr_t size, size_t alignment) throw()
    {
		//static_assert(alignment is power of two)
        return size + (alignment - 1)  & ~(alignment - 1);
    }

	inline bool is_aligned(uintptr_t size, size_t alignment) throw()
    {
		//static_assert(alignment is power of two)
        return ( size & (alignment - 1) ) == 0;
    }

	inline bool is_aligned(void* pointer, size_t alignment) throw()
    {
		//static_assert(alignment is power of two)
        return is_aligned( reinterpret_cast<uintptr_t> ( pointer ), alignment );
    }

    template <typename t, typename h > inline t* allocate(h* heap)
    {
        return reinterpret_cast<t*> (heap->allocate(sizeof(t)));
    }

    template <typename t, typename h > inline t* allocate(h& heap)
    {
        return reinterpret_cast<t*> (heap.allocate(sizeof(t)));
    }
    
    class virtual_alloc_heap
    {
        public:

        void* allocate(std::size_t size) throw()
        {
            return ::VirtualAlloc( 0, size, MEM_COMMIT | MEM_RESERVE , PAGE_READWRITE);
        }

        void free(void* pointer) throw()
        {
            ::VirtualFree(pointer, 0, MEM_RELEASE);
        }
    };

    template <uint32_t reserve_page_count, uint32_t allocation_page_count> class virtual_alloc_heap_fixed
    {
        static const uint32_t page_size = 4096;

        public:

        virtual_alloc_heap_fixed()
        {
            const uintptr_t size = static_cast<uintptr_t> (reserve_page_count) * static_cast<uintptr_t> (page_size);
            m_heap_base = ::VirtualAlloc(0, size,  MEM_RESERVE , PAGE_READWRITE);
            //todo exceptions
        }

        ~virtual_alloc_heap_fixed()
        {
            const uintptr_t size = static_cast<uintptr_t> (reserve_page_count) * static_cast<uintptr_t> (page_size);
            ::VirtualFree(m_heap_base, size, MEM_RELEASE);
        }

        void* allocate() throw()
        {
            const uintptr_t size = static_cast<uintptr_t> (allocation_page_count) * static_cast<uintptr_t> (page_size);
            return ::VirtualAlloc(m_heap_base, size, MEM_COMMIT,  PAGE_READWRITE);
        }

        void* allocate(std::size_t) throw()
        {
            return allocate();
        }

        void free(void* pointer) throw()
        {
            const uintptr_t size = static_cast<uintptr_t> (allocation_page_count) * static_cast<uintptr_t> (page_size);
            ::VirtualFree(pointer, size, MEM_DECOMMIT);
        }

        void* get_heap_base() const
        {
            return m_heap_base;
        }

        void*   m_heap_base;
    };

    class large_page_virtual_alloc_heap
    {
        public:

        void* allocate(std::size_t size) throw()
        {
            return ::VirtualAlloc(0, size, MEM_LARGE_PAGES | MEM_COMMIT | MEM_RESERVE , PAGE_READWRITE);
        }

        void free(void* pointer)  throw()
        {
            ::VirtualFree(pointer, 0, MEM_RELEASE);
        }
    };

    template <class super_heap> class free_list_heap
    {
    private:
        typedef typename super_heap super;

    public:
        explicit free_list_heap(super_heap* super_heap)  : m_super_heap(super_heap), m_list(nullptr)
        {

        }

        ~free_list_heap() throw()
        {
            auto pointer = m_list;

            while (pointer != nullptr)
            {
                auto oldPointer = pointer;
                pointer = pointer->m_next;
                m_super_heap->free(oldPointer, oldPointer->m_size, oldPointer->m_alignment);
            }
        }

        void* allocate(std::size_t size) throw()
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

        void free(void* pointer) throw()
        {
            internal_free(pointer);
        }

    private:
        free_list_heap(const free_list_heap&);
        const free_list_heap& operator=(const free_list_heap&);

        struct free_object
        {
            free_list_heap* m_next;
        };

        super_heap*		m_superHeap;
        free_object*	m_list;

        inline void internal_free(void* pointer) throw()
        {
            //super heap must allocate minimum sizeof(free_object)
            auto ptr = reinterpret_cast<free_object*> (pointer);
            ptr->m_next = m_list;
            m_list = ptr;
        }
    };


    template <uint32_t chunk_size, class super_heap> class chunk_heap
    {
        public:
        explicit chunk_heap(super_heap* super_heap)  : 
            m_super_heap(super_heap)
            , m_chunk_ptr( (uintptr_t)(  (uintptr_t) 0L - (uintptr_t) (chunk_size))  )
            , m_free_objects(nullptr)
        {

        }

        ~chunk_heap() throw()
        {
            auto pointer = m_free_objects;

            while (pointer != nullptr)
            {
                auto old_pointer = pointer;
                pointer = pointer->m_next;
                m_super_heap->free(old_pointer);
            }
        }

        void* allocate(std::size_t size) throw()
        {
            uintptr_t size_to_allocate = size;

            if ( m_chunk_ptr + size_to_allocate < m_chunk_ptr + chunk_size)
            {
                m_chunk_ptr +=size_to_allocate;
                return reinterpret_cast<void*> (m_chunk_ptr);
            }
            else
            {
                uint32_t alignment = 8;
                size_t size_to_allocate = align(chunk_size + sizeof(free_object), alignment);
                void* chunk = m_super_heap->allocate(size_to_allocate);

                free_object* object = reinterpret_cast<free_object*> (chunk);
                object->m_next = m_free_objects;
                m_free_objects = object;

                m_chunk_ptr = reinterpret_cast<uintptr_t> (chunk);
                m_chunk_ptr = align(m_chunk_ptr + sizeof(free_object), alignment);
                return reinterpret_cast<void*> (m_chunk_ptr);
            }
        }

        void free(void* pointer) throw()
        {

        }

        private:
        chunk_heap(const chunk_heap&);
        const chunk_heap& operator=(const chunk_heap&);

        struct free_object
        {
            free_object* m_next;
        };

        super_heap*                 m_super_heap;
        uintptr_t					m_chunk_ptr;
        free_object*                m_free_objects;
    };

}


#endif
