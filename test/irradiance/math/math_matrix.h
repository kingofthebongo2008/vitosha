#ifndef __MATH_MATRIX__H__
#define __MATH_MATRIX__H__

#include <math/math_vector.h>

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

	inline matrix_float44 identity_matrix()
	{
		matrix_float44 m;

		static const vector_float4 identity_r0  = {1.0f, 0.0f, 0.0f, 0.0f};
		static const vector_float4 identity_r1  = {0.0f, 1.0f, 0.0f, 0.0f};
		static const vector_float4 identity_r2  = {0.0f, 0.0f, 1.0f, 0.0f};
		static const vector_float4 identity_r3  = {0.0f, 0.0f, 0.0f, 1.0f};

		m.r[0] = identity_r0;
		m.r[1] = identity_r1;
		m.r[2] = identity_r2;
		m.r[3] = identity_r3;

		return m;
	}

	inline matrix_float44 zero_matrix()
	{
		matrix_float44 m;

		m.r[0] = zero();
		m.r[1] = zero();
		m.r[2] = zero();
		m.r[3] = zero();

		return m;
	}

	inline matrix_float44 load44(const float* __restrict address)
	{
		matrix_float44 m;

		m.r[0] = load4(address);
		m.r[1] = load4(address+4);
		m.r[2] = load4(address+8);
		m.r[3] = load4(address+12);

		return m;
	}

	inline matrix_float44 load43(const float* __restrict address)
	{
		matrix_float44 m;

		static const vector_float4 identity_r3  = {0.0f, 0.0f, 0.0f, 1.0f};

		m.r[0] = load4(address);
		m.r[1] = load4(address+4);
		m.r[2] = load4(address+8);
		m.r[3] = identity_r3;

		return m;
	}

	inline void store44(float* __restrict address, matrix_float44 m)
	{
		store4(address, m.r[0]);
		store4(address+4, m.r[1]);
		store4(address+8, m.r[2]);
		store4(address+12, m.r[3]);
	}

	inline void store43(float* __restrict address, matrix_float44 m)
	{
		store4(address, m.r[0]);
		store4(address+4, m.r[1]);
		store4(address+8, m.r[2]);
	}

	inline matrix_float44 transpose(matrix_float44 m)
	{
		matrix_float44 m1;

		vector_float4 v1 = shuffle<x,y,x,y>( m.r[0], m.r[1] );
		vector_float4 v2 = shuffle<x,y,x,y>( m.r[2], m.r[3] );
		vector_float4 v3 = shuffle<z,w,z,w>( m.r[0], m.r[1] );
		vector_float4 v4 = shuffle<z,w,z,w>( m.r[2], m.r[3] );

		m1.r[0] = shuffle<x,z,x,z>(v1, v2);
		m1.r[1] = shuffle<y,w,y,w>(v1, v2);
		m1.r[2] = shuffle<x,z,x,z>(v3, v4);
		m1.r[3] = shuffle<y,w,y,w>(v3, v4);

		return m1;
	}

	inline matrix_float44 inverse(matrix_float44 m)
	{
		vector_float4 tmp;
		vector_float4 det;
		vector_float4 minor0; 
		vector_float4 minor1; 
		vector_float4 minor2; 
		vector_float4 minor3;
		vector_float4 row0;
		vector_float4 row1;
		vector_float4 row2;
		vector_float4 row3;

		//------ Transposition
		matrix_float44 m1 = transpose(m);

		row0 = m1.r[0];
		row1 = swizzle<z,w,x,y>(m1.r[1]);
		row2 = m1.r[2];
		row3 = swizzle<z,w,x,y>(m1.r[3]);

		//------ 
		tmp = mul( row2, row3);
		tmp = swizzle<y,x,w,z>(tmp); //b1

		minor0 = mul(row1, tmp);
		minor1 = mul(row0, tmp);

		tmp = swizzle<z,w,x,y>(tmp); //4e

		minor0 = sub(mul(row1, tmp), minor0);
		minor1 = sub(mul(row0, tmp), minor1);

		minor1 = swizzle<z,w,x,y>(minor1); //4e
			
		//------ 
		tmp = mul(row1, row2);
		tmp = swizzle<y,x,w,z>(tmp);

		minor0 = add( mul(row3, tmp), minor0 );
		minor3 = mul(row0, tmp);

		tmp = swizzle<z,w,x,y>(tmp);

		minor0 = sub(minor0, mul(row3, tmp));
		minor3 = sub(mul(row0,tmp), minor3);
		minor3 = swizzle<z,w,x,y>(minor3);

		//------ 
		tmp = mul( swizzle<z,w,x,y>(row1), row3);
		tmp = swizzle<y,x,w,z>(tmp);
		row2 = swizzle<z,w,x,y>(row2);

		minor0 = add(mul(row2,tmp), minor0);
		minor2 = mul(row0, tmp);

		tmp = swizzle<z,w,x,y>(tmp);

		minor0 = sub(minor0, mul(row2, tmp));
		minor2 = sub(mul(row0, tmp), minor2);
		minor2 = swizzle<z,w,x,y>(minor2);

		//------ 

		tmp = mul(row0, row1);
		tmp = swizzle<y,x,w,z>(tmp);

		minor2 = add(mul(row3, tmp), minor2);
		minor3 = sub(mul(row2, tmp), minor3);

		tmp = swizzle<z,w,x,y>(tmp);
		minor2 = sub( mul(row3, tmp), minor2);
		minor3 = sub(minor3, mul(row2, tmp));

		//------ 

		tmp = mul(row0, row3);
		tmp = swizzle<y,x,w,z>(tmp);

		minor1 = sub(minor1, mul(row2, tmp));
		minor2 = add(mul(row1, tmp), minor2 );

		tmp = swizzle<z,w,x,y>(tmp);
		minor1 = add( mul(row2, tmp), minor1 );
		minor2 = sub(minor2, mul(row1, tmp) );

		//------ 
		tmp = mul(row0, row2);
		tmp = swizzle<y,x,w,z>(tmp);

		minor1 = add( mul(row3, tmp), minor1);
		minor3 = sub( minor3, mul(row1, tmp));

		tmp = swizzle<z,w,x,y>(tmp);

		minor1 = sub(minor1, mul(row3, tmp));
		minor3 = add( mul(row1, tmp), minor3);
		//------ 

		det = mul(row0, minor0);
		det = add( swizzle<z,w,x,y>(det), det );
		det = _mm_add_ss(swizzle<y,x,z,w>(det),det);
		tmp = _mm_rcp_ss(det);

		det = _mm_sub_ss(_mm_add_ss(tmp, tmp), _mm_mul_ss(det, _mm_mul_ss(tmp, tmp)));
		det = swizzle<x,x,x,x>(det);

		matrix_float44 m2;
			
		m2.r[0] = mul(det, minor0);
		m2.r[1] = mul(det, minor1);
		m2.r[2] = mul(det, minor2);
		m2.r[3] = mul(det, minor3);

		return m2;

	}

	inline vector_float4 mul(vector_float4 v, matrix_float44 m)
	{
		vector_float4  v1 = swizzle<x,x,x,x>(v);
		vector_float4  v2 = swizzle<y,y,y,y>(v);
		vector_float4  v3 = swizzle<z,z,z,z>(v);
		vector_float4  v4 = swizzle<w,w,w,w>(v);

		vector_float4  v5 = mul(v1, m.r[0]);
		vector_float4  v6 = mul(v2, m.r[1]);
		vector_float4  v7 = mul(v3, m.r[2]);
		vector_float4  v8 = mul(v4, m.r[3]);

		vector_float4  v9 = add(v5, v6);
		vector_float4  v10 = add(v7, v8);
		vector_float4  v11 = add(v9, v10);

		return v11;
	}

	inline vector_float4 mul(matrix_float44 m, vector_float4 v)
	{
		matrix_float44 trans = transpose(m);
		return mul(v, trans);
	}

	inline matrix_float44 mul(matrix_float44 m1, matrix_float44 m2)
	{
		matrix_float44 m;

		m.r[0] = mul(m1.r[0], m2);
		m.r[1] = mul(m1.r[1], m2);
		m.r[2] = mul(m1.r[2], m2);
		m.r[3] = mul(m1.r[3], m2);

		return m;
	}

	inline matrix_float44 add(matrix_float44 m1, matrix_float44 m2)
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
		matrix_float44 m4 = mul(m1, m2);
		matrix_float44 m5 = add(m4, m3);
		return m5;
	}
}


#endif