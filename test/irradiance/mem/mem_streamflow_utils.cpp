#include "precompiled.h"

#include <mem/mem_streamflow.h>
#include <memory>

//comment this, if you do not want c++ allocations to go through streamflow
#define MEM_STREAMFLOW_OVERRIDE_OPERATOR_NEW

#if defined(MEM_STREAMFLOW_OVERRIDE_OPERATOR_NEW)

//---------------------------------------------------------------------------------------
void* operator new(std::size_t size)
{
    void* result = mem::streamflow::get_heap(0)->allocate( size );

    if (result == nullptr)
    {
        throw std::bad_alloc();
    }

    return result;
}

void operator delete(void* pointer) throw()
{
    if (pointer != nullptr)
    {
        mem::streamflow::get_heap(0)->free(pointer);
    }
}
//---------------------------------------------------------------------------------------
void* operator new   (std::size_t size, const std::nothrow_t&) throw()
{
    return mem::streamflow::get_heap(0)->allocate( size );
}

void operator delete (void* pointer, const std::nothrow_t&) throw()
{
    if (pointer != nullptr)
    {
        mem::streamflow::get_heap(0)->free(pointer);
    }
}
//---------------------------------------------------------------------------------------
void* operator new  [](std::size_t size)
{
    void* result = mem::streamflow::get_heap(0)->allocate( size );

    if (result == nullptr)
    {
        throw std::bad_alloc();
    }

    return result;
}

void operator delete[](void* pointer) throw()
{
    if (pointer != nullptr)
    {
        mem::streamflow::get_heap(0)->free(pointer);
    }
}
//---------------------------------------------------------------------------------------
void* operator new  [](std::size_t size, const std::nothrow_t&) throw()
{
    return mem::streamflow::get_heap(0)->allocate( size );
}

void operator delete[](void* pointer, const std::nothrow_t&) throw()
{
    if (pointer != nullptr)
    {
        mem::streamflow::get_heap(0)->free(pointer);
    }
}
//---------------------------------------------------------------------------------------
#endif