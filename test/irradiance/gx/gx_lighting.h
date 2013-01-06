#ifndef __GX_LIGHTING_H__
#define __GX_LIGHTING_H__

#include <cstdint>
#include <math.h>

#include <dxgiformat.h>

#include <math/math_matrix.h>



namespace gx
{
	inline float	encode_specular_power(float power)
	{
		power = std::max( 0.0f, std::min( power, 100000.f ) );
		float l =  ( logf(power) / logf(2.0f) - 0.01f) / 10.0f ;
		return l;
	}


	namespace materials
	{
		inline math::float4 water( )
		{
			return math::set(0.02f, 0.02f, 0.02f, 20.0f);
		}

		inline math::float4 plastic( )
		{
			return math::set(0.05f, 0.05f, 0.05f, 20.0f);
		}

		inline math::float4 glass( )
		{
			return math::set(0.08f, 0.08f, 0.08f, 20.0f);
		}

		inline math::float4 diamond( )
		{
			return math::set(0.17f, 0.17f, 0.17f, 20.0f);
		}

		inline math::float4 copper( )
		{
			return math::set(0.95f, 0.64f, 0.54f, 20.0f);
		}

		inline math::float4 aluminuium( )
		{
			return math::set(0.91f, 0.92f, 0.92f, 20.0f);
		}

	}
}

#endif