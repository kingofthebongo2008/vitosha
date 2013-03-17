#ifndef __MEM_ALLOC_ALIGNED_H__
#define __MEM_ALLOC_ALIGNED_H__

#include <memory>

#include <mem/mem_alloc.h>

namespace mem
{
    //default allocators guarantee 8 byte allocations, if you want more guarantees you can inherit from this class
    
    template <typename derived>
    class alloc_aligned 
    {
        private:

        static size_t alignment ()
        {
            return __alignof(derived);
        }

        public:

        //---------------------------------------------------------------------------------------
        void* operator new(std::size_t size)
        {
            size_t allocate_size = align( size, alignment() );

            void* result = ::operator new( size ) ;

            if (result == nullptr)
            {
                throw std::bad_alloc();
            }

            return reinterpret_cast<void*> (  align( result, alignment() ) );
        }

        void operator delete(void* pointer) throw()
        {
            ::operator delete(pointer);
        }
        //---------------------------------------------------------------------------------------
        void* operator new  (std::size_t size, const std::nothrow_t& t) throw()
        {
            size_t allocate_size = align( size, alignment() );

            void* result = ::operator new( size , t) ;

            if (result )
            {
                return reinterpret_cast<void*> (  align( result, alignment() ) );
            }
            else
            {
                return nullptr;
            }
        }

        void operator delete (void* pointer, const std::nothrow_t& t) throw()
        {
            ::operator delete(pointer, t);
        }

        //---------------------------------------------------------------------------------------
        void* operator new  [](std::size_t size)
        {
            size_t allocate_size = align( size, alignment() );

            void* result = ::operator new[]( size ) ;

            if (result == nullptr)
            {
                throw std::bad_alloc();
            }

            return reinterpret_cast<void*> (  align( result, alignment() ) );
        }

        void operator delete[](void* pointer) throw()
        {
            ::operator delete( pointer );
        }

        //---------------------------------------------------------------------------------------
        void* operator new  [](std::size_t size, const std::nothrow_t& t) throw()
        {
            size_t allocate_size = align( size, alignment() );

            void* result = ::operator new[]( size , t) ;

            if (result )
            {
                return reinterpret_cast<void*> (  align( result, alignment() ) );
            }
            else
            {
                return nullptr;
            }
        }

        void operator delete[](void* pointer, const std::nothrow_t& t) throw()
        {
            ::operator delete[]( pointer, t );
        }
    };
}


#endif
