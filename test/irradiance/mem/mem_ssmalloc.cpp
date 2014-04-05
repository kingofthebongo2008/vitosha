#include "precompiled.h"

#include <mem/mem_ssmalloc.h>


namespace mem
{
    namespace ssmalloc
    {
        static uint8_t sizemap [] =
        {
            0,  0,  1,  2,  3,  4,  5,  6, 
            7,  8,  9,  10, 11, 12, 13, 14, 
            15, 15, 15, 15, 16, 16, 16, 16, 
            17, 17, 17, 17, 18, 18, 18, 18, 
            19, 19, 19, 19, 19, 19, 19, 19, 
            20, 20, 20, 20, 20, 20, 20, 20, 
            21, 21, 21, 21, 21, 21, 21, 21, 
            22, 22, 22, 22, 22, 22, 22, 22, 
            23, 23, 23, 23, 23, 23, 23, 23, 
            23, 23, 23, 23, 23, 23, 23, 23, 
            23, 23, 23, 23, 23, 23, 23, 23, 
            23, 23, 23, 23, 23, 23, 23, 23, 
            24, 24, 24, 24, 24, 24, 24, 24, 
            24, 24, 24, 24, 24, 24, 24, 24, 
            24, 24, 24, 24, 24, 24, 24, 24, 
            24, 24, 24, 24, 24, 24, 24, 24, 
            25, 25, 25, 25, 25, 25, 25, 25, 
            25, 25, 25, 25, 25, 25, 25, 25, 
            25, 25, 25, 25, 25, 25, 25, 25, 
            25, 25, 25, 25, 25, 25, 25, 25, 
            25, 25, 25, 25, 25, 25, 25, 25, 
            25, 25, 25, 25, 25, 25, 25, 25, 
            25, 25, 25, 25, 25, 25, 25, 25, 
            25, 25, 25, 25, 25, 25, 25, 25, 
            26, 26, 26, 26, 26, 26, 26, 26, 
            26, 26, 26, 26, 26, 26, 26, 26, 
            26, 26, 26, 26, 26, 26, 26, 26, 
            26, 26, 26, 26, 26, 26, 26, 26, 
            26, 26, 26, 26, 26, 26, 26, 26, 
            26, 26, 26, 26, 26, 26, 26, 26, 
            26, 26, 26, 26, 26, 26, 26, 26, 
            26, 26, 26, 26, 26, 26, 26, 26 
        };

        static uint8_t sizemap2 [] =
        {
            0,  26, 27, 28, 29, 29, 30, 30, 
            31, 31, 31, 31, 32, 32, 32, 32, 
            33, 33, 33, 33, 33, 33, 33, 33, 
            34, 34, 34, 34, 34, 34, 34, 34, 
            35, 35, 35, 35, 35, 35, 35, 35, 
            35, 35, 35, 35, 35, 35, 35, 35, 
            36, 36, 36, 36, 36, 36, 36, 36, 
            36, 36, 36, 36, 36, 36, 36, 36, 
            37, 37, 37, 37, 37, 37, 37, 37, 
            37, 37, 37, 37, 37, 37, 37, 37, 
            37, 37, 37, 37, 37, 37, 37, 37, 
            37, 37, 37, 37, 37, 37, 37, 37, 
            38, 38, 38, 38, 38, 38, 38, 38, 
            38, 38, 38, 38, 38, 38, 38, 38, 
            38, 38, 38, 38, 38, 38, 38, 38, 
            38, 38, 38, 38, 38, 38, 38, 38 
        };

        static const uint8_t large_size_class = 39;

        //---------------------------------------------------------------------------------------
        static inline size_class compute_size_class(size_t size)
        {
            if ( size <= 1024)
            {
                return sizemap[ ( size - 1 ) >> 2 ] ;
            }
            else if ( size <= 65536 )
            {
                return sizemap2[ ( size -  1 ) >> 9 ] ;
            }
            else
            {
                return large_size_class;                
            }
        }
        //---------------------------------------------------------------------------------------
        static inline uint32_t compute_size(size_class size_class)
        {
            static  uint16_t reverse[] =
            {	
                8 - 1,  12 - 1, 16 - 1, 20 - 1, 24 - 1, 28 - 1, 32 - 1, 36 - 1, 40 - 1, 
                44 - 1, 48 - 1, 52 - 1, 56 - 1, 60 - 1, 64 - 1, 80 - 1, 96 - 1, 112 - 1, 128 - 1, 
                160 - 1, 192 - 1, 224 - 1, 256 - 1, 384 - 1, 512 - 1, 768 - 1, 1024 - 1, 1536 - 1, 
                2048 - 1, 3072 - 1, 4096 - 1, 6144 - 1, 8192 - 1, 12288 - 1, 16384 - 1, 24576 - 1, 32768 - 1, 
                49152 - 1 ,  65536 - 1, 
            };

            return reverse[size_class] + 1;
        }
        //---------------------------------------------------------------------------------------
        static inline uintptr_t align_chunk( uintptr_t address )
        {
            return address - ( address % chunk_size() ) ;
        }
        //---------------------------------------------------------------------------------------
        static inline memory_chunk* locate_chunk( uintptr_t address )
        {
            return reinterpret_cast<memory_chunk*> ( align_chunk ( address ) );
        }
        //---------------------------------------------------------------------------------------
        static inline memory_chunk* locate_chunk( void* address )
        {
            return reinterpret_cast<memory_chunk*> ( align_chunk ( reinterpret_cast<uintptr_t> ( address ) ) );
        }
        //---------------------------------------------------------------------------------------
        memory_chunk* private_heap::get_new_chunk( size_class cls ) throw()
        {
            auto result = m_background_chunks[cls].front();

            if (result != nullptr)
            {
                m_background_chunks[cls].remove(result);
            }
            else
            {
                //concurrent stack
                result = m_remote_free_chunks[cls].pop<memory_chunk>();

                if ( result != nullptr)
                {
                    result->garbage_collect();
                }
                else
                {
                    result = m_local_free_chunks.front();

                    if (result != nullptr)
                    {
                        m_local_free_chunks.remove(result);
                        result->reset( compute_size(cls), 0);
                    } 
                    else
                    {
                        result = m_global_pool->allocate_chunk();

                        if (result != nullptr)
                        {
                            result->reset( compute_size(cls), 0);
                        }
                    }
                }
            }

            return result;
        }

        //---------------------------------------------------------------------------------------
        void* private_heap::allocate( size_t size ) throw()
        {
            size_class cls = compute_size_class(size);

            retry:
            memory_chunk* chunk = m_foreground_chunks[cls];

            auto result = chunk->allocate();

            if (chunk->full())
            {
                //swap chunk
                m_foreground_chunks[cls] = get_new_chunk( cls );
                if ( is_dummy_chunk( chunk ) )
                {
                    chunk->reset(8, 0 );
                    goto retry;
                }
            }

            return result;
        }
        //---------------------------------------------------------------------------------------
        void  private_heap::free(void* ) throw()
        {

        }

        //---------------------------------------------------------------------------------------
        MEM_SSMALLOC_DLL void test_ssmalloc()
        {
        }

    }
}


