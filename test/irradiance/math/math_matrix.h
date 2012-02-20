#if !defined(__MATH_MATRIX__H__)
#define __MATH_MATRIX__H__

#include <intrin.h>

#include <cstdint>
#include <math.h>

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


	inline matrix_float44 matrix44_identity()
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

	inline matrix_float44 matrix44_zero()
	{
		matrix_float44 m;

		m.r[0] = zero();
		m.r[1] = zero();
		m.r[2] = zero();
		m.r[3] = zero();

		return m;
	}

	inline matrix_float44 matrix44_load44(const float* __restrict address)
	{
		matrix_float44 m;

		m.r[0] = load4(address);
		m.r[1] = load4(address+4);
		m.r[2] = load4(address+8);
		m.r[3] = load4(address+12);

		return m;
	}

	inline matrix_float44 matrix44_load43(const float* __restrict address)
	{
		matrix_float44 m;

		static const vector_float4 identity_r3  = {0.0f, 0.0f, 0.0f, 1.0f};

		m.r[0] = load4(address);
		m.r[1] = load4(address+4);
		m.r[2] = load4(address+8);
		m.r[3] = identity_r3;

		return m;
	}

	inline void matrix44_store44(float* __restrict address, matrix_float44 m)
	{
		store4(address, m.r[0]);
		store4(address+4, m.r[1]);
		store4(address+8, m.r[2]);
		store4(address+12, m.r[3]);
	}

	inline void matrix44_store43(float* __restrict address, matrix_float44 m)
	{
		store4(address, m.r[0]);
		store4(address+4, m.r[1]);
		store4(address+8, m.r[2]);
	}

	inline matrix_float44 matrix44_transpose(matrix_float44 m)
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

	inline matrix_float44 matrix44_inverse(matrix_float44 m)
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
		matrix_float44 m1 = matrix44_transpose(m);

		row0 = m1.r[0];
		row1 = m1.r[1];
		row2 = m1.r[2];
		row3 = m1.r[3];

		//------ 
		tmp = mul( row2, row3);
		tmp = swizzle<y,x,w,z>(tmp); //b1

		minor0 = mul(row1, tmp);
		minor1 = mul(row0, tmp);

		tmp = swizzle<z,w,x,y>(tmp);

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
		tmp = swizzle<y,x,z,w>(tmp);

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

	inline vector_float4 matrix44_mul(vector_float4 v, matrix_float44 m )
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

	inline matrix_float44 matrix44_mul(matrix_float44 m1, matrix_float44 m2)
	{
		matrix_float44 m;

		m.r[0] = matrix44_mul(m1.r[0], m2);
		m.r[1] = matrix44_mul(m1.r[1], m2);
		m.r[2] = matrix44_mul(m1.r[2], m2);
		m.r[3] = matrix44_mul(m1.r[3], m2);

		return m;
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
		matrix_float44 m4 = matrix44_mul(m1, m2);
		matrix_float44 m5 = matrix44_add(m4, m3);
		return m5;
	}

	inline matrix_float44 matrix44_translation(vector_float4 v)
	{
		matrix_float44 m;

		static const vector_float4 identity_r0  = {1.0f, 0.0f, 0.0f, 0.0f};
		static const vector_float4 identity_r1  = {0.0f, 1.0f, 0.0f, 0.0f};
		static const vector_float4 identity_r2  = {0.0f, 0.0f, 1.0f, 0.0f};
		
		m.r[0] = identity_r0;
		m.r[1] = identity_r1;
		m.r[2] = identity_r2;
		m.r[3] = v;

		return m;
	}

	inline matrix_float44 matrix44_translation(float x, float y, float z)
	{
		vector_float4 v = set(x, y, z , 1.0f);
		return matrix44_translation(v);
	}


	inline matrix_float44 matrix44_scaling(vector_float4 v)
	{
		matrix_float44 m;

		static const uint32_t mask_x[4] = { 0xFFFFFFFF, 0x0, 0, 0};
		static const uint32_t mask_y[4] = { 0, 0xFFFFFFFF, 0, 0};
		static const uint32_t mask_z[4] = { 0, 0, 0xFFFFFFFF, 0};

		static const vector_float4 identity_r3  = {0.0f, 0.0f, 0.0f, 1.0f};

		m.r[0] = and( v, reinterpret_cast< const vector_float4*> (&mask_x)[0] );
		m.r[1] = and( v, reinterpret_cast< const vector_float4*> (&mask_y)[0] );
		m.r[2] = and( v, reinterpret_cast< const vector_float4*> (&mask_z)[0] );
		m.r[3] = identity_r3;

		return m;
	}

	inline matrix_float44 matrix44_scaling(float x, float y, float z)
	{
		vector_float4 v = set(x, y, z, 1.0f);
		return matrix44_scaling(v);
	}

	inline matrix_float44 matrix44_rotation_x(float sin_angle, float cos_angle)
	{

		matrix_float44 m;

		static const vector_float4 negate_y = {1.0f, -1.0f,1.0f,1.0f};
		static const vector_float4 identity_r0  = {1.0f, 0.0f, 0.0f, 0.0f};
		static const vector_float4 identity_r3  = {0.0f, 0.0f, 0.0f, 1.0f};

		vector_float4 v1 = _mm_set_ss(cos_angle);
		vector_float4 v2 = _mm_set_ss(sin_angle);

		vector_float4 v3 = shuffle<y,x,x,y>(v1, v2);
		vector_float4 v4 = swizzle<x,z,y,w>(v3);
		vector_float4 v5 = negate_y;
		vector_float4 v6 = mul(v4, v5);

		m.r[0] = identity_r0;
		m.r[1] = v3;
		m.r[2] = v6;
		m.r[3] = identity_r3;

		return m;
	}

	inline matrix_float44 matrix44_rotation_y(float sin_angle, float cos_angle)
	{
		matrix_float44 m;

		static const vector_float4 negate_z = {1.0f, 1.0f,-1.0f,1.0f};
		static const vector_float4 identity_r1  = {0.0f, 1.0f, 0.0f, 0.0f};
		static const vector_float4 identity_r3  = {0.0f, 0.0f, 0.0f, 1.0f};

		vector_float4 v1 = _mm_set_ss(cos_angle);
		vector_float4 v2 = _mm_set_ss(sin_angle);

		vector_float4 v3 = shuffle<x,y,x,y>(v1, v2);
		vector_float4 v4 = swizzle<z,y,x,w>(v3);
		vector_float4 v5 = negate_z;
		vector_float4 v6 = mul(v4, v5);

		m.r[0] = v3;
		m.r[1] = identity_r1;
		m.r[2] = v6;
		m.r[3] = identity_r3;

		return m;
	}

	inline matrix_float44 matrix44_rotation_z(float sin_angle, float cos_angle)
	{
		matrix_float44 m;

		static const vector_float4 negate_x = {-1.0f, 1.0f,1.0f,1.0f};
		static const vector_float4 identity_r2  = {0.0f, 0.0f, 1.0f, 0.0f};
		static const vector_float4 identity_r3  = {0.0f, 0.0f, 0.0f, 1.0f};

		vector_float4 v1 = _mm_set_ss(cos_angle);
		vector_float4 v2 = _mm_set_ss(sin_angle);

		vector_float4 v3 = _mm_unpacklo_ps(v1, v2);
		vector_float4 v4 = swizzle<y,x,z,w>(v3);
		vector_float4 v5 = negate_x;
		vector_float4 v6 = mul(v4, v5);

		m.r[0] = v3;
		m.r[1] = v6;
		m.r[2] = identity_r2;
		m.r[3] = identity_r3;

		return m;
	}

	inline matrix_float44 matrix44_rotation_x(float angle)
	{
		float sin_angle = sinf(angle);
		float cos_angle = cosf(angle);
		return matrix44_rotation_x(sin_angle, cos_angle);
	}
		
	inline matrix_float44 matrix44_rotation_z(float angle)
	{
		float sin_angle = sinf(angle);
		float cos_angle = cosf(angle);
		return matrix44_rotation_z(sin_angle, cos_angle);
	}

	inline matrix_float44 matrix44_rotation_y(float angle)
	{
		float sin_angle = sinf(angle);
		float cos_angle = cosf(angle);
		return matrix44_rotation_y(sin_angle, cos_angle);
	}


	inline matrix_float44 matrix44_view(vector_float4 eye_position, vector_float4 eye_direction, vector_float4 up_direction)
	{
		//eye_direction and up_direction should be normalized;
		//eye_direction and up_direction w components should be zero

		static const uint32_t		mask_w[4] = { 0, 0, 0, 0xFFFFFFFF};
		static const uint32_t		mask_xyz[4] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0};
		static const vector_float4	identity_r3 = {0.0f, 0.0f, 0.0f, 1.0f};

		matrix_float44 m1;
		matrix_float44 m2;

		vector_float4 v0 = cross3(up_direction, eye_direction);
		vector_float4 v1 = cross3(eye_direction, v0);
		vector_float4 v2 = eye_direction;

		//form the translation
		vector_float4 negative_eye_position = negate(eye_position);

		vector_float4 d0 = dot3(v0, negative_eye_position);
		vector_float4 d1 = dot3(v1, negative_eye_position);
		vector_float4 d2 = dot3(v1, negative_eye_position);

		//clear all components except the w from the dot product
		d0 = and( d0, reinterpret_cast< const vector_float4*> (&mask_w)[0] );
		d1 = and( d1, reinterpret_cast< const vector_float4*> (&mask_w)[0] );
		d2 = and( d2, reinterpret_cast< const vector_float4*> (&mask_w)[0] );

		m1.r[0] = or( v0, d0 );
		m1.r[1] = or( v1, d1 ); 
		m1.r[2] = or( v2, d2 ); 
		m1.r[3] = identity_r3;

		m2 = matrix44_transpose(m1);
		return m2;
	}

	inline matrix_float44 matrix44_look_at_lh(vector_float4 eye_position, vector_float4 look_at_position, vector_float4 up_direction)
	{
		vector_float4 v1 = sub(look_at_position, eye_position);
		vector_float4 v2 = normalize3(v1);
		return matrix44_view(eye_position, v2, up_direction);
	}

	inline matrix_float44 matrix44_perspective_lh(float view_width, float view_height, float z_near, float z_far)
	{
		static const uint32_t		mask_x[4] = { 0xFFFFFFFF, 0, 0, 0};
		static const uint32_t		mask_yzw[4] = { 0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
		static const vector_float4	identity_r3 = {0.0f, 0.0f, 0.0f, 1.0f};

		float a = 2 * z_near;

		float r = z_far / (z_far - z_near);

		vector_float4 v1 = set( a / view_width, a / view_height, r, -r * z_near );
		vector_float4 ze = zero();

		matrix_float44 m;

		vector_float4 v2 = shuffle<x,y,x,y>(v1, ze);
		m.r[0] = and(v2, reinterpret_cast< const vector_float4*> (&mask_x)[0] );

		vector_float4 v3 = and(v1, reinterpret_cast< const vector_float4*> (&mask_yzw)[0] );
			
		m.r[1] = swizzle<x,y,x,x>(v1);

		vector_float4 v4 = shuffle<z,w,z,w>(v3, identity_r3);

		m.r[2] = swizzle<z,z,x,w>(v4);
		m.r[3] = swizzle<z,z,y,z>(v4);

		return m;
	}

	inline matrix_float44 matrix44_perspective_fov_lh(float fov, float aspect_ratio, float z_near, float z_far)
	{
		static const uint32_t		mask_x[4] = { 0xFFFFFFFF, 0, 0, 0};
		static const uint32_t		mask_yzw[4] = { 0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
		static const vector_float4	identity_r3 = {0.0f, 0.0f, 0.0f, 1.0f};

		float r = z_far / (z_far - z_near);

		float sin = sinf(fov);
		float cos = cosf(fov);
		float height = cos / sin;

		vector_float4 v1 = set( height / aspect_ratio, height, r, -r * z_near );
		vector_float4 ze = zero();

		matrix_float44 m;

		vector_float4 v2 = shuffle<x,y,x,y>(v1, ze);
		m.r[0] = and(v2, reinterpret_cast< const vector_float4*> (&mask_x)[0] );

		vector_float4 v3 = and(v1, reinterpret_cast< const vector_float4*> (&mask_yzw)[0] );

		m.r[1] = swizzle<x,y,x,x>(v1);

		vector_float4 v4 = shuffle<z,w,z,w>(v3, identity_r3);

		m.r[2] = swizzle<z,z,x,w>(v4);
		m.r[3] = swizzle<z,z,y,z>(v4);

		return m;
	}

	inline matrix_float44 matrix44_orthographic_lh(float view_width, float view_height, float z_near, float z_far)
	{
		static const uint32_t		mask_x[4] = { 0xFFFFFFFF, 0, 0, 0};
		static const uint32_t		mask_yzw[4] = { 0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
		static const vector_float4	identity_r3 = {0.0f, 0.0f, 0.0f, 1.0f};

		float a = 2.0f;

		float r = 1.0f / (z_far - z_near);

		vector_float4 v1 = set( a / view_width, a / view_height, r, -r * z_near );
		vector_float4 ze = zero();

		matrix_float44 m;

		vector_float4 v2 = shuffle<x,y,x,y>(v1, ze);
		m.r[0] = and(v2, reinterpret_cast< const vector_float4*> (&mask_x)[0] );

		vector_float4 v3 = and(v1, reinterpret_cast< const vector_float4*> (&mask_yzw)[0] );

		m.r[1] = swizzle<x,y,x,x>(v1);

		vector_float4 v4 = shuffle<z,w,z,w>(v3, identity_r3);

		m.r[2] = swizzle<z,z,x,w>(v4);
		m.r[3] = swizzle<z,z,y,z>(v4);

		return m;
	}

	inline void matrix44_extract_view_frustum(matrix_float44 wvp, float frustum[24])
	{
		vector_float4 v1;
		vector_float4 v2;
		vector_float4 v3;
		vector_float4 v4;
		vector_float4 v5;
		vector_float4 v6;

		matrix_float44 m1;

		m1 = matrix44_transpose(wvp);

		v1 = add(wvp.r[3], m1.r[0]);	//left
		v2 = sub(wvp.r[3], m1.r[0]);	//right
		v3 = sub(wvp.r[3], m1.r[1]);	//top
		v4 = add(wvp.r[3], m1.r[0]);	//bottom
				
		v5 = m1.r[2];				    //near
		v6 = sub(wvp.r[3],m1.r[2]);	    //far

		v1 = normalize_plane(v1);
		v2 = normalize_plane(v2);
		v3 = normalize_plane(v3);
		v4 = normalize_plane(v4);
		v5 = normalize_plane(v5);
		v6 = normalize_plane(v6);

		v1 = negate(v1);
		v2 = negate(v2);
		v3 = negate(v3);
		v4 = negate(v4);
		v5 = negate(v5);
		v6 = negate(v6);

		store4( &frustum[0], v1);       //left
		store4( &frustum[4], v2);       //right
		store4( &frustum[8], v3);       //top
		store4( &frustum[12], v4);      //bottom
		store4( &frustum[16], v5);      //near
		store4( &frustum[20], v6);      //far
	}

	//determinant

	//inverse 

}


#endif