#include "precompiled.h"

#include <mem/mem_streamflow.h>


namespace mem
{
    namespace streamflow
    {
        //these are per heap actually
		static std::atomic<thread_id>						g_thread_id = thread_id_orphan;
	
        static THREAD_LOCAL void*                           t_thread_local_heap_info_memory;
        static THREAD_LOCAL thread_local_heap_info*         t_thread_local_heap_info;

		static THREAD_LOCAL thread_id						t_thread_id;


		static thread_id create_thread_id()
		{
			thread_id id = std::atomic_fetch_add(&g_thread_id, 1);

			if (id == thread_id_orphan)
			{
				id =  std::atomic_fetch_add(&g_thread_id, 1);
			}

			return id;
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
		//---------------------------------------------------------------------------------------
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
		//---------------------------------------------------------------------------------------
		static inline std::uint32_t compute_size(size_class size_class)
		{
			return reverse[size_class];
		}
		//---------------------------------------------------------------------------------------
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
		//---------------------------------------------------------------------------------------
		static inline std::uint32_t compute_page_block_size_class( std::uint32_t page_block_size)
		{
			const uint32_t page = 4096;
			const uint32_t page_count = (16 * 1024) / page;;
			const uint32_t min_page_log = detail::log2_c<page_count>::value;

			//16kb, 32kb, 64kb, 128kb, 256kb
			return detail::log2( page_block_size / page ) - min_page_log;
		}
		//---------------------------------------------------------------------------------------
		super_page* super_page_manager::allocate_super_page() throw()
        {
            //1. allocate memory for the header
            void* super_page_header = m_header_allocator.allocate();

            if (super_page_header)
            {
                //2. allocate memory for the pages
                void* sp_base = m_os_heap_pages.allocate( 4096 * 1024);

                if (sp_base)
                {
                    super_page* page = new 
							(super_page_header) super_page(sp_base, free_super_page_callback, this, &m_super_pages_lock);


                    m_super_pages.push_front(page);

					return page;
                }
                else
                {
					//free the header
					m_header_allocator.free(super_page_header);
					return nullptr;
                }
            }
            else
            {
                return nullptr;
            }
        }

		//---------------------------------------------------------------------------------------
		super_page* super_page_manager::get_super_page(std::uint32_t page_block_size) throw()
		{
        	//scan super page list for a block
            if ( !m_super_pages.empty() )
            {
                super_page* page = m_super_pages.front();

                while (page)
                {
                    if (page->has_enough_free_space(page_block_size))
                    {
                        return page;
                    }

                    page = page->get_next();
                }
            }
			
			return nullptr;
		}
		//---------------------------------------------------------------------------------------
		page_block*	super_page_manager::allocate_page_block( std::uint32_t page_block_size ) throw()
		{
            sys::lock<sys::spinlock_fas> guard(m_super_pages_lock);
			
			super_page* super_page = get_super_page(page_block_size);

			if (super_page == nullptr)
			{
				super_page = allocate_super_page();
			}

			if (super_page)
			{
                page_block* result = super_page->alllocate(page_block_size);

                if (result)
                {
                    m_page_map.register_tiny_pages( result->get_memory(), result->get_memory_size(), reinterpret_cast<uintptr_t> ( result ) );
                }

				return  result;
			}
			else
			{
				return nullptr;
			}
		}
		//---------------------------------------------------------------------------------------
        void*   super_page_manager::allocate_large_block( size_t size ) throw()
        {
            void* result = m_os_heap_pages.allocate(size);

            if (result)
            {
                sys::lock<sys::spinlock_fas> guard(m_super_pages_lock);
                m_page_map.register_large_pages( reinterpret_cast<uintptr_t> ( result ), size, reinterpret_cast<uintptr_t> ( result ) );
            }

            return result;
        }
        //---------------------------------------------------------------------------------------
        void    super_page_manager::free_large_block( void* pointer) throw()
        {
            m_os_heap_pages.free( reinterpret_cast<void*> ( decode_large_object(pointer) ) );
        }
        //---------------------------------------------------------------------------------------
		static page_block* get_free_page_block(concurrent_stack* stack_1, concurrent_stack* stack_2)
		{
			page_block* block = reinterpret_cast<page_block*> ( stack_1->pop() );

			if ( block == nullptr )
			{
				block = reinterpret_cast<page_block*> ( stack_2->pop() );
			}

			if (block && block->full())
			{
                block->garbage_collect();
			}

			return block;
		}
		//---------------------------------------------------------------------------------------
		static page_block* get_free_page_block( uint32_t size, uint32_t page_block_size, super_page_manager* page_manager, concurrent_stack* stack_1, concurrent_stack* stack_2, thread_id thread_id )
		{
			page_block* block = get_free_page_block(stack_1, stack_2);

			if (block == nullptr)
			{
				block = page_manager->allocate_page_block(page_block_size);
                block->reset(size, thread_id);
			}
			else if ( block->get_size_class() != size)
			{
				block->reset(size, thread_id);
			}
			

			return block;
		}

		//---------------------------------------------------------------------------------------
		page_block* heap::get_free_page_block( uint32_t size, thread_id thread_id )
		{
			size_class size_class		= compute_size_class(size);
			uint32_t page_block_size	= compute_page_block_size( size_class );
			uint32_t page_block_class	= compute_page_block_size_class( page_block_size );


			concurrent_stack* stack_1 = &m_page_blocks_free[size_class];
			concurrent_stack* stack_2 = &m_page_blocks_orphaned[page_block_class];

			super_page_manager* page_manager = &m_super_page_manager;

			return streamflow::get_free_page_block( compute_size(size_class), page_block_size, page_manager, stack_1, stack_2, thread_id);
		}

		//---------------------------------------------------------------------------------------
		static void remote_free(void* pointer, page_block* block, thread_local_heap* heap, thread_id thread_id);

		static void adopt_page_block( void* pointer, page_block* block, thread_local_heap* heap, thread_id thread_id)
		{
			//try to set this thread as owner
			if ( block->try_set_thread( thread_id ) )
			{
				heap->push_front(block);
				block->free(pointer);
			}
			else
			{
				//another thread took ownership of the block, do a remote free
				remote_free( pointer, block, heap, thread_id );
			}
		}

		//---------------------------------------------------------------------------------------
		static void remote_free(void* pointer, page_block* block, thread_local_heap* heap, thread_id thread_id)
		{
			uint64_t reference = 0;
			uint64_t new_reference = 0;

			do
			{
                //reference holds in one 64 bit variable, counter, next pointer and thread id
				reference = block->get_block_info();
				auto block_thread_id = remote_page_block_info::get_thread_id( reference );

				if (block_thread_id != thread_id_orphan)
				{
                    //fetch the old head and version
                    remote_free_queue queue = remote_page_block_info::get_free_queue(reference);
					uint16_t count = remote_page_block_info::get_count( queue );
					uint16_t next = remote_page_block_info::get_next ( queue );

                    //store the old offset in the empty space
					uint16_t offset = block->convert_to_object_offset( pointer );
                    * reinterpret_cast<uint16_t*> ( pointer ) = next;
                    count++;
                    next = offset;
                    
                    //create new reference and try to set it
                    new_reference = remote_page_block_info::set_thread_next_count( block_thread_id, next, count );
				}
				else
				{
					adopt_page_block(pointer, block, heap, thread_id);
					break;
				}
			}
			while (! block->try_set_block_info_weak( reference, new_reference) );
		}

        thread_local_heap* heap::get_thread_local_heap(uint32_t size) throw()
        {
            thread_local_info* local_heap_info = t_thread_local_heap_info->get_thread_local_info( get_index() );
            size_class c = compute_size_class(size);
            thread_local_heap*  local_heap = &local_heap_info->t_local_heaps[c];

            return local_heap;
        }

        void* heap::allocate(uint32_t size) throw()
        {
            if ( size < 2048 )
            {
                page_block* block = nullptr;
                thread_local_info* local_heap_info = t_thread_local_heap_info->get_thread_local_info( get_index() );
                size_class c = compute_size_class(size);

                thread_local_heap*  local_heap = &local_heap_info->t_local_heaps[c];

                if ( local_heap->empty() )
                {
                    //1. check the inactive blocks
                    const uint32_t page_block_size	= compute_page_block_size( c );
			        const uint32_t page_block_class	= compute_page_block_size_class( page_block_size );
                    stack* inactive_blocks      = &local_heap_info->t_local_inactive_page_blocks[page_block_class];
                    block = inactive_blocks->pop<page_block>();

                    if ( block == nullptr)
                    {
                        //2. check the orphaned and global page_blocks
                        block = get_free_page_block( size, t_thread_id );
                    }

                    local_heap->push_front(block);
                }
                else
                {
                    block = local_heap->front();

                    if (block->full())
                    {
                        block->garbage_collect();
                    }

                    if (block->full())
                    {
                        local_heap->rotate_back();
                        block = get_free_page_block(size, t_thread_id );
                        if (block)
                        {
                            local_heap->push_front(block);
                        }
                    }
                }

                if (block)
                {
                    void* result = block->allocate();

                    if (block->full())
                    {
                        local_heap->rotate_back();
                    }

                    return result;
                }

                return nullptr;
            }
            else
            {
                size_t size_to_allocate = align(size, 4096 );
                return this->m_super_page_manager.allocate_large_block(size_to_allocate);
            }
        }

        static void global_free_page_block( page_block* block, concurrent_stack* global_free_stack )
        {
            const uint32_t global_inactive_blocks = 4;
            if ( global_free_stack->size() < global_inactive_blocks )
            {
                global_free_stack->push(block);
            }
            else
            {
                free_page_block_mt_safe(block);
            }
        }

        void heap::free_page_block(page_block* block, page_block_size_class size_class) throw()
        {
            global_free_page_block( block, &m_page_blocks_free[size_class] );
        }

        void heap::free_page_blocks() throw()
        {
            for (uint32_t i = 0; i < page_block_size_classes;++i)
            {
                page_block* block = m_page_blocks_free[i].pop<page_block>();
                if ( block != nullptr)
                {
                    free_page_block( block, i);
                }
            }
        }

        void heap::local_free(void* pointer, page_block* block, thread_local_heap* local_heap, stack* stack1, concurrent_stack* stack2) throw()
        {
            block->free(pointer);

            if (block->empty())
            {
                local_heap->remove(block);

                const uint32_t local_inactive_blocks = 4;

                if ( stack1->size() < local_inactive_blocks )
                {
                    stack1->push(block);
                }
                else
                {
                    global_free_page_block(block, stack2);
                }
            }
            else
            {
                local_heap->remove(block);
                local_heap->push_front(block);
            }
        }

        void heap::free(void* pointer) throw()
        {
            if (!m_super_page_manager.is_large_object(pointer) )
            {            
                page_block* block = m_super_page_manager.decode_pointer(pointer);
                thread_id   tid = block->get_owning_thread_id();
            
                thread_local_info* local_heap_info = t_thread_local_heap_info->get_thread_local_info( get_index() );
                size_class         c = compute_size_class(block->get_size_class());
                thread_local_heap* local_heap = &local_heap_info->t_local_heaps[c];

                if ( tid == t_thread_id )
                {
            	    const uint32_t page_block_size	= compute_page_block_size( c );
			        const uint32_t page_block_class	= compute_page_block_size_class( page_block_size );

                    local_free(pointer, block, local_heap, &local_heap_info->t_local_inactive_page_blocks[page_block_class], &m_page_blocks_free[page_block_class]);
                }
                else if ( tid == thread_id_orphan )
                {
                    adopt_page_block( pointer, block, local_heap, tid);

                } else
                {
                    remote_free( pointer, block, local_heap, t_thread_id);
                }
            }
            else
            {
                return m_super_page_manager.free_large_block(pointer);
            }
        }


		static void thread_initialize()
		{
            //allocate data for 8 heaps
            const size_t size = sizeof(thread_local_heap_info);
			t_thread_local_heap_info_memory = ::VirtualAlloc( 0, size , MEM_COMMIT | MEM_RESERVE , PAGE_READWRITE);

            t_thread_local_heap_info = 0;

			if (t_thread_local_heap_info_memory)
			{
                t_thread_local_heap_info = new ( t_thread_local_heap_info_memory ) thread_local_heap_info();
			}
			else
			{
				//?????
			}

			t_thread_id = create_thread_id();
		}

		static void thread_finalize( heap** heaps, uint32_t heap_count )
		{
            for (uint32_t i = 0 ; i < heap_count; ++i)
            {
                heap* h = heaps[i];
                thread_local_info* local_heap_info = t_thread_local_heap_info->get_thread_local_info( h->get_index() );
                
                for (uint32_t i = 0; i < size_classes;++i)
                {
                    thread_local_heap* local_heap = &local_heap_info->t_local_heaps[ i ] ;

                    if ( !local_heap->empty() )
                    {
                        do
                        {
                            page_block* block = local_heap->front();
                            
                            local_heap->remove(block);

                            if (block->empty())
                            {
                                //insert into global free
                    			uint32_t page_block_class	= compute_page_block_size_class( static_cast<uint32_t> (block->get_memory_size()) );
                                h->free_page_block( block, page_block_class);
                            }
                            else
                            {
                                
                                uint64_t reference = 0;
			                    uint64_t new_reference = 0;

                                //reference holds in one 64 bit variable, counter, next pointer and thread id
				                reference = block->get_block_info();

                                //fetch the old head and version
                                remote_free_queue queue = remote_page_block_info::get_free_queue(reference);
					            uint16_t count = remote_page_block_info::get_count( queue );

                                if (count > 0 || !block->empty() )
                                {
                                    //insert into orphaned block
                                    h->push_orphaned_block(block, i);
                                }
                                else
                                {
                                    //try to set this as orphaned block

                                    //create new reference and try to set it
                                    new_reference = remote_page_block_info::set_thread_next_count( thread_id_orphan, 0, 0 );

                                    if ( !block->try_set_block_info_strong(reference, new_reference) )
                                    {
                                        //insert into orphaned block, somebody else did a remote free
                                        h->push_orphaned_block(block, i);
                                    }
                                    else
                                    {
                                        //do nothing, the first thread that frees will adopt the block
                                    }
                                }
                            }
                        }  while ( !local_heap->empty() );
                    } 

                    h->free_page_blocks();
                }

            }
            
            t_thread_local_heap_info->~thread_local_heap_info();
			::VirtualFree(t_thread_local_heap_info_memory, 0, MEM_RELEASE);

		}


    	void test_streamflow()
		{
            void* heap_memory = ::VirtualAlloc( 0, sizeof(heap) , MEM_COMMIT | MEM_RESERVE , PAGE_READWRITE);
            void* heap_memory1 = ::VirtualAlloc( 0, 32768 * 8 , MEM_COMMIT | MEM_RESERVE , PAGE_READWRITE);

            if (heap_memory)
            {
                heap* heap = new (heap_memory) ::mem::streamflow::heap(0);
                thread_initialize( );
                
                for (uint32_t i = 0 ; i < 32768;++i)
                {
                     
                    void* v = heap->allocate(256);

                    void** v1 = reinterpret_cast<void**> ( reinterpret_cast<uintptr_t> (heap_memory1) + i * 8 );

                    *v1 = v;
                }

                for (uint32_t i = 0 ; i < 32768;++i)
                {
                    void** v = reinterpret_cast<void**> ( reinterpret_cast<uintptr_t> (heap_memory1) + i * 8 );
                    heap->free(*v);
                }

                void* r1 = heap->allocate(2049);
                void* r2 = heap->allocate(256);
                void* r3 = heap->allocate(345);
                void* r4 = heap->allocate(168);

                heap->free(r1);
                heap->free(r4);
                heap->free(r3);
                heap->free(r2);
                heap->free(r1);

                thread_finalize(&heap, 1);
                heap->~heap();

                ::VirtualFree(heap_memory, 0, MEM_RELEASE);
            }
		}
    }
}
