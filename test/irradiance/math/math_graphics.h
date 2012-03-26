#ifndef __MATH_GRAPHICS_H__
#define __MATH_GRAPHICS_H__

#include <math.h>
#include <math/math_matrix.h>

namespace math
{
	struct view_port
	{
		float m_left;
		float m_top;
		float m_right;
		float m_bottom;
		float m_min_z;
		float m_max_z;
	};

	inline matrix_float44 translation(vector_float4 v)
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

	inline matrix_float44 translation(float x, float y, float z)
	{
		vector_float4 v = set(x, y, z , 1.0f);
		return translation(v);
	}


	inline matrix_float44 scaling(vector_float4 v)
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

	inline matrix_float44 scaling(float x, float y, float z)
	{
		vector_float4 v = set(x, y, z, 1.0f);
		return scaling(v);
	}

	inline matrix_float44 rotation_x(float sin_angle, float cos_angle)
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

	inline matrix_float44 rotation_y(float sin_angle, float cos_angle)
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

	inline matrix_float44 rotation_z(float sin_angle, float cos_angle)
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

	inline matrix_float44 rotation_x(float angle)
	{
		float sin_angle = sinf(angle);
		float cos_angle = cosf(angle);
		return rotation_x(sin_angle, cos_angle);
	}

	inline matrix_float44 rotation_y(float angle)
	{
		float sin_angle = sinf(angle);
		float cos_angle = cosf(angle);
		return rotation_y(sin_angle, cos_angle);
	}

	inline matrix_float44 rotation_z(float angle)
	{
		float sin_angle = sinf(angle);
		float cos_angle = cosf(angle);
		return rotation_z(sin_angle, cos_angle);
	}

	inline matrix_float44 view(vector_float4 eye_position, vector_float4 eye_direction, vector_float4 up_direction)
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

		m2 = transpose(m1);
		return m2;
	}

	inline matrix_float44 look_at_lh(vector_float4 eye_position, vector_float4 look_at_position, vector_float4 up_direction)
	{
		vector_float4 v1 = sub(look_at_position, eye_position);
		vector_float4 v2 = normalize3(v1);
		return view(eye_position, v2, up_direction);
	}

	inline matrix_float44 perspective_lh(float view_width, float view_height, float z_near, float z_far)
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

	inline matrix_float44 perspective_fov_lh(float fov, float aspect_ratio, float z_near, float z_far)
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

	inline matrix_float44 orthographic_lh(float view_width, float view_height, float z_near, float z_far)
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

	inline void extract_view_frustum(matrix_float44 wvp, float frustum[24])
	{
		vector_float4 v1;
		vector_float4 v2;
		vector_float4 v3;
		vector_float4 v4;
		vector_float4 v5;
		vector_float4 v6;

		matrix_float44 m1;

		m1 = transpose(wvp);

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

	//transforms a point with a transformation matrix (scale, rotation, translation) , assumes w = 1.0f
	inline vector_float4 transform3(vector_float4 v, matrix_float44 m)
	{
		// assume w component is 1 and multiply vector * matrix
		vector_float4  v1 = swizzle<x,x,x,x>(v);
		vector_float4  v2 = swizzle<y,y,y,y>(v);
		vector_float4  v3 = swizzle<z,z,z,z>(v);

		vector_float4  v5 = mul(v1, m.r[0]);
		vector_float4  v6 = mul(v2, m.r[1]);
		vector_float4  v7 = mad(v3, m.r[2], m.r[3]);

		vector_float4  v8 = add(v5, v6);
		vector_float4  v9 = add(v7, v8);

		return v9;
	}

	//does a perspective transform of a point, what the vertex shader + the gpu does
	inline vector_float4 perspective_transform3(vector_float4 v, matrix_float44 m)
	{
		// assume w component of the input is 1 and multiply vector * matrix
		vector_float4  v1 = swizzle<x,x,x,x>(v);
		vector_float4  v2 = swizzle<y,y,y,y>(v);
		vector_float4  v3 = swizzle<z,z,z,z>(v);
		vector_float4  v4 = swizzle<w,w,w,w>(v);

		vector_float4  v5 = mul(v1, m.r[0]);
		vector_float4  v6 = mul(v2, m.r[1]);
		vector_float4  v7 = mad(v3, m.r[2], m.r[3]);

		vector_float4  v8 = add(v5, v6);
		vector_float4  v9 = add(v7, v8);

		//do perspective divide
		vector_float4  v10 = rcp(v4);
		vector_float4  v11 = mul(v9, v10 );

		return v11;
	}

	//projects a point on the screen and applies the view port transform. the result is in window coordinates
	inline vector_float4 project( vector_float4 v, matrix_float44 wvp, view_port view_port)
	{
		vector_float4  v1	= perspective_transform3(v, wvp);

		float	half_height	= (view_port.m_bottom - view_port.m_top) * 0.5f;
		float	half_width	= (view_port.m_right - view_port.m_left) * 0.5f;

		vector_float4  scale	= set( half_width,  -half_height, view_port.m_max_z - view_port.m_min_z, 0.0f);
		vector_float4  offset	= set( view_port.m_left + half_width, view_port.m_top + half_height, view_port.m_min_z, 0.0f);

		return mad(v1, scale, offset );
	}
}



#endif