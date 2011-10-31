#if !defined(__MATH_VECTOR_H__)
#define __MATH_VECTOR_H__

#include <intrin.h>

#include <cstdint>

namespace vts
{
	namespace math
	{
		typedef __m128 vector_float4;
		

		enum component
		{
			x = 0,
			y = 1,
			z = 2,
			w = 3
		};
		


		//memory control and initialization

		inline vector_float4 zero()
		{
			return _mm_setzero_ps();
		}

		inline vector_float4 one()
		{
			return _mm_set_ps(1.0f, 1.0f, 1.0f, 1.0f);
		}

		inline vector_float4 minus_one()
		{
			return _mm_set_ps(-1.0f, -1.0f, -1.0f, -1.0f);
		}

		template <uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4> inline vector_float4 shuffle(vector_float4 value1, vector_float4 value2)
		{
			const uint32_t shuffle_k = _MM_SHUFFLE(v4, v3, v2, v1);
			return _mm_shuffle_ps( value1, value2, shuffle_k);
		}

		template <uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4> inline vector_float4 swizzle(vector_float4 value)
		{
			return shuffle<v1, v2, v3, v4>(value, value);
		}

		inline vector_float4 splat(float value)
		{
			return _mm_set1_ps(value);
		}
			
		inline vector_float4 splat_x(vector_float4 value)
		{
			return swizzle<x,x,x,x>(value);
		}

		inline vector_float4 splat_y(vector_float4 value)
		{
			return swizzle<y,y,y,y>(value);
		}

		inline vector_float4 splat_z(vector_float4 value)
		{
			return swizzle<z,z,z,z>(value);
		}

		inline vector_float4 splat_w(vector_float4 value)
		{
			return swizzle<w,w,w,w>(value);
		}

		inline vector_float4 set(float v1, float v2, float v3, float v4)
		{
			return _mm_set_ps(v4, v3, v2, v1);
		}

		inline vector_float4 set_uint32(uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4)
		{
			__m128i v = _mm_set_epi32( v4, v3, v2, v1);
			return reinterpret_cast< __m128*> (&v)[0];
		}

		inline vector_float4 load1(const float* const __restrict address)
		{
			return _mm_load_ss(address);
		}

		inline vector_float4 load2(const float* const address)
		{
			__m128i v = _mm_loadl_epi64( (const __m128i * ) address );
			return reinterpret_cast<__m128 *>(&v)[0];
		}

		inline vector_float4 load3(const float* __restrict const address)
		{
			vector_float4 v = set_uint32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0);
			return _mm_and_ps(v, _mm_load_ps(address) );
		}

		inline vector_float4 load4(const float* __restrict const address)
		{
			return _mm_load_ps(address);
		}

		inline void store1(float* __restrict address, vector_float4 value)
		{
			_mm_store_ss(address, value);	
		}

		inline void store2(float* __restrict address, vector_float4 value)
		{
			__m128i* __restrict destination = reinterpret_cast<__m128i* __restrict> (address);
			__m128i  v = reinterpret_cast<const __m128i* __restrict > (&value)[0];
			_mm_storel_epi64( destination, v);
		}

		inline void store3(float* __restrict address, vector_float4 value)
		{
			vector_float4 v1 = splat_z(value);
			store2(address, value);
			store1(address + 2, v1);
		}

		inline void store4(float* __restrict address, vector_float4 value)
		{
			_mm_store_ps(address, value);	
		}

		inline void stream(float* __restrict address, vector_float4 value)
		{
			_mm_stream_ps(address, value);	
		}

		//compare operations

		inline vector_float4 compare_eq(vector_float4 v1, vector_float4 v2)
		{
			return _mm_cmpeq_ps(v1, v2);
		}

		inline vector_float4 compare_lt(vector_float4 v1, vector_float4 v2)
		{
			return _mm_cmplt_ps(v1, v2);
		}

		inline vector_float4 compare_le(vector_float4 v1, vector_float4 v2)
		{
			return _mm_cmple_ps(v1, v2);
		}

		inline vector_float4 compare_gt(vector_float4 v1, vector_float4 v2)
		{
			return _mm_cmpgt_ps(v1, v2);
		}

		inline vector_float4 compare_ge(vector_float4 v1, vector_float4 v2)
		{
			return _mm_cmpge_ps(v1, v2);
		}

		inline vector_float4 compare_not_eq(vector_float4 v1, vector_float4 v2)
		{
			return _mm_cmpneq_ps(v1, v2);
		}

		inline vector_float4 compare_not_lt(vector_float4 v1, vector_float4 v2)
		{
			return _mm_cmpnlt_ps(v1, v2);
		}

		inline vector_float4 compare_not_le(vector_float4 v1, vector_float4 v2)
		{
			return _mm_cmpnle_ps(v1, v2);
		}

		inline vector_float4 compare_not_gt(vector_float4 v1, vector_float4 v2)
		{
			return _mm_cmpngt_ps(v1, v2);
		}

		inline vector_float4 compare_not_ge(vector_float4 v1, vector_float4 v2)
		{
			return _mm_cmpnge_ps(v1, v2);
		}

		inline vector_float4 select(vector_float4 value, vector_float4 value2, vector_float4 control)
		{
			vector_float4 v1 = _mm_andnot_ps(control, value);
			vector_float4 v2 = _mm_and_ps(value, control);
			return _mm_or_ps(v1, v2);
		}

		inline vector_float4 select_control(uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4)
		{
			vector_float4 v = set_uint32(v1, v2, v3, v4);
			vector_float4 z = zero();
			return compare_gt( v, z );
		}


		//simple math operations
		inline vector_float4 add(vector_float4 v1, vector_float4 v2)
		{
			return _mm_add_ps(v1, v2);
		}

		//simple math operations
		inline vector_float4 horizontal_add(vector_float4 v1, vector_float4 v2)
		{
			return _mm_hadd_ps(v1, v2);
		}

		inline vector_float4 sub(vector_float4 v1, vector_float4 v2)
		{
			return _mm_sub_ps(v1, v2);
		}

		//simple math operations
		inline vector_float4 horizontal_sub(vector_float4 v1, vector_float4 v2)
		{
			return _mm_hsub_ps(v1, v2);
		}

		inline vector_float4 mul(vector_float4 v1, vector_float4 v2)
		{
			return _mm_mul_ps(v1, v2);
		}

		inline vector_float4 mul(vector_float4 v, float f)
		{
			vector_float4 v1 = splat(f);
			return mul(v, v1);
		}

		inline vector_float4 mad(vector_float4 v1, vector_float4 v2, vector_float4 v3)
		{
			return add( mul(v1, v2) , v3 ) ;
		}

		inline vector_float4 div(vector_float4 v1, vector_float4 v2)
		{
			return _mm_div_ps(v1, v2);
		}

		inline vector_float4 sqrt(vector_float4 v)
		{
			return _mm_sqrt_ps(v);
		}

		inline vector_float4 rcp(vector_float4 v)
		{
			return _mm_rcp_ps(v);
		}

		inline vector_float4 rsqrt(vector_float4 v)
		{
			return _mm_rsqrt_ps(v);
		}

		inline vector_float4 min(vector_float4 v1, vector_float4 v2)
		{
			return _mm_min_ps(v1, v2);
		}

		inline vector_float4 max(vector_float4 v1, vector_float4 v2)
		{
			return _mm_max_ps(v1, v2);
		}

		inline vector_float4 clamp(vector_float4 v, vector_float4 a, vector_float4 b)
		{
			vector_float4 v1 = min(v,b);
			vector_float4 v2 = max(v1,a);
			return v2;
		}

		inline vector_float4 saturate(vector_float4 v)
		{
			return clamp(v, zero(), one() );
		}

		inline vector_float4 negate(vector_float4 v)
		{
			return mul( v, minus_one() );
		}

		inline vector_float4 abs(vector_float4 v)
		{
			vector_float4 v3 = sub( zero(), v );
			vector_float4 v4 = max( v, v3);
			return v4;
		}

		inline vector_float4 lerp(vector_float4 v1, vector_float4 v2, vector_float4 l)
		{
			vector_float4 a = sub (v2, v1);
			return mad( l, a , v2 );
		}

		inline vector_float4 lerp(vector_float4 v1, vector_float4 v2, float l)
		{
			vector_float4 s = splat(l);
			return lerp(v1, v2, s);
		}


		//simple logical operations
		inline vector_float4 and(vector_float4 v1, vector_float4 v2)
		{
			return _mm_and_ps(v1, v2);
		}

		inline vector_float4 and_not(vector_float4 v1, vector_float4 v2)
		{
			return _mm_andnot_ps(v1, v2);
		}

		inline vector_float4 or(vector_float4 v1, vector_float4 v2)
		{
			return _mm_or_ps(v1, v2);
		}

		inline vector_float4 xor(vector_float4 v1, vector_float4 v2)
		{
			return _mm_xor_ps(v1, v2);
		}

		//misc functions
		inline int32_t movemask(vector_float4 v)
		{
			return _mm_movemask_ps(v);
		}

		//math functions
		inline vector_float4 dot2(vector_float4 v1, vector_float4 v2)
		{
			vector_float4 v3 = mul(v1, v2);
			vector_float4 v4 = swizzle<x,x,x,x>(v3);
			vector_float4 v5 = swizzle<y,y,y,y>(v3);
			return add(v4, v5);
		}

		inline vector_float4 dot3(vector_float4 v1, vector_float4 v2)
		{
			vector_float4 v3 = mul(v1, v2);
			vector_float4 v4 = swizzle<x,x,x,x>(v3);
			vector_float4 v5 = swizzle<y,y,y,y>(v3);
			vector_float4 v6 = swizzle<z,z,z,z>(v3);
			vector_float4 v7 = add(v4, v5);
			return add(v6, v7);
		}

		inline vector_float4 dot4(vector_float4 v1, vector_float4 v2)
		{
			vector_float4 v3 = mul(v1, v2);
			vector_float4 v4 = horizontal_add(v3, v3);
			vector_float4 v5 = horizontal_add(v4, v4);
			return v5;
		}


		inline vector_float4 length2(vector_float4 v)
		{
			vector_float4 d = dot2(v, v);
			vector_float4 l = sqrt(d);
			return l;
		}

		inline vector_float4 length3(vector_float4 v)
		{
			vector_float4 d = dot3(v, v);
			vector_float4 l = sqrt(d);
			return l;
		}

		inline vector_float4 length4(vector_float4 v)
		{
			vector_float4 d = dot4(v, v);
			vector_float4 l = sqrt(d);
			return l;
		}

		inline vector_float4 normalize2(vector_float4 v)
		{
			vector_float4 l = length2(v);
			vector_float4 l_r = rsqrt(l);
			vector_float4 n = mul(v, l_r);
			return n;
		}

		inline vector_float4 normalize3(vector_float4 v)
		{
			vector_float4 l = length3(v);
			vector_float4 l_r = rsqrt(l);
			vector_float4 n = mul(v, l_r);
			return n;
		}

		inline vector_float4 normalize4(vector_float4 v)
		{
			vector_float4 l = length4(v);
			vector_float4 l_r = rsqrt(l);
			vector_float4 n = mul(v, l_r);
			return n;
		}

		inline vector_float4 normalize_plane(vector_float4 v)
		{
			vector_float4 l = length3(v);
			vector_float4 l_r = rsqrt(l);
			vector_float4 n = mul(v, l_r);
			return n;
		}

		inline vector_float4 cross2(vector_float4 v1, vector_float4 v2)
		{
			vector_float4 v3 = swizzle<x,y,x,y>(v2);
			vector_float4 v4 = mul(v1, v3);
			vector_float4 v5 = swizzle<y,y,y,y>(v4);
			vector_float4 v6 = sub(v4, v5);
			vector_float4 v7 = swizzle<x,x,x,x>(v6);
			return v7;
		}

		inline vector_float4 cross3(vector_float4 v1, vector_float4 v2)
		{
			vector_float4 v3 = swizzle<y,z,x,w>(v1);
			vector_float4 v4 = swizzle<z,x,y,w>(v2);
			vector_float4 v5 = mul(v3, v4);

			vector_float4 v6 = swizzle<z,x,y,w>(v1);
			vector_float4 v7 = swizzle<y,z,x,w>(v2);

			vector_float4 v8 = mul(v6,v7);
			vector_float4 v9 = sub( v5, v8);

			return v9;
		}

		inline vector_float4 ortho2(vector_float4 v)
		{
			vector_float4 v3 = swizzle<y,x,z,w>(v);
			vector_float4 v4 = negate(v3);
			return v4;
		}

		inline vector_float4 ortho4(vector_float4 v)
		{
			vector_float4 v3 = swizzle<y,x,w,z>(v);
			vector_float4 v4 = negate(v3);
			return v4;
		}
	}
}


#endif