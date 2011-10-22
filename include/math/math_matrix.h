#if !defined(__MATH_MATRIX__H__)
#define __MATH_MATRIX__H__

#include <intrin.h>

#include <cstdint>

#include "math_vector.h"

namespace vts
{
	namespace math
	{
		struct __declspec( align(16) ) matrix_float44
		{
			union
			{
				vector_float4 r[4];
				float m[4][4];
			};
		};


		inline matrix_float44 matrix44_identity()
		{
			matrix_float44 m;

			m.r[0] = set(1.0f,0.0f, 0.0f, 0.0f);
			m.r[1] = set(0.0f,1.0f, 0.0f, 0.0f);
			m.r[2] = set(0.0f,0.0f, 1.0f, 0.0f);
			m.r[3] = set(0.0f,0.0f, 0.0f, 1.0f);

			return m;
		}

		inline matrix_float44 matrix44_zero()
		{
			matrix_float44 m;

			m.r[0] = zero();
			m.r[1] = zero();
			m.r[2] = zero();
			m.r[3] = zero();

			return m;
		}

		inline matrix_float44 matrix44_mul(matrix_float44 m1, matrix_float44 m2)
		{
			return m1;
		}

		inline matrix_float44 matrix44_add(matrix_float44 m1, matrix_float44 m2)
		{
			matrix_float44 m;

			m.r[0] = add(m1.r[0], m2.r[0]);
			m.r[1] = add(m1.r[1], m2.r[1]);
			m.r[2] = add(m1.r[2], m2.r[2]);
			m.r[3] = add(m1.r[3], m2.r[3]);

			return m;

		}


		inline matrix_float44 matrix44_sub(matrix_float44 m1, matrix_float44 m2)
		{
			matrix_float44 m;

			m.r[0] = sub(m1.r[0], m2.r[0]);
			m.r[1] = sub(m1.r[1], m2.r[1]);
			m.r[2] = sub(m1.r[2], m2.r[2]);
			m.r[3] = sub(m1.r[3], m2.r[3]);

			return m;
		}

		inline matrix_float44 matrix44_mad(matrix_float44 m1, matrix_float44 m2, matrix_float44 m3)
		{

		}

		inline matrix_float44 matrix44_translation(vector_float4 v)
		{
			matrix_float44 m;
		
			m.r[0] = set(1.0f,0.0f, 0.0f, 0.0f);
			m.r[1] = set(0.0f,1.0f, 0.0f, 0.0f);
			m.r[2] = set(0.0f,0.0f, 1.0f, 0.0f);
			m.r[3] = v;

			return m;
		}

		inline matrix_float44 matrix44_translation(float x, float y, float z, float w)
		{
			vector_float4 v = set(x,y,z,w);
			return matrix44_translation(v);
		}


		inline matrix_float44 matrix44_scaling(vector_float4 v)
		{

		}

		inline matrix_float44 matrix44_scaling(float x, float y, float z, float w)
		{

		}

		inline matrix_float44 matrix44_rotation_x(float angle)
		{

		}

		inline matrix_float44 matrix44_rotation_y(float angle)
		{

		}

		inline matrix_float44 matrix44_rotation_z(float angle)
		{

		}

	}

}


#endif