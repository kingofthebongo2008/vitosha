#ifndef __UTILITY_BITS_H__
#define __UTILITY_BITS_H__

#include <cstdint>

namespace util
{
    template<uint32_t bit, typename t> inline bool bit_is_set( t value);
    template<uint32_t bit, typename t> inline t bit_reset(t value) ;
    template<uint32_t bit, typename t> inline t bit_set(t value) ;

    template<uint32_t bit, typename t> inline bool bit_is_set ( uint32_t value )
    {
        return (value & (1<<bit)) !=0; 
    }

    template<uint32_t bit, typename t> inline uint32_t bit_reset(uint32_t value) 
    {
        return value  & ( ~( 1<<bit) );
    }

    template<uint32_t bit, typename t> inline uint32_t bit_set(uint32_t value) 
    {
        return value |  ( 1<<bit) ;
    }

    template<uint32_t bit, typename t> inline bool bit_is_set( uint16_t value)
    {
        return (value & (1<<bit)) !=0; 
    }

    template<uint32_t bit, typename t> inline uint16_t bit_reset(uint16_t value) 
    {
        return value  & ( ~( 1<<bit) );
    }

    template<uint32_t bit, typename t> inline uint16_t bit_set(uint16_t value) 
    {
        return value |  ( 1<<bit) ;
    }

    template<uint32_t bit, typename t> inline bool bit_is_set( uint64_t value )
    {
        return (value & (1<<bit)) !=0; 
    }

    template<uint32_t bit, typename t> inline uint64_t bit_reset( uint64_t value ) 
    {
        return value  & ( ~( 1<<bit) );
    }

    template<uint32_t bit, typename t> inline uint64_t bit_set(uint64_t value) 
    {
        return value |  ( 1<<bit) ;
    }

    template<uint32_t bit, typename t> inline bool bit_is_set( uint8_t value )
    {
        return (value & (1<<bit)) !=0; 
    }

    template<uint32_t bit, typename t> inline uint8_t bit_reset( uint8_t value ) 
    {
        return value  & ( ~( 1<<bit) );
    }

    template<uint32_t bit, typename t> inline unsigned char bit_set(unsigned char value) 
    {
        return value |  ( 1<<bit) ;
    }

    template<uint32_t bit, typename t> inline t bit_set(t value, bool on)
    {
        if (on)
        {
            return bit_set<bit>();
        }
        else
        {
            return bit_reset<bit>();
        }
    }
}



#endif