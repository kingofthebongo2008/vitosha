#ifndef __MEM_STREAMFLOW_ALLOCATOR_H__
#define __MEM_STREAMFLOW_ALLOCATOR_H__

#include <cstddef>
#include <mem/mem_streamflow.h>

namespace mem
{
    namespace streamflow
    {
        template <typename T>
        class allocator
        {
            public:
            typedef T               value_type;
            typedef T*              pointer;
            typedef const T*        const_pointer;

            typedef T&              reference;
            typedef const T&        const_reference;
            

            typedef std::size_t     size_type;
            typedef std::ptrdiff_t  difference_type;

            allocator() throw()
            {

            }

            allocator(const allocator& ) throw()
            {

            }

            template <class U> allocator (const allocator<U>& ) throw()
            {

            }

            template<class other> struct   rebind
            {
                typedef   allocator<other>   other;
            };

            pointer address ( reference x ) const throw()
            {
                return (&x);
            }

            const_pointer address ( const_reference x ) const throw()
            {
                return (&x);
            }

            pointer allocate (size_type n, typename allocator<T>::const_pointer hint=0)
            {
                (hint);
                pointer result = reinterpret_cast<pointer> ( get_heap(0)->allocate( n * sizeof(value_type) ) );

                if (result == nullptr)
                {
                    throw std::bad_alloc();
                }

                return result;
            }

            void deallocate (pointer p, size_type )
            {
                if (p != nullptr)
                {
                    get_heap(0)->free(p);
                }
            }

            size_type max_size() const throw()
            {
                return ((std::size_t)(-1) / sizeof (value_type));
            }

            void construct ( pointer p, const_reference val )
            {
                new (p) value_type(val);
            }

            void destroy (pointer p)
            {
                p->~value_type(); 
            }

            //wait till c++11x support
            /*
            template <class U, class... Args> void construct (U* p, Args&&... args)
            {

            }

            template <class U> void destroy (U* p)
            {

                
            }
            */
        };
    }
}

#endif
