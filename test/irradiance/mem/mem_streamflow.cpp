#include "precompiled.h"

#include <mem/mem_streamflow.h>

namespace mem
{
    namespace streamflow
    {
		static const std::uint32_t							size_classes = 256;
		static const std::uint32_t							page_block_size_classes = 5; //16kb, 32kb, 64kb, 128kb, 256kb

		static concurrent_stack								g_orphaned_page_blocks[size_classes];
		static concurrent_stack								g_free_page_blocks[page_block_size_classes];

		
		static __declspec(thread) void*						t_local_heaps_memory;
		static __declspec(thread) thread_local_heap*		t_local_heaps[ size_classes ];

		static __declspec(thread) void*						t_local_inactive_page_blocks_memory;
		static __declspec(thread) concurrent_stack*			t_local_inactive_page_blocks[ page_block_size_classes ];

		static void thread_initialize()
		{
			t_local_heaps_memory = ::VirtualAlloc( 0, size_classes * sizeof(thread_local_heap) , MEM_COMMIT | MEM_RESERVE , PAGE_READWRITE);

			if (t_local_heaps_memory)
			{
				std::uint8_t* memory = reinterpret_cast<std::uint8_t*> ( t_local_heaps_memory);
				for ( std::uint32_t i = 0 ; i < size_classes; ++i)
				{
					t_local_heaps[i] = new (memory) thread_local_heap();

					memory += sizeof(thread_local_heap) ;
					memory = reinterpret_cast<uint8_t*> ( align(reinterpret_cast<uintptr_t>(memory), __alignof(thread_local_heap)) );
				}
			}
			else
			{
				//?????
			}

			t_local_inactive_page_blocks_memory = ::VirtualAlloc( 0, page_block_size_classes * sizeof(concurrent_stack) , MEM_COMMIT | MEM_RESERVE , PAGE_READWRITE);

			if ( t_local_inactive_page_blocks_memory )
			{
				std::uint8_t* memory = reinterpret_cast<std::uint8_t*> ( t_local_inactive_page_blocks_memory);

				for ( std::uint32_t i = 0 ; i < page_block_size_classes; ++i, memory += sizeof(concurrent_stack) )
				{
					t_local_inactive_page_blocks[i] = new (memory) concurrent_stack();
					
					memory += sizeof(concurrent_stack) ;
					memory = reinterpret_cast<uint8_t*> ( align(reinterpret_cast<uintptr_t>(memory), __alignof(concurrent_stack)) );
				}
			}
			else
			{
				//?????
			}
		}

		static void thread_finalize()
		{
			for ( std::uint32_t i = 0 ; i < size_classes; ++i)
			{
				t_local_heaps[i]->~thread_local_heap();
			}

			::VirtualFree(t_local_heaps_memory, 0, MEM_RELEASE);

			for ( std::uint32_t i = 0 ; i < page_block_size_classes; ++i)
			{
				t_local_inactive_page_blocks[i]->~concurrent_stack();
			}

			::VirtualFree(t_local_inactive_page_blocks_memory, 0, MEM_RELEASE);
		}

		static const std::uint16_t base[] =
		{
				0,  16, 24, 28, 30, 31, 31, 32, 32, 32, 
				32, 33, 33, 33, 33, 34, 34, 34, 34, 35, 
				35, 35, 35, 36, 36, 36, 36, 37, 37, 37, 
				37, 38, 38, 38, 38, 39, 39, 39, 39, 40, 
				40, 40, 40, 41, 41, 41, 41, 42, 42, 42, 
				42, 43, 43, 43, 43, 44, 44, 44, 44, 45, 
				45, 45, 45, 46, 46, 46, 46, 47, 47, 47, 
				47, 48, 48, 48, 48, 49, 49, 49, 49, 50, 
				50, 50, 50, 51, 51, 51, 51, 52, 52, 52, 
				52, 53, 53, 53, 53, 54, 54, 54, 54, 55, 
				55, 55, 55, 56, 56, 56, 56, 57, 57, 57, 
				57, 58, 58, 58, 58, 59, 59, 59, 59, 60, 
				60, 60, 60, 61, 61, 61, 61, 62, 62, 62, 
				62, 63, 63, 63, 63, 64, 64, 64, 64, 65, 
				65, 65, 65, 66, 66, 66, 66, 67, 67, 67, 
				67, 68, 68, 68, 68, 69, 69, 69, 69, 70, 
				70, 70, 70, 71, 71, 71, 71, 72, 72, 72, 
				72, 73, 73, 73, 73, 74, 74, 74, 74, 75, 
				75, 75, 75, 76, 76, 76, 76, 77, 77, 77, 
				77, 78, 78, 78, 78, 79, 79, 79, 79, 80, 
				80, 80, 80, 81, 81, 81, 81, 82, 82, 82, 
				82, 83, 83, 83, 83, 84, 84, 84, 84, 85, 
				85, 85, 85, 86, 86, 86, 86, 87, 87, 87, 
				87, 88, 88, 88, 88, 89, 89, 89, 89, 90, 
				90, 90, 90, 91, 91, 91, 91, 92, 92, 92, 
				92, 93, 93, 93, 93, 94, 94, 94, 94
		};

		static const std::uint16_t factor[] =
		{	
				4,	 8,   16,  32,  64,  128, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
				256, 256, 256, 256, 256, 256, 256, 256, 256
		};

		static const std::uint16_t reverse[] =
		{	
			4,     8,     12,    16,    20,    24,    28,    32,    36,    40,
			44,    48,    52,    56,    60,    64,    72,    80,    88,    96,
			104,   112,   120,   128,   144,   160,   176,   192,   224,   256,
			320,   448,   704,   960,   1216,  1472,  1728,  1984,  2240,  2496,
			2752,  3008,  3264,  3520,  3776,  4032,  4288,  4544,  4800,  5056,
			5312,  5568,  5824,  6080,  6336,  6592,  6848,  7104,  7360,  7616,
			7872,  8128,  8384,  8640,  8896,  9152,  9408,  9664,  9920,  10176,
			10432, 10688, 10944, 11200, 11456, 11712, 11968, 12224, 12480, 12736,
			12992, 13248, 13504, 13760, 14016, 14272, 14528, 14784, 15040, 15296,
			15552, 15808, 16064, 16320, 16576
		}; 

		static inline size_class compute_size_class(size_t size)
		{
			const std::uint32_t object_granularity = sizeof(void*);
			const std::uint32_t cache_line_size = 64;

			if (size < object_granularity)
			{
				size = object_granularity;
			}

			std::size_t bin = size / (cache_line_size);
			std::size_t position = (size - 1) % cache_line_size;

			if (size %  cache_line_size  == 0)
			{
				bin = (size - 1) / cache_line_size;
				position = (size - 2) % cache_line_size;
			}

			return static_cast<size_class> ( base[bin] + (position / factor[bin]) );
		}

		static inline std::uint32_t compute_size(size_class size_class)
		{
			return reverse[size_class];
		}

		static inline std::uint32_t compute_page_block_size(size_class size_class)
		{
			const std::uint32_t size = compute_size(size_class);
			const std::uint32_t	objects_per_page_block = 1024;
			const std::uint32_t min = 16384;
			const std::uint32_t max = 262144;

			std::uint32_t size_to_allocate = static_cast<std::uint32_t> ( align( size * objects_per_page_block, 4096) );
			std::uint32_t log_2 = detail::log2(size_to_allocate);

			size_to_allocate= 1 << (log_2 + 1);

			size_to_allocate = std::min<std::uint32_t> ( size_to_allocate, max );
			size_to_allocate = std::max<std::uint32_t> ( size_to_allocate, min );

			return size_to_allocate;
		}

		void test_streamflow()
		{
			size_class size_class = compute_size_class(4);
			auto page_block_size = compute_page_block_size(size_class);
			std::size_t s = sizeof(page_block);
			std::size_t s1 = sizeof(DWORD);
			std::size_t s2 = sizeof(concurrent_stack::concurrent_stack_element);


			int k;
			k +=page_block_size;
		}

    }
}
