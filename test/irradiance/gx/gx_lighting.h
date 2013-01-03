#ifndef __GX_LIGHTING_H__
#define __GX_LIGHTING_H__

#include <cstdint>
#include <math.h>
#include <math/math_matrix.h>

namespace gx
{
	inline float	encode_specular_power(float power)
	{
		power = std::max( 0.0f, std::min( power, 100000.f ) );
		float l =  ( logf(power) / logf(2.0f) - 0.01f) / 10.0f ;
		return l;
	}
}

#endif