#ifndef __GX_COLOR_H__
#define __GX_COLOR_H__

#include <cstdint>

#include <math/math_vector.h>

namespace gx
{
	namespace color
	{
		inline math::float4 red()
		{
			return math::set(1.0f, 0.0f, 0.0f, 1.0f);
		}

		inline math::float4 green()
		{
			return math::set(0.0f, 1.0f, 0.0f, 1.0f);
		}

		inline math::float4 blue()
		{
			return math::set(0.0f, 0.0f, 1.0f, 1.0f);
		}

        inline math::float4 black()
		{
			return math::set(0.0f, 0.0f, 0.0f, 1.0f);
		}

        inline math::float4 white()
		{
			return math::set(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}
}

#endif