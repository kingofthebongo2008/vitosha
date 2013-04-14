#ifndef __MATH_VECTOR_H__
#define __MATH_VECTOR_H__

#include <intrin.h>
#include <cstdint>

namespace math
{
    typedef __m128 float4;

    enum component
    {
        x = 0,
        y = 1,
        z = 2,
        w = 3
    };

    //memory control and initialization

    inline float4 zero()
    {
        return _mm_setzero_ps();
    }

    inline float4 one()
    {
        return _mm_set_ps(1.0f, 1.0f, 1.0f, 1.0f);
    }

    inline float4 minus_one()
    {
        return _mm_set_ps(-1.0f, -1.0f, -1.0f, -1.0f);
    }

    inline float4 identity_r0()
    {
        static const float4 r0  = {1.0f, 0.0f, 0.0f, 0.0f};
        return r0;
    }

    inline float4 identity_r1()
    {
        static const float4 r1  = {0.0f, 1.0f, 0.0f, 0.0f};
        return r1;
    }

    inline float4 identity_r2()
    {
        static const float4 r2  = {0.0f, 0.0f, 1.0f, 0.0f};
        return r2;
    }

    inline float4 identity_r3()
    {
        static const float4 r3  = {0.0f, 0.0f, 0.0f, 1.0f};
        return r3;
    }
    
    template <uint32_t shuffle_mask> inline float4 shuffle(float4 value1, float4 value2)
    {
        return _mm_shuffle_ps( value1, value2, shuffle_mask);
    }
    
    //until we get constexpr in visual studio 2012
    #define shuffle_mask(v1, v2, v3, v4) _MM_SHUFFLE(v4, v3, v2, v1)

    /*
    template <uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4> inline uint32_t shuffle_mask()
    {
        return _MM_SHUFFLE(v4, v3, v2, v1);
    }
    */

    template <uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4> inline float4 shuffle(float4 value1, float4 value2)
    {
        const uint32_t shuffle_k = _MM_SHUFFLE(v4, v3, v2, v1);
        return _mm_shuffle_ps( value1, value2, shuffle_k);
    }

    template <uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4> inline float4 swizzle(float4 value)
    {
        return shuffle<v1, v2, v3, v4>(value, value);
    }

    inline float4 merge_xy(float4 v1, float4 v2)
    {
        return _mm_unpacklo_ps( v1, v2 );		
    }

    inline float4 merge_zw(float4 v1, float4 v2)
    {
        return _mm_unpackhi_ps( v1, v2 );		
    }
    
    inline float4 splat(float value)
    {
        return _mm_set1_ps(value);
    }
            
    inline float4 splat_x(float4 value)
    {
        return swizzle<x,x,x,x>(value);
    }

    inline float4 splat_y(float4 value)
    {
        return swizzle<y,y,y,y>(value);
    }

    inline float4 splat_z(float4 value)
    {
        return swizzle<z,z,z,z>(value);
    }

    inline float4 splat_w(float4 value)
    {
        return swizzle<w,w,w,w>(value);
    }

    inline float4 set(float v1, float v2, float v3, float v4)
    {
        return _mm_set_ps(v4, v3, v2, v1);
    }

    inline float4 set_uint32(uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4)
    {
        __m128i v = _mm_set_epi32( v4, v3, v2, v1);
        return reinterpret_cast< __m128*> (&v)[0];
    }

    inline float4 load1(const void* const __restrict address)
    {
        return _mm_load_ss( reinterpret_cast<const float* const __restrict> (address) );
    }

    inline float4 load2(const void* const address)
    {
        __m128i v = _mm_loadl_epi64( (const __m128i * ) address );
        return reinterpret_cast<__m128 *>(&v)[0];
    }

    inline float4 load3(const void* __restrict const address)
    {
        float4 v = set_uint32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0);
        return _mm_and_ps(v, _mm_load_ps( reinterpret_cast< const float* __restrict > (address)) );
    }

    inline float4 load3u(const void* __restrict const address)
    {
        float4 v = set_uint32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0);
        return _mm_and_ps(v, _mm_loadu_ps( reinterpret_cast<const float* __restrict> (address) ) );
    }

    inline float4 load4( const void* __restrict const address )
    {
        return _mm_load_ps(reinterpret_cast<const float* __restrict const> ( address) );
    }

    inline float4 load4( const float* __restrict const address )
    {
        return _mm_load_ps(reinterpret_cast<const float* __restrict const> ( address) );
    }

    inline __m128i load4i( const void* __restrict const address )
    {
        return _mm_load_si128(reinterpret_cast<const __m128i * __restrict const> ( address) );
    }

    inline float4 load4u(const void* __restrict const address)
    {
        return _mm_loadu_ps( reinterpret_cast<const float* __restrict> (address) );
    }

    inline float4 load4u(const float* __restrict const address)
    {
        return _mm_loadu_ps( reinterpret_cast<const float* __restrict> (address) );
    }

    inline void store1(void* __restrict address, float4 value)
    {
        _mm_store_ss(reinterpret_cast<float*__restrict> (address), value);	
    }

    inline void store1(float* __restrict address, float4 value)
    {
        _mm_store_ss(reinterpret_cast<float*__restrict> (address), value);	
    }

    inline void store2(void* __restrict address, float4 value)
    {
        __m128i* __restrict destination = reinterpret_cast<__m128i* __restrict> (address);
        __m128i  v = reinterpret_cast<const __m128i* __restrict > (&value)[0];
        _mm_storel_epi64( destination, v);
    }

    inline void store2(float* __restrict address, float4 value)
    {
        __m128i* __restrict destination = reinterpret_cast<__m128i* __restrict> (address);
        __m128i  v = reinterpret_cast<const __m128i* __restrict > (&value)[0];
        _mm_storel_epi64( destination, v);
    }

    inline void store3(void* __restrict address, float4 value)
    {
        float4 v1 = splat_z(value);
        store2( address, value);
        store1( reinterpret_cast<float* __restrict> (address) + 2, v1);
    }

    inline void store3(float* __restrict address, float4 value)
    {
        float4 v1 = splat_z(value);
        store2( address, value);
        store1( reinterpret_cast<float* __restrict> (address) + 2, v1);
    }

    inline void store4(void* __restrict address, float4 value)
    {
        _mm_store_ps( reinterpret_cast<float* __restrict> (address), value);	
    }

    inline void store4(float* __restrict address, float4 value)
    {
        _mm_store_ps( reinterpret_cast<float* __restrict> (address), value);	
    }

    inline void stream(void* __restrict address, float4 value)
    {
        _mm_stream_ps(reinterpret_cast<float* __restrict> (address), value);	
    }

    inline void stream(float* __restrict address, float4 value)
    {
        _mm_stream_ps(reinterpret_cast<float* __restrict> (address), value);	
    }

    //compare operations

    inline float4 compare_eq(float4 v1, float4 v2)
    {
        return _mm_cmpeq_ps(v1, v2);
    }

    inline float4 compare_lt(float4 v1, float4 v2)
    {
        return _mm_cmplt_ps(v1, v2);
    }

    inline float4 compare_le(float4 v1, float4 v2)
    {
        return _mm_cmple_ps(v1, v2);
    }

    inline float4 compare_gt(float4 v1, float4 v2)
    {
        return _mm_cmpgt_ps(v1, v2);
    }

    inline float4 compare_ge(float4 v1, float4 v2)
    {
        return _mm_cmpge_ps(v1, v2);
    }

    inline float4 compare_not_eq(float4 v1, float4 v2)
    {
        return _mm_cmpneq_ps(v1, v2);
    }

    inline float4 compare_not_lt(float4 v1, float4 v2)
    {
        return _mm_cmpnlt_ps(v1, v2);
    }

    inline float4 compare_not_le(float4 v1, float4 v2)
    {
        return _mm_cmpnle_ps(v1, v2);
    }

    inline float4 compare_not_gt(float4 v1, float4 v2)
    {
        return _mm_cmpngt_ps(v1, v2);
    }

    inline float4 compare_not_ge(float4 v1, float4 v2)
    {
        return _mm_cmpnge_ps(v1, v2);
    }

    inline float4 select(float4 value1, float4 value2, float4 control)
    {
        float4 v1 = _mm_andnot_ps(control, value1);
        float4 v2 = _mm_and_ps(value2, control);
        return _mm_or_ps(v1, v2);
    }

    inline float4 select_control(uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4)
    {
        float4 v = set_uint32(v1, v2, v3, v4);
        float4 z = zero();
        return compare_gt( v, z );
    }


    //simple math operations
    inline float4 add(float4 v1, float4 v2)
    {
        return _mm_add_ps(v1, v2);
    }

    //simple math operations
    inline float4 horizontal_add(float4 v1, float4 v2)
    {
        return _mm_hadd_ps(v1, v2);
    }

    inline float4 sub(float4 v1, float4 v2)
    {
        return _mm_sub_ps(v1, v2);
    }

    //simple math operations
    inline float4 horizontal_sub(float4 v1, float4 v2)
    {
        return _mm_hsub_ps(v1, v2);
    }

    inline float4 mul(float4 v1, float4 v2)
    {
        return _mm_mul_ps(v1, v2);
    }

    inline float4 mul(float4 v, float f)
    {
        float4 v1 = splat(f);
        return mul(v, v1);
    }

    inline float4 mad(float4 v1, float4 v2, float4 v3)
    {
        return add( mul(v1, v2) , v3 ) ;
    }

    inline float4 div(float4 v1, float4 v2)
    {
        return _mm_div_ps(v1, v2);
    }

    inline float4 sqrt(float4 v)
    {
        return _mm_sqrt_ps(v);
    }

    inline float4 rcp(float4 v)
    {
        return _mm_rcp_ps(v);
    }

    inline float4 rsqrt(float4 v)
    {
        return _mm_rsqrt_ps(v);
    }

    inline float4 min(float4 v1, float4 v2)
    {
        return _mm_min_ps(v1, v2);
    }

    inline float4 max(float4 v1, float4 v2)
    {
        return _mm_max_ps(v1, v2);
    }

    inline float4 clamp(float4 v, float4 a, float4 b)
    {
        float4 v1 = min(v,b);
        float4 v2 = max(v1,a);
        return v2;
    }

    inline float4 saturate(float4 v)
    {
        return clamp(v, zero(), one() );
    }

    inline float4 negate(float4 v)
    {
        return mul( v, minus_one() );
    }

    inline float4 abs(float4 v)
    {
        float4 v3 = sub( zero(), v );
        float4 v4 = max( v, v3);
        return v4;
    }

    inline float4 lerp(float4 v1, float4 v2, float4 l)
    {
        float4 a = sub (v2, v1);
        return mad( l, a , v2 );
    }

    inline float4 lerp(float4 v1, float4 v2, float l)
    {
        float4 s = splat(l);
        return lerp(v1, v2, s);
    }


    //simple logical operations
    inline float4 and(float4 v1, float4 v2)
    {
        return _mm_and_ps(v1, v2);
    }

    inline float4 and_not(float4 v1, float4 v2)
    {
        return _mm_andnot_ps(v1, v2);
    }

    inline float4 or(float4 v1, float4 v2)
    {
        return _mm_or_ps(v1, v2);
    }

    inline float4 xor(float4 v1, float4 v2)
    {
        return _mm_xor_ps(v1, v2);
    }

    //misc functions
    inline int32_t movemask(float4 v)
    {
        return _mm_movemask_ps(v);
    }

    //math functions
    inline float4 dot2(float4 v1, float4 v2)
    {
        float4 v3 = mul(v1, v2);
        float4 v4 = swizzle<x,x,x,x>(v3);
        float4 v5 = swizzle<y,y,y,y>(v3);
        return add(v4, v5);
    }

    inline float4 dot3(float4 v1, float4 v2)
    {
        float4 v3 = mul(v1, v2);
        float4 v4 = swizzle<x,x,x,x>(v3);
        float4 v5 = swizzle<y,y,y,y>(v3);
        float4 v6 = swizzle<z,z,z,z>(v3);
        float4 v7 = add(v4, v5);
        return add(v6, v7);
    }

    inline float4 dot4(float4 v1, float4 v2)
    {
        float4 v3 = mul(v1, v2);
        float4 v4 = horizontal_add(v3, v3);
        float4 v5 = horizontal_add(v4, v4);
        return v5;
    }


    inline float4 length2(float4 v)
    {
        float4 d = dot2(v, v);
        float4 l = sqrt(d);
        return l;
    }

    inline float4 length3(float4 v)
    {
        float4 d = dot3(v, v);
        float4 l = sqrt(d);
        return l;
    }

    inline float4 length4(float4 v)
    {
        float4 d = dot4(v, v);
        float4 l = sqrt(d);
        return l;
    }

    inline float4 normalize2(float4 v)
    {
        float4 l = length2(v);
        float4 n = div(v, l);
        return n;
    }

    inline float4 normalize3(float4 v)
    {
        float4 l = length3(v);
        float4 n = div(v, l);
        return n;
    }

    inline float4 normalize4(float4 v)
    {
        float4 l = length4(v);
        float4 n = div(v, l);
        return n;
    }

    inline float4 normalize_plane(float4 v)
    {
        float4 l = length3(v);
        float4 n = div(v, l);
        return n;
    }

    inline float4 cross2(float4 v1, float4 v2)
    {
        float4 v3 = swizzle<x,y,x,y>(v2);
        float4 v4 = mul(v1, v3);
        float4 v5 = swizzle<y,y,y,y>(v4);
        float4 v6 = sub(v4, v5);
        float4 v7 = swizzle<x,x,x,x>(v6);
        return v7;
    }

    inline float4 cross3(float4 v1, float4 v2)
    {
        float4 v3 = swizzle<y,z,x,w>(v1);
        float4 v4 = swizzle<z,x,y,w>(v2);
        float4 v5 = mul(v3, v4);

        float4 v6 = swizzle<z,x,y,w>(v1);
        float4 v7 = swizzle<y,z,x,w>(v2);

        float4 v8 = mul(v6,v7);
        float4 v9 = sub( v5, v8);

        return v9;
    }

    inline float4 ortho2(float4 v)
    {
        float4 v3 = swizzle<y,x,z,w>(v);
        float4 v4 = negate(v3);
        return v4;
    }

    inline float4 ortho4(float4 v)
    {
        float4 v3 = swizzle<y,x,w,z>(v);
        float4 v4 = negate(v3);
        return v4;
    }

    namespace details
    {
        template <uint32_t c> inline float get_component(float4 v)
        {
            float4 v1 = swizzle<c,c,c,c>(v);
            float __declspec( align(16) ) f;
            store1(&f, v1);
            return f;
        }
    }

    inline float get_x(float4 v)
    {
        return details::get_component<x>(v);
    }

    inline float get_y(float4 v)
    {
        return details::get_component<y>(v);
    }

    inline float get_z(float4 v)
    {
        return details::get_component<z>(v);
    }

    inline float get_w(float4 v)
    {
        return details::get_component<w>(v);
    }


    inline float4 mask_x()
    {
        static const uint32_t __declspec( align(16) ) mask_x[4] = { 0xFFFFFFFF, 0x0, 0x0, 0x0};
        return load4(&mask_x[0] );
    }

    inline float4 mask_y()
    {
        static const uint32_t __declspec( align(16) ) mask_y[4] = { 0x0, 0xFFFFFFFF, 0x0, 0x0};
        return load4(&mask_y[0] );
    }

    inline float4 mask_z()
    {
        static const uint32_t __declspec( align(16) ) mask_z[4] = { 0x0, 0x0, 0xFFFFFFFF, 0x0};
        return load4(&mask_z[0] );
    }

    inline float4 mask_w()
    {
        static const uint32_t __declspec( align(16) ) mask_w[4] = { 0x0, 0x0, 0x0, 0xFFFFFFFF};
        return load4(&mask_w[0] );
    }

}

#endif