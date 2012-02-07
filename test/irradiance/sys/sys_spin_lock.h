#ifndef __SYS_SPIN_LOCK_H__
#define __SYS_SPIN_LOCK_H__

#include <cstdint>

namespace sys
{
    namespace details
    {
        //perform exponential backoff
        inline uint32_t delay_eb(uint32_t value) throw()
        {
            volatile uint32_t i;

            for (i = 0; i < value; ++i)
            {
                ;
            }

            return value * value;
        }

        //perform exponential backoff
        inline uint32_t delay_gm(uint32_t value) throw()
        {
            volatile uint32_t i;

            for (i = 0; i < value; ++i)
            {
                ;
            }
            return value << 1;
        }
    }

    //use for low contention
    //paper: The Performance of Spin Lock Alternatives for Shaded - Memory Multiprocessors
    //implemented also in cilk
    class alignas(64) spinlock_fas
    {
        public:
        spinlock_fas() : m_lock(free)
        {

        }

        void lock()
        {
            while ( m_lock == busy || _InterlockedExchange(&m_lock, busy) == busy)
            {
                _mm_pause();
            }
        }

        void lock_eb()
        {
            uint32_t delay_value = 2;
            while ( m_lock == busy || _InterlockedExchange(&m_lock, busy) == busy)
            {
                delay_value = details::delay_eb(delay_value);
            }
        }

        void lock_gm()
        {
            uint32_t delay_value = 2;
            while ( m_lock == busy || _InterlockedExchange(&m_lock, busy) == busy)
            {
                delay_value = details::delay_gm(delay_value);
            }
        }

        //does not work on amd sse3
        void lock_wait()
        {
            while (_InterlockedExchange(&m_lock, busy) == busy )
            {
                volatile long* address = &m_lock;
                _mm_mwait( (unsigned int) address, (unsigned int) ( address + 1) );
            }
        }

        void unlock()
        {
            m_lock = free;
        }

        private:
        enum
        {
            free = 0,
            busy = 1
        };

        volatile long     m_lock;
        uint8_t           m_pad[64 - sizeof(uint32_t)];

        spinlock_fas(const spinlock_fas&);
        const spinlock_fas& operator=(const spinlock_fas&);
    };

    //use for medium to high contention. has increased latency. takes a lot of space
    //paper: The Performance of Spin Lock Alternatives for Shared - Memory Multiprocessors
    class alignas(64) spinlock_anderson
    {
        //fix contending processor to 16.
        static const uint32_t slot_count = 16;

        public:

        spinlock_anderson() : m_queue_last(0)
        {
            m_slots[0].m_flag = has_lock;

            slot slot = {must_wait};
            std::fill( &m_slots[1], &m_slots[1] + slot_count - 1, slot );
        }

        void lock( uint32_t* position )
        {
            uint32_t pos = _InterlockedIncrement( (volatile long*) &m_queue_last) - 1;

            while ( m_slots[ pos % slot_count].m_flag == must_wait)
            {
                _mm_pause();
            }

            m_slots[ pos % slot_count ].m_flag = must_wait;
            *position = pos;
        }

        void unlock(uint32_t position)
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
            uint8_t  m_pad  [ 64 - sizeof(uint32_t) ];
        } slot;

        slot        m_slots[slot_count];

        uint32_t    m_queue_last;
        uint8_t     m_pad1[ 64 - sizeof(uint32_t) ];
        

        spinlock_anderson(const spinlock_anderson&);
        const spinlock_anderson& operator=(const spinlock_anderson&);

    };
    
    class spinlock_mcs
    {

    };

    class spinlock_clh
    {

    };

    template <typename t> class lock
    {
        public:
        lock(t& l) : m_l(l)
        {
            l.lock();
        }

        ~lock()
        {
            l.unlock();
        }

        private:
        lock(const lock&);
        const lock& operator = (const lock&);
    };

    template <> class lock<spinlock_anderson>
    {
        public:
        lock(spinlock_anderson& l) : m_l(l)
        {
            l.lock(&m_position);
        }

        ~lock()
        {
            m_l.unlock(m_position);
        }

        private:
        spinlock_anderson& m_l;
        uint32_t m_position;
        lock(const lock&);
        const lock& operator = (const lock&);


    };
}


#endif