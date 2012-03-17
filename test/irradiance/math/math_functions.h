#if !defined(__MATH_FUNCTIONS_H__)
#define __MATH_FUNCTIONS_H__

#include <intrin.h>
#include <cstdint>

#include <tuple>

#include <math/math_vector.h>

namespace math
{
	namespace details1
	{
		inline vector_float4 int_part(vector_float4 v)
		{
			static __declspec( align(16) ) const uint32_t sign_mask[4]		= { 0x80000000,	0x80000000, 0x80000000, 0x80000000 };

			vector_float4 mask = load4( reinterpret_cast<const float*> (&sign_mask[0]) );
			vector_float4 sign = and( v, mask);
			vector_float4 abs  = and_not(mask, v);

			//this number is 2<<23 as a floating point number.
			//23 is the bits in the mantissa
			vector_float4 two_shift_23 = splat( 8388608.0f );

			vector_float4 result = add(abs, two_shift_23);	//eliminate the mantissa in v
			result = sub(result, two_shift_23);				//reconstruct the number

			//1.6 returns 2, instead of 1
			vector_float4 le = compare_le(abs, result);
			vector_float4 result_1 = sub (result, one() );
			vector_float4 s = select(result, result_1 , le);

			//mirror the result for negatives
			return or( s, sign);
		}

		inline vector_float4 frac_part(vector_float4 v)
		{
			return sub(v, int_part(v) );
		}
	}

	namespace details2
	{
		inline vector_float4 int_part(vector_float4 v)
		{
		    // truncate value
			__m128i  trunc = _mm_cvttps_epi32( v );
			__m128  mask = _mm_castsi128_ps( _mm_cmpeq_epi32( trunc, _mm_set1_epi32( 0x80000000u ) ) );

			// convert back to float and mask out invalid values
			__m128  x = _mm_cvtepi32_ps( trunc );
			x = _mm_andnot_ps( mask, x );
			return _mm_add_ps( x, _mm_and_ps( mask, v ) );
		}

		inline vector_float4 frac_part(vector_float4 v)
		{
			return sub(v, int_part(v) );
		}
	}

	namespace details
	{
		inline vector_float4 int_part(vector_float4 v)
		{
			return details2::int_part(v);
		}

		inline vector_float4 frac_part(vector_float4 v)
		{
			return details2::frac_part(v);
		}
	}

	inline math::vector_float4 truncate(math::vector_float4 value)
	{
		return details::int_part(value);
	}

	inline math::vector_float4 floor(math::vector_float4 value)
	{
		math::vector_float4 int_part = details::int_part( value );
		math::vector_float4 frac_part = sub( value, int_part );
		math::vector_float4 int_part_minus_one = sub( int_part, one() );

		math::vector_float4 mask = compare_ge( frac_part, zero() );

		math::vector_float4 result = select( int_part_minus_one, int_part, mask );

		return result;
	}

	inline math::vector_float4 ceiling(math::vector_float4 value)
	{
		math::vector_float4 int_part = details::int_part( value );
		math::vector_float4 frac_part = sub( value, int_part );
		math::vector_float4 int_part_plus_one = add( int_part, one() );

		math::vector_float4 mask = compare_ge( frac_part, zero() );
		math::vector_float4 result = select( int_part, int_part_plus_one, mask );

		return result;
	}

	inline math::vector_float4 round(math::vector_float4 value)
	{
		math::vector_float4 one_half = splat(0.5f);
		math::vector_float4 minus_one_half = splat(-0.5f);

		math::vector_float4 mask = compare_lt( value, zero() );

		math::vector_float4 int_part_1 = details::int_part( add ( value, one_half) );
		math::vector_float4 int_part_2 = details::int_part( add ( value, minus_one_half) );

		return select ( int_part_1, int_part_2, mask );
	}
	
	namespace details
	{
		inline math::vector_float4 negative_multiply_subtract(math::vector_float4 v_1, math::vector_float4 v_2, math::vector_float4 v_3)
		{
			math::vector_float4 v = mul(v_1, v_2);
			return sub(v_3, v);
		}

		inline math::vector_float4 mod_angles(math::vector_float4 value)
		{
			math::vector_float4 v;
			math::vector_float4 result;

			math::vector_float4 reciprocal_two_pi   = {0.159154943f, 0.159154943f, 0.159154943f, 0.159154943f};
			math::vector_float4 two_pi				= {6.283185307f, 6.283185307f, 6.283185307f, 6.283185307f};

			// Modulo the range of the given angles such that -XM_PI <= Angles < XM_PI
			v = mul(value, reciprocal_two_pi);
			v = round(v);

			result = details::negative_multiply_subtract( two_pi, v, value);

			return result;
		}
	}

	math::vector_float4	sin_1(math::vector_float4 value);
	math::vector_float4	cos_1(math::vector_float4 value);

	inline std::tuple<math::vector_float4, math::vector_float4> sin_cos(math::vector_float4 value)
	{

		math::vector_float4 v = details::mod_angles( value );
		math::vector_float4 v_1 = sin_1(v);
		math::vector_float4 v_2 = cos_1(v);

		return std::make_tuple( v_1, v_2 );
	}
	
	inline std::tuple<math::vector_float4, math::vector_float4> sin_cos_1(math::vector_float4 value)
	{
		math::vector_float4 v_1 = sin_1(value);
		math::vector_float4 v_2 = cos_1(value);

		return std::make_tuple( v_1, v_2 );
	}

	inline math::vector_float4	sin(math::vector_float4 value)
	{
		math::vector_float4 v = details::mod_angles(value);
		return sin_1(v);
	}

	inline math::vector_float4	cos(math::vector_float4 value)
	{
		math::vector_float4 v = details::mod_angles(value);
		return cos_1(v);
	}

}



#endif