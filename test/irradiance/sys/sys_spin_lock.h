#ifndef __SYS_SPIN_LOCK_H__
#define __SYS_SPIN_LOCK_H__

#include <cstdint>
#include <intrin.h>

namespace sys
{
    namespace details
    {
        //perform exponential backoff
        inline uint32_t delay_eb(uint32_t value) throw()
        {
            volatile uint32_t   i;
            const uint32_t      max = 0x00FFFFFF;

            for (i = 0; i < value; ++i)
            {
                ;
            }

            return max & (value * value);
        }

        //perform geometric backoff
        inline uint32_t delay_gm(uint32_t value) throw()
        {
            volatile uint32_t   i;
            const uint32_t      max = 0x00FFFFFF;

            for (i = 0; i < value; ++i)
            {
                ;
            }
            return max & (value << 1);
        }

        //stall
        inline void delay() throw()
        {
            _mm_pause();
        }

        //mapped compiler intrinsics to more standard names in scientific papers and c++ 11x

        inline uint32_t test_and_set(volatile uint32_t* address, uint32_t value)
        {
            return _InterlockedExchange( (volatile long*) address, value);
        }

        inline uint32_t fetch_and_add(volatile uint32_t* address, uint32_t value)
        {
            return _InterlockedExchangeAdd((volatile long*) address, value);
        }

        inline bool compare_and_swap(volatile uint64_t* address, uint64_t old_value, uint64_t new_value)
        {
            return ( _InterlockedCompareExchange64((volatile long long*) address, new_value, old_value) == (long long) old_value );
        }

        inline uint64_t fetch_and_store(volatile uint64_t* address, uint64_t value)
        {
            return _InterlockedExchange64( (volatile long long*)  address, value);
        }
    }

    //paper: The Performance of Spin Lock Alternatives for Shared - Memory Multiprocessors
    class alignas(64) spinlock_fas
    {
        public:
        spinlock_fas() : m_lock(free)
        {

        }

        void acquire()
        {
            acquire_eb();
        }

        void acquire_lock()
        {
            while ( m_lock == busy || details::test_and_set(&m_lock, busy) == busy)
            {
                details::delay();
            }
        }

        void acquire_eb()
        {
            uint32_t delay_value = 2;
            while ( m_lock == busy || details::test_and_set(&m_lock, busy) == busy)
            {
                delay_value = details::delay_eb(delay_value);
            }
        }

        void acquire_gm()
        {
            uint32_t delay_value = 2;
            while ( m_lock == busy || details::test_and_set(&m_lock, busy) == busy)
            {
                delay_value = details::delay_gm(delay_value);
            }
        }

        void release()
        {
            m_lock = free;
        }

        private:
        enum
        {
            free = 0,
            busy = 1
        };

        volatile uint32_t m_lock;
        uint8_t           m_pad[64 - sizeof(uint32_t)];

        spinlock_fas(const spinlock_fas&);
        const spinlock_fas& operator=(const spinlock_fas&);
    };

    //paper: The Performance of Spin Lock Alternatives for Shared - Memory Multiprocessors
    //performs poorly when the lock is shared by more threads than processors
    class alignas(64) spinlock_anderson
    {
        //fix contending processor to 16.
        static const uint32_t slot_count = 32;

        public:

        spinlock_anderson() : m_queue_last(0)
        {
            m_slots[0].m_flag = has_lock;

            slot slot = {must_wait};
            std::fill( &m_slots[1], &m_slots[1] + slot_count - 1, slot );
        }

        void acquire( uint32_t* position )
        {
            uint32_t pos = details::fetch_and_add(&m_queue_last, 1);

            while ( m_slots[ pos % slot_count].m_flag == must_wait)
            {
                details::delay();
            }

            m_slots[ pos % slot_count ].m_flag = must_wait;
            *position = pos;
        }

        void release(uint32_t position)
        {
            m_slots[ (position + 1 ) % slot_count ].m_flag = has_lock;
        }

        private:
        enum
        {
            has_lock = 0,
            must_wait = 1
        };

        typedef union alignas(64) 
        {
            uint32_t m_flag;
            uint8_t  m_pad  [ 64  ];
        } slot;

        slot        m_slots[slot_count];

        uint32_t    m_queue_last;
        uint8_t     m_pad1[ 64 - sizeof(uint32_t) ];
        

        spinlock_anderson(const spinlock_anderson&);
        const spinlock_anderson& operator=(const spinlock_anderson&);

    };

    //paper: Algorithms for Scalable Synchronization on Shared-Memory Multiprocessors
    //performs poorly when the lock is shared by more threads than processors
    class alignas(64) spinlock_mcs
    {
        public:
        struct alignas(64) qnode
        {
            qnode*      m_next;
            uint32_t    m_locked;

            uint8_t     m_pad[ 64 - sizeof(uint64_t) - sizeof(uint32_t) ];

            qnode() : m_next(nullptr), m_locked(0)
            {

            }
        };

        spinlock_mcs() : m_node(nullptr)
        {

        }

        void acquire(qnode* node)
        {
            //prepare the node for insertion
            node->m_next = nullptr;

            qnode* predecessor = reinterpret_cast<qnode*> ( details::fetch_and_store((volatile uint64_t*) &m_node, (uint64_t) node) );

            if (predecessor != nullptr)
            {
                node->m_locked = 1;
                predecessor->m_next = node;
                while (node->m_locked)
                {
                    details::delay();
                }
            }
        }

        void release(qnode* node)
        {
            if ( node->m_next == nullptr)
            {
                if ( details::compare_and_swap( (volatile uint64_t*) &m_node, (uint64_t) node, 0 ) )
                {
                    return;
                }

                //compensates for the timing window between fetch_and_store and the assignment
                //to predecessor->m_next
                while(node->m_next == nullptr)
                {
                    details::delay();
                }
            }

            node->m_next->m_locked = 0;
        }

        private:
        qnode*  m_node;
        uint8_t m_pad[64 - sizeof(uint64_t) ];
    };

    class spinlock_clh
    {

    };

    template <typename t> class lock
    {
        public:
        explicit lock(t& l) : m_l(l)
        {
            m_l.acquire();
        }

        ~lock()
        {
            m_l.release();
        }

        private:
        t& m_l;

        lock(const lock&);
        const lock& operator = (const lock&);
    };

    template <> class lock<spinlock_anderson>
    {
        public:
        explicit lock(spinlock_anderson& l) : m_l(l)
        {
            l.acquire(&m_position);
        }

        ~lock()
        {
            m_l.release(m_position);
        }

        private:
        spinlock_anderson& m_l;
        uint32_t m_position;
        lock(const lock&);
        const lock& operator = (const lock&);
    };

    template <> class lock<spinlock_mcs>
    {
        private:
        typedef lock<spinlock_mcs> this_type;

        public:
        explicit lock(spinlock_mcs& l) : m_l(l)
        {
            m_l.acquire(&m_node);   
        }

        ~lock()
        {
            m_l.release(&m_node);
        }

        private:
        spinlock_mcs::qnode m_node;
        spinlock_mcs& m_l;
        lock(const lock&);
        const lock& operator = (const lock&);
    };
}


#endif