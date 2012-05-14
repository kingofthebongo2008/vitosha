#ifndef __MATH_GRAPHICS_H__
#define __MATH_GRAPHICS_H__

#include <tuple>

#include <math.h>
#include <math/math_matrix.h>
#include <math/math_quaternion.h>

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

	inline float4x4 translation(float4 v)
	{
		float4x4 m;

		static const float4 identity_r0  = {1.0f, 0.0f, 0.0f, 0.0f};
		static const float4 identity_r1  = {0.0f, 1.0f, 0.0f, 0.0f};
		static const float4 identity_r2  = {0.0f, 0.0f, 1.0f, 0.0f};
		
		m.r[0] = identity_r0;
		m.r[1] = identity_r1;
		m.r[2] = identity_r2;
		m.r[3] = v;

		return m;
	}

	inline float4x4 translation(float x, float y, float z)
	{
		float4 v = set(x, y, z , 1.0f);
		return translation(v);
	}

    inline float4x4 translation_x(float v)
	{
        return translation( v, 0.0f, 0.0f );
	}

    inline float4x4 translation_y(float v)
	{
        return translation( v, 0.0f, 0.0f );
	}

    inline float4x4 translation_z(float v)
	{
        return translation( v, 0.0f, 0.0f );
	}

    inline float4x4 translation_xy(float v)
	{
        return translation( v, v, 0.0f );
	}

    inline float4x4 translation_xz(float v)
	{
        return translation( v, 0.0f, v );
	}

    inline float4x4 translation_yz(float v)
	{
        return translation( 0.0f, v, v );
	}

	inline float4x4 scaling(float4 v)
	{
		float4x4 m;

		static const uint32_t mask_x[4] = { 0xFFFFFFFF, 0x0, 0, 0};
		static const uint32_t mask_y[4] = { 0, 0xFFFFFFFF, 0, 0};
		static const uint32_t mask_z[4] = { 0, 0, 0xFFFFFFFF, 0};

		static const float4 identity_r3  = {0.0f, 0.0f, 0.0f, 1.0f};

		m.r[0] = and( v, reinterpret_cast< const float4*> (&mask_x)[0] );
		m.r[1] = and( v, reinterpret_cast< const float4*> (&mask_y)[0] );
		m.r[2] = and( v, reinterpret_cast< const float4*> (&mask_z)[0] );
		m.r[3] = identity_r3;

		return m;
	}

    inline float4x4 scaling_x(float x)
	{
        return scaling( set ( x, 1.0f, 1.0f, 1.0f ) );
	}

    inline float4x4 scaling_y(float y)
	{
        return scaling( set ( 1.0f, y, 1.0f, 1.0f) );
	}

    inline float4x4 scaling_z(float z)
	{
        return scaling( set ( 1.0f, 1.0f, z, 1.0f) );
	}

    inline float4x4 scaling_w(float w)
	{
        return scaling( set ( 1.0f, 1.0f, 1.0f, w) );
	}

    inline float4x4 scaling_xy(float v)
	{
        return scaling( set ( v, v, 1.0f, 1.0f) );
	}

    inline float4x4 scaling_xz(float v)
	{
        return scaling( set ( v, 1.0f, v, 1.0f) );
	}

    inline float4x4 scaling_xw(float v)
	{
        return scaling( set ( v, 1.0f, z, 1.0f) );
	}

    inline float4x4 scaling_yz(float v)
	{
        return scaling( set ( 1.0f, v, v, 1.0f) );
	}

    inline float4x4 scaling_yw(float v)
	{
        return scaling( set ( 1.0f, v, 1.0f, v) );
	}

    inline float4x4 scaling_zw(float v)
	{
        return scaling( set ( 1.0f, 1.0f, v, v) );
	}
    
    inline float4x4 scaling(float x, float y, float z)
	{
		float4 v = set(x, y, z, 1.0f);
		return scaling(v);
	}

	inline float4x4 rotation_x(float sin_angle, float cos_angle)
	{
		float4x4 m;

		static const float4 negate_y = {1.0f, -1.0f,1.0f,1.0f};
		static const float4 identity_r0  = {1.0f, 0.0f, 0.0f, 0.0f};
		static const float4 identity_r3  = {0.0f, 0.0f, 0.0f, 1.0f};

		float4 v1 = _mm_set_ss(cos_angle);
		float4 v2 = _mm_set_ss(sin_angle);

		float4 v3 = shuffle<y,x,x,y>(v1, v2);
		float4 v4 = swizzle<x,z,y,w>(v3);
		float4 v5 = negate_y;
		float4 v6 = mul(v4, v5);

		m.r[0] = identity_r0;
		m.r[1] = v3;
		m.r[2] = v6;
		m.r[3] = identity_r3;

		return m;
	}

	inline float4x4 rotation_y(float sin_angle, float cos_angle)
	{
		float4x4 m;

		static const float4 negate_z = {1.0f, 1.0f,-1.0f,1.0f};
		static const float4 identity_r1  = {0.0f, 1.0f, 0.0f, 0.0f};
		static const float4 identity_r3  = {0.0f, 0.0f, 0.0f, 1.0f};

		float4 v1 = _mm_set_ss(cos_angle);
		float4 v2 = _mm_set_ss(sin_angle);

		float4 v3 = shuffle<x,y,x,y>(v1, v2);
		float4 v4 = swizzle<z,y,x,w>(v3);
		float4 v5 = negate_z;
		float4 v6 = mul(v4, v5);

		m.r[0] = v3;
		m.r[1] = identity_r1;
		m.r[2] = v6;
		m.r[3] = identity_r3;

		return m;
	}

	inline float4x4 rotation_z(float sin_angle, float cos_angle)
	{
		float4x4 m;

		static const float4 negate_x = {-1.0f, 1.0f,1.0f,1.0f};
		static const float4 identity_r2  = {0.0f, 0.0f, 1.0f, 0.0f};
		static const float4 identity_r3  = {0.0f, 0.0f, 0.0f, 1.0f};

		float4 v1 = _mm_set_ss(cos_angle);
		float4 v2 = _mm_set_ss(sin_angle);

		float4 v3 = _mm_unpacklo_ps(v1, v2);
		float4 v4 = swizzle<y,x,z,w>(v3);
		float4 v5 = negate_x;
		float4 v6 = mul(v4, v5);

		m.r[0] = v3;
		m.r[1] = v6;
		m.r[2] = identity_r2;
		m.r[3] = identity_r3;

		return m;
	}

	inline float4x4 rotation_x(float angle)
	{
		float sin_angle = sinf(angle);
		float cos_angle = cosf(angle);
		return rotation_x(sin_angle, cos_angle);
	}

	inline float4x4 rotation_y(float angle)
	{
		float sin_angle = sinf(angle);
		float cos_angle = cosf(angle);
		return rotation_y(sin_angle, cos_angle);
	}

	inline float4x4 rotation_z(float angle)
	{
		float sin_angle = sinf(angle);
		float cos_angle = cosf(angle);
		return rotation_z(sin_angle, cos_angle);
	}

	inline float4x4 view(float4 eye_position, float4 eye_direction, float4 up_direction)
	{
		//eye_direction and up_direction should be normalized;
		//eye_direction and up_direction w components should be zero

		static const uint32_t		mask_w[4] = { 0, 0, 0, 0xFFFFFFFF};
		static const uint32_t		mask_xyz[4] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0};
		static const float4	identity_r3 = {0.0f, 0.0f, 0.0f, 1.0f};

		float4x4 m1;
		float4x4 m2;

		float4 v0 = cross3(up_direction, eye_direction);
		v0 = normalize3(v0);

		float4 v1 = cross3(eye_direction, v0);
		float4 v2 = eye_direction;

		
		//form the translation
		float4 negative_eye_position = negate(eye_position);

		float4 d0 = dot3(v0, negative_eye_position);
		float4 d1 = dot3(v1, negative_eye_position);
		float4 d2 = dot3(v2, negative_eye_position);

		//clear all components except the w from the dot product
		d0 = and( d0, reinterpret_cast< const float4*> (&mask_w)[0] );
		d1 = and( d1, reinterpret_cast< const float4*> (&mask_w)[0] );
		d2 = and( d2, reinterpret_cast< const float4*> (&mask_w)[0] );

		m1.r[0] = or( v0, d0 );
		m1.r[1] = or( v1, d1 ); 
		m1.r[2] = or( v2, d2 ); 
		m1.r[3] = identity_r3;

		m2 = transpose(m1);
		return m2;
	}

	inline float4x4 look_at_lh(float4 eye_position, float4 look_at_position, float4 up_direction)
	{
		float4 v1 = sub(look_at_position, eye_position);
		float4 v2 = normalize3(v1);
		float4 v3 = normalize2(up_direction);
		return view(eye_position, v2, v3);
	}

	inline float4x4 perspective_lh(float view_width, float view_height, float z_near, float z_far)
	{
		static const uint32_t		mask_x[4] = { 0xFFFFFFFF, 0, 0, 0};
		static const uint32_t		mask_y[4] = { 0, 0xFFFFFFFF, 0, 0 };
		static const uint32_t		mask_yzw[4] = { 0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
		static const float4	identity_r3 = {0.0f, 0.0f, 0.0f, 1.0f};

		float a = 2 * z_near;

		float r = z_far / (z_far - z_near);

		float4 v1 = set( a / view_width, a / view_height, r, -r * z_near );
		float4 ze = zero();

		float4x4 m;

		float4 v2 = shuffle<x,y,x,y>(v1, ze);
		m.r[0] = and(v2, reinterpret_cast< const float4*> (&mask_x)[0] );

		float4 v3 = and(v1, reinterpret_cast< const float4*> (&mask_yzw)[0] );

		v2 = swizzle<x,y,x,x>(v1);
		m.r[1] = and ( v2, reinterpret_cast< const float4*> (&mask_y)[0] );

		float4 v4 = shuffle<z,w,z,w>(v3, identity_r3);

		m.r[2] = swizzle<z,z,x,w>(v4);
		m.r[3] = swizzle<z,z,y,z>(v4);

		return m;
	}

	inline float4x4 perspective_fov_lh(float fov, float aspect_ratio, float z_near, float z_far)
	{
		static const uint32_t		mask_x[4] = { 0xFFFFFFFF, 0, 0, 0};
		static const uint32_t		mask_y[4] = { 0, 0xFFFFFFFF, 0, 0 };
		static const uint32_t		mask_yzw[4] = { 0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
		static const float4	identity_r3 = {0.0f, 0.0f, 0.0f, 1.0f};

		float r = z_far / (z_far - z_near);

		float sin = sinf(fov * 0.5f);
		float cos = cosf(fov * 0.5f);
		float height = cos / sin;

		float4 v1 = set( height / aspect_ratio, height, r, -r * z_near );
		float4 ze = zero();

		float4x4 m;

		float4 v2 = shuffle<x,y,x,y>(v1, ze);
		m.r[0] = and(v2, reinterpret_cast< const float4*> (&mask_x)[0] );

		v2 = swizzle<x,y,x,x>(v1);
		m.r[1] = and ( v2, reinterpret_cast< const float4*> (&mask_y)[0] );

		float4 v3 = and(v1, reinterpret_cast< const float4*> (&mask_yzw)[0] );
		float4 v4 = shuffle<z,w,z,w>(v3, identity_r3);

		m.r[2] = swizzle<z,z,x,w>(v4);
		m.r[3] = swizzle<z,z,y,z>(v4);

		return m;
	}

	inline float4x4 orthographic_lh(float view_width, float view_height, float z_near, float z_far)
	{
		static const uint32_t		mask_x[4] = { 0xFFFFFFFF, 0, 0, 0};
		static const uint32_t		mask_yzw[4] = { 0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
		static const float4	identity_r3 = {0.0f, 0.0f, 0.0f, 1.0f};

		float a = 2.0f;

		float r = 1.0f / (z_far - z_near);

		float4 v1 = set( a / view_width, a / view_height, r, -r * z_near );
		float4 ze = zero();

		float4x4 m;

		float4 v2 = shuffle<x,y,x,y>(v1, ze);
		m.r[0] = and(v2, reinterpret_cast< const float4*> (&mask_x)[0] );

		float4 v3 = and(v1, reinterpret_cast< const float4*> (&mask_yzw)[0] );

		m.r[1] = swizzle<x,y,x,x>(v1);

		float4 v4 = shuffle<z,w,z,w>(v3, identity_r3);

		m.r[2] = swizzle<z,z,x,w>(v4);
		m.r[3] = swizzle<z,z,y,z>(v4);

		return m;
	}

	inline void extract_view_frustum(float4x4 wvp, float frustum[24])
	{
		float4 v1;
		float4 v2;
		float4 v3;
		float4 v4;
		float4 v5;
		float4 v6;

		float4x4 m1;

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
	inline float4 transform3(float4 v, float4x4 m)
	{
		// assume w component is 1 and multiply vector * matrix
		float4  v1 = swizzle<x,x,x,x>(v);
		float4  v2 = swizzle<y,y,y,y>(v);
		float4  v3 = swizzle<z,z,z,z>(v);

		float4  v5 = mul(v1, m.r[0]);
		float4  v6 = mul(v2, m.r[1]);
		float4  v7 = mad(v3, m.r[2], m.r[3]);

		float4  v8 = add(v5, v6);
		float4  v9 = add(v7, v8);

		return v9;
	}

	//does a perspective transform of a point, what the vertex shader + the gpu does
	inline float4 perspective_transform3(float4 v, float4x4 m)
	{
		// assume w component of the input is 1 and multiply vector * matrix
		float4  v1 = swizzle<x,x,x,x>(v);
		float4  v2 = swizzle<y,y,y,y>(v);
		float4  v3 = swizzle<z,z,z,z>(v);
		float4  v4 = swizzle<w,w,w,w>(v);

		float4  v5 = mul(v1, m.r[0]);
		float4  v6 = mul(v2, m.r[1]);
		float4  v7 = mad(v3, m.r[2], m.r[3]);

		float4  v8 = add(v5, v6);
		float4  v9 = add(v7, v8);

		//do perspective divide
		float4  v10 = rcp(v4);
		float4  v11 = mul(v9, v10 );

		return v11;
	}

	//projects a point on the screen and applies the view port transform. the result is in window coordinates
	inline float4 project( float4 v, float4x4 wvp, view_port view_port)
	{
		float4  v1	= perspective_transform3(v, wvp);

		float	half_height	= (view_port.m_bottom - view_port.m_top) * 0.5f;
		float	half_width	= (view_port.m_right - view_port.m_left) * 0.5f;

		float4  scale	= set( half_width,  -half_height, view_port.m_max_z - view_port.m_min_z, 0.0f);
		float4  offset	= set( view_port.m_left + half_width, view_port.m_top + half_height, view_port.m_min_z, 0.0f);

		return mad(v1, scale, offset );
	}

    //returns near and far from a projection matrix
    inline std::tuple<float, float> extract_near_far( math::float4x4 p )
    {
        float a = p.m[3][2];
        float b = p.m[2][2];

        float zf = a / ( 1 - b );
        float zn = - a / b;

        return std::make_tuple<float,float> ( zn, zf );
    }

    inline float4 rotate_vector3(float4 v, float4 quaternion )
    {
        float4 conjugate = quaternion_conjugate( quaternion );

        float4 result_1  = quaternion_mul( conjugate, v );
        float4 result_2  = quaternion_mul( result_1, quaternion );

        return result_2;
    }

    inline float4 reflect_vector3(float4 v, float4 quaternion )
    {
        float4 result_1  = quaternion_mul( quaternion, v );
        float4 result_2  = quaternion_mul( result_1, quaternion );

        return result_2;
    }

    inline float4 point3(float x, float y, float z)
    {
        return math::set(x,y,z, 1.0f);
    }

    inline float4 vector3(float x, float y, float z)
    {
        return math::set(x,y,z, 0.0f);
    }

}



#endif