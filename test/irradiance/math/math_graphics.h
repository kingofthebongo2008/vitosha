#ifndef __MATH_GRAPHICS_H__
#define __MATH_GRAPHICS_H__

#include <limits>
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

		static const uint32_t __declspec( align(16) ) mask_x[4] = { 0xFFFFFFFF, 0x0, 0, 0};
		static const uint32_t __declspec( align(16) ) mask_y[4] = { 0, 0xFFFFFFFF, 0, 0};
		static const uint32_t __declspec( align(16) ) mask_z[4] = { 0, 0, 0xFFFFFFFF, 0};

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

		float4 v3 = shuffle<x,y,x,y>(v2, v1);
		float4 v4 = swizzle<z,y,x,w>(v3);
		float4 v5 = negate_z;
		float4 v6 = mul(v4, v5);

		m.r[0] = v6;
		m.r[1] = identity_r1;
        m.r[2] = v3;
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

    //creates left handed view matrix
	inline float4x4 view(float4 eye_position, float4 eye_direction, float4 up_direction)
	{
		//eye_direction and up_direction should be normalized;
		//eye_direction and up_direction w components should be zero

		static const uint32_t   __declspec( align(16) ) mask_w[4] = { 0, 0, 0, 0xFFFFFFFF};
		static const uint32_t	__declspec( align(16) ) mask_xyz[4] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0};
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

        float4  mask = load4(mask_w);
        m1.r[0] = select( v0, d0, mask );
		m1.r[1] = select( v1, d1, mask ); 
		m1.r[2] = select( v2, d2, mask ); 
		m1.r[3] = identity_r3;


		m2 = transpose(m1);
		return m2;
	}

    //creates left handed view matrix
	inline float4x4 look_at_lh(float4 eye_position, float4 look_at_position, float4 up_direction)
	{
		float4 v1 = sub(look_at_position, eye_position);
		float4 v2 = normalize3(v1);
		float4 v3 = normalize2(up_direction);
		return view(eye_position, v2, v3);
	}

    //creates left handed perspective projection matrix
	inline float4x4 perspective_lh(float view_width, float view_height, float z_near, float z_far)
	{
		static const uint32_t	__declspec( align(16) )	mask_x[4] = { 0xFFFFFFFF, 0, 0, 0};
		static const uint32_t	__declspec( align(16) )	mask_y[4] = { 0, 0xFFFFFFFF, 0, 0 };
		static const uint32_t	__declspec( align(16) )	mask_yzw[4] = { 0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
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

    //creates left handed perspective projection matrix
	inline float4x4 perspective_fov_lh(float fov, float aspect_ratio, float z_near, float z_far)
	{
		static const uint32_t	__declspec( align(16) )	mask_x[4] = { 0xFFFFFFFF, 0, 0, 0};
		static const uint32_t	__declspec( align(16) )	mask_y[4] = { 0, 0xFFFFFFFF, 0, 0 };
		static const uint32_t	__declspec( align(16) )	mask_yzw[4] = { 0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
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

    //creates left handed orthographic projection matrix
	inline float4x4 orthographic_lh(float view_width, float view_height, float z_near, float z_far)
	{
		static const uint32_t	__declspec( align(16) )	mask_x[4] = { 0xFFFFFFFF, 0, 0, 0};
		static const uint32_t	__declspec( align(16) )	mask_yzw[4] = { 0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
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

    //extracts view frusutm from world view projection matrix
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
		float4  v10 = div(v9, v4 );

		return v10;
	}

	//projects a point on the screen and applies the view port transform. the result is in window coordinates
	//maps from 3d space -> view port
	inline float4 project( float4 v, float4x4 wvp, const view_port view_port)
	{
		float4  v1	= perspective_transform3(v, wvp);

		float	half_height	= (view_port.m_bottom - view_port.m_top) * 0.5f;
		float	half_width	= (view_port.m_right - view_port.m_left) * 0.5f;

		float4  scale	= set( half_width,  -half_height, view_port.m_max_z - view_port.m_min_z, 1.0f);
		float4  offset	= set( view_port.m_left + half_width, view_port.m_top + half_height, view_port.m_min_z, 0.0f);

		return	mad(v1, scale, offset );
	}

	//unprojects a point on the screen coordinates, removes the view port transform
	//maps from view port -> 3d space
	inline float4 unproject( float4 v, float4x4 inverse_wvp, const view_port view_port)
	{
		float	half_height	= (view_port.m_bottom - view_port.m_top) * 0.5f;
		float	half_width	= (view_port.m_right - view_port.m_left) * 0.5f;

		float4  scale	= set( half_width,  -half_height, view_port.m_max_z - view_port.m_min_z, 1.0f);
		float4  offset	= set( view_port.m_left + half_width, view_port.m_top + half_height, view_port.m_min_z, 0.0f);

		//undo the view port transform
		float4  v1 = sub ( v, offset );
		float4	v2 = div ( v1, scale );

		float4  v3 = perspective_transform3(v2, inverse_wvp);

		return	v3;
	}


    //returns near and far from a projection matrix
    inline std::tuple<float, float> extract_near_far( math::float4x4 p )
    {
        float a = p.m[3][2];
        float b = p.m[2][2];

        //float zf = a / ( 1.0f - b );
        //float zn = - a / b;

        return std::make_tuple<float,float> ( -a / b , a / (1.0f - b) );
    }

    //performs vector rotation with quaternion
    inline float4 rotate_vector3(float4 v, float4 quaternion )
    {
        float4 conjugate = quaternion_conjugate( quaternion );

        float4 result_1  = quaternion_mul( conjugate, v );
        float4 result_2  = quaternion_mul( result_1, quaternion );

        return result_2;
    }

    //reflects a vector around a quaternion
    inline float4 reflect_vector3(float4 v, float4 quaternion )
    {
        float4 result_1  = quaternion_mul( quaternion, v );
        float4 result_2  = quaternion_mul( result_1, quaternion );

        return result_2;
    }

    //creates quaternion that aligns vector s with vector t
    //todo parallel s and t
    inline float4 quaternion_rotate_vector3(float4 s, float4 t )
    {
        float4  e        = dot3(s , t);
        float4  cross    = cross3(s, t);
        float   v_1      = sqrtf( 2.0f * (1 + get_x(e) ) );

        float4 m         = splat ( 1.0f / v_1 );

        float4 v          = mul(m , cross );
        float4 w          = splat( v_1 / 2.0f);

        static const uint32_t __declspec( align(16) )   mask_w[4] = { 0, 0, 0, 0xFFFFFFFF };
        return select( v, w, reinterpret_cast< const float4*> (&mask_w)[0] ) ;
    }

    //creates matrix that aligns vector s with vector t
    //todo parallel s and t
    inline float4x4 matrix_rotate_vector3_ref(float4 s, float4 t )
    {

        float4 v = cross3( s, t);
        float4 e_v = dot4(s, t);
        float  e = get_x(e_v);
        float  h = 1.0f / ( 1.0f + e );

        float v_x = get_x(v);
        float v_y = get_y(v);
        float v_z = get_z(v);

        static const float4 identity_r3  = {0.0f, 0.0f, 0.0f, 1.0f};

        float4x4 m;

        m.r[0] = set ( e + h * v_x * v_x    , h * v_x * v_y + v_z   , h * v_x * v_z - v_y    , 0.0f );
        m.r[1] = set ( h * v_x * v_y - v_z  , e + h * v_y * v_y     , h * v_y * v_z  + v_x   , 0.0f );
        m.r[2] = set ( h * v_x * v_z + v_y  , h * v_y * v_z  - v_x  , e + h * v_z * v_z      , 0.0f );
        m.r[3] = identity_r3;

        return m;

    }

    //creates matrix that aligns vector s with vector t. usage :  v = mul (v , m );
    //todo or parallel s and t
    inline float4x4 matrix_rotate_vector3(float4 s, float4 t )
    {
        //  e + hvx^2, hvxvy + vz, hvxvz - vy, 0
        //  hvxvy - vz, e + hvy^2, hvyvz + vx, 0
        //  hvxvz + vy, hvyvz - vx, e + hvz^2, 0
        //  0         , 0         , 0        , 1

        //  v = sxt
        //  e = s.t;
        //  h = 1 / ( 1 + e)


        float4 v = cross3( s, t );
        float4 e = quaternion_dot(s, t);
        float4 h = splat ( 1.0f / (1.0f + get_x(e) ) ) ;

        float4 vx = mul ( h, mul ( v, splat_x(v) ) ); // hvxvx, hvxvy, hvxvz
        float4 vy = mul ( h, mul ( v, splat_y(v) ) ); // hvxvy, hvyvy, hvyvz
        float4 vz = mul ( h, mul ( v, splat_z(v) ) ); // hvxvz, hvyvz, hvzvz

		static const float4 identity_r3                             = { 0.0f, 0.0f, 0.0f, 1.0f};
        static const float4 masks                                   = { 1.0f, -1.0f, 0.0f, 0.0f};
        static const uint32_t	__declspec( align(16) )	mask_w[4]   = { 0,      0, 0, 0xFFFFFFFF };

        float4  v_mask_w = load4( reinterpret_cast<const float*> ( &mask_w[0] )  )  ;
        float4  mask = masks;

        float4 v_p = select( v, e, v_mask_w);
        float4 v_px = swizzle<x,x,x,z>(mask);

        float4 v_1 = swizzle<w,z,y,w>(v_p);     //e, vz, vy, ?
        float4 v_2 = swizzle<z,w,x,w>(v_p);     //vz, e, vx, ?
        float4 v_3 = swizzle<y,x,w,w>(v_p);     //vy, vx, e, ? 

        v_1 = mul(v_1, swizzle<x,x,y,z> ( mask) );  //e, +vz, -vy, 0
        v_2 = mul(v_2, swizzle<y,x,x,z> ( mask) );  //-vz, e, +vx, 0
        v_3 = mul(v_3, swizzle<x,y,x,z> ( mask) );  //+vy, -vx, e, 0

        vx = mul ( vx, v_px); // hvxvx, hvxvy, hvxvz, 0
        vy = mul ( vy, v_px); // hvxvy, hvyvy, hvyvz, 0
        vz = mul ( vz, v_px); // hvxvz, hvyvz, hvzvz, 0

        float4x4 m;

		m.r[0] = add( vx, v_1);
		m.r[1] = add( vy, v_2);
		m.r[2] = add( vz, v_3);
		m.r[3] = identity_r3;

        return m;
    }

    //creates a point in 3d
    inline float4 point3(float x, float y, float z)
    {
        return math::set(x,y,z, 1.0f);
    }

    //creates a vector in 3d
    inline float4 vector3(float x, float y, float z)
    {
        return math::set(x,y,z, 0.0f);
    }

    // see ken shoemake in graphic gems 4
    // http://www.talisman.org/~erlkonig/misc/shoemake92-arcball.pdf
    // x, y : screen coordinates
    // c    : arcball center ( 2d )
    // r    : arcball radius

    inline float4 arc_ball_point_on_unit_sphere( float x_, float y_, float r, float4 c )
    {
        static const uint32_t	__declspec( align(16) )	mask_z[4]   = { 0, 0, 0xFFFFFFFF, 0 };
        static const uint32_t	__declspec( align(16) )	mask_w[4]   = { 0, 0, 0, 0xFFFFFFFF };

        float4 s0       = point3( x_, y_, 0.0f );
        float4 v0       = div ( sub ( s0, c ), splat(r) ); // v0 = (s0-c) / r
        float4 dot      = dot2( v0, v0 );

        float4 v0_1     = mul ( v0, rsqrt( dot ) );     //normalize v0
        float4 z_       = sqrt ( sub ( one() , dot ) );
        
        float4 v_mask_z = load4( mask_z );
        float4 v0_2     = select ( v0, z_, v_mask_z ) ; // v0.x, v0.y,  sqrt( 1 - (vx * vy)^2), 0

        float4 cmp      = swizzle<x,x,x,x>(dot);
        float4 mask     = compare_gt ( cmp , one() );   // dot > 1.0f  

        float4 result   = select ( v0_2, v0_1, mask ); // select of two points, depending on the coordinates. snap on the ball

        // make point
        float4 v_mask_w = load4(mask_w); 
        result = select ( result, one(), v_mask_w) ;
        return result;
    }

    //create a rotation quaternion from v_0 to v_1 on the 3 sphere
    inline float4 arc_ball_quaternion( float4 v_0, float4 v_1)
    {
        static const uint32_t	__declspec( align(16) )	mask_w[4]   = { 0, 0, 0, 0xFFFFFFFF };

        float4 dot      = dot3 ( v_0, v_1 );
        float4 cross    = cross3(v_0, v_1 );

        // make point
        float4 v_mask_w = load4(mask_w); 
        float4 result = select ( cross, dot, v_mask_w) ;
        return result;
    }


    //given a point on the 3 sphere and a constraint axis, returns a point on the 3 sphere and plane perpendicular to the axis and through the center
    inline float4 arc_ball_constraint_on_axis( float4 sphere_point, float4 axis )
    {
        const  float4 dot       = dot3( sphere_point, axis);
        float4 projected_point  = sub( sphere_point, mul ( axis, dot ) );
        const  float4 norm      = dot3(projected_point, projected_point);

        const float4 result_1   = {1.0f, 0.0f, 0.0f, 1.0f };    
        const float4 mask_res_2 = {-1.0f, 1.0f, 0.0f, 0.0f };
        const float4 mask_res_3 = { 0.0f, 0.0f, 0.0f, 1.0f };

        static const uint32_t	__declspec( align(16) )	mask_w[4]   = { 0, 0, 0, 0xFFFFFFFF };

        float4 result_2 = swizzle< y, x, w, w> ( axis );        
        result_2 = mad( result_2, mask_res_2, mask_res_3);  // -axis.y, axis.x, 0, 0

        float4 s = rcp(norm);

        float4 one_     = one();
        float4 zero_    = zero();

        float4 mask = compare_lt ( swizzle<z,z,z,z> ( projected_point), zero_ );
        float4 invert = select( one_, minus_one(), mask );

        s = mul ( s, invert );
        float4 result_3 = mul ( projected_point, s );

        float4 axis_z = swizzle<z,z,z,z>(axis);

        // if axis.z == 1.0f
        float4 mask_axis_z = compare_eq ( axis_z, one_ );
        float4 result_12 = select( result_2, result_1, mask_axis_z ) ;

        // if norm > 0.0f
        float4 mask_dot = compare_gt( norm, zero_) ;
        float4 result_123 = select( result_12, result_3, mask_dot) ;

        
        // set w= 1.0f;
        return select( result_123, one(), load4(mask_w) );
    }

    inline float4 arc_ball_closest_axis( float4 sphere_point, const float4* axis, uint32_t axis_count)
    {
        const float  negative_infinity_float = -1.0f * std::numeric_limits<float>::infinity();
        const float4 negative_infinity = { negative_infinity_float, negative_infinity_float, negative_infinity_float, negative_infinity_float  };

        float4 result  = zero();
        float4 max_dot = negative_infinity;

        for (uint32_t i = 0; i < axis_count; ++i)
        {
            float4 closest_point = arc_ball_constraint_on_axis( sphere_point, axis[i] );

            float4 dot  = dot3(closest_point, sphere_point);

            float4 mask = compare_gt(dot, max_dot);

            result      = select( result, axis[i], mask );
            max_dot     = select( max_dot, dot, mask);
        }

        return result;
    }
}



#endif