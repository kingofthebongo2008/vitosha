#ifndef __MEM_STREAMFLOW_H__
#define __MEM_STREAMFLOW_H__

#include <cstdint>

//Paper: Scalable Locality-Conscious Multithreaded Memory Allocation

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
        class MEM_STREAMFLOW_DLL heap
        {
            public:

            void*   allocate(size_t size) throw();
            void    free(void* pointer) throw();
            void*   reallocate(void* pointer, size_t size) throw();
            
            private:
            void*   m_implementation;
        };

        //returns one of 8 heaps to be used; default heap is 0
        MEM_STREAMFLOW_DLL heap*  get_heap(uint32_t index) throw();
    }
}

#endif
