#ifndef __MATH_QUATERNION_H__
#define __MATH_QUATERNION_H__

#include <intrin.h>
#include <cstdint>

#include <math/math_matrix.h>
#include <math/math_vector.h>
#include <math/math_functions.h>

namespace math
{ 
    inline float4   quaternion_conjugate( float4 q1)
    {
        float4 v = math::set( -1.0f, -1.0f, -1.0f, 1.0f );
        return mul ( q1, v );
    }

    inline float4    quaternion_dot( float4 q1, float4 q2)
    {
        return dot4( q1, q2 );
    }

    inline float4   quaternion_inverse( float4 q1 )
    {
        float4 length = quaternion_dot( q1, q1 );
        float4 conjugate = quaternion_conjugate( q1 );
        float4 result = div ( conjugate, length );

        return result;
    }

    inline float4   quaternion_length( float4 q1 )
    {
        return length4( q1 );
    }

    inline float4   quaternion_mul( float4 q, float4 r )
    {
        /*
        t0 = (  + r0q3 + r1q2 - r2q1 + r3q0)
        t1 = (  - r0q2 + r1q3 + r2q0 + r3q1)
        t2 = (  + r0q1 - r1q0 + r2q3 + r3q2)
        t3 = (  - r0q0 - r1q1 - r2q2 + r3q3)
        */

        float4 t;
        float4 q_1;
        float4 q_2;
        float4 q_3;

        const float4 mask_0  = { 1.0f, -1.0f,  1.0f, -1.0f};
        const float4 mask_1  = swizzle<x,x,y,y>(mask_0);
        const float4 mask_2  = swizzle<y,x,x,y>(mask_0);
        
        t = mul ( splat_w(r), q );

        q_1 = swizzle<w,z,y,x>(q);
        q_1 = mul (mask_0, q_1);
        q_1 = mul (q_1, splat_x(r));

        q_2 = swizzle<z,w,x,y>(q);
        q_2 = mul (mask_1, q_2);
        q_2 = mul (q_2, splat_y(r));

        q_3 = swizzle<y,x,w,z>(q);
        q_3 = mul (mask_2, q_3);
        q_3 = mul (q_3, splat_z(r));

        t = add( add ( t, q_2) , add (q_1, q_3) );

        return t;
    }

    inline float4   quaternion_normal_angle( float4 normal, float angle )
    {
        float sin_angle = sinf(0.5f * angle);
        float cos_angle = cosf(0.5f * angle);

        static const uint32_t __declspec( align(16) )   mask_w[4] = { 0, 0, 0, 0xFFFFFFFF };
        static const float4	                            identity_r3 = {0.0f, 0.0f, 0.0f, 1.0f};

        float4 n = select( normal, identity_r3, reinterpret_cast< const float4*> (&mask_w)[0] ) ;

        float4   v = set ( sin_angle, sin_angle, sin_angle, cos_angle );

        return mul ( v, n );
    }

    inline float4   quaternion_axis_angle( float4 axis, float angle )
    {
        float4 normal = normalize3(axis);
        return quaternion_normal_angle( normal, angle );
    }

    inline std::tuple< float4, float >    quaternion_axis_angle( float4 q )
    {
        static const uint32_t __declspec( align(16) )   mask_xyz[4] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0  };
        std::make_tuple< float4, float > ( and(q,  reinterpret_cast< const float4*> (&mask_xyz)[0] ) , 2 * acosf( get_w(q) )  );
    }

    inline float4   quaternion_normalize( float4 q1 )
    {
        return normalize4( q1 );
    }

    inline float4 matrix_2_quaternion(float4x4 rotation_matrix )
    {
        return one();
    }

    //converts quaterion to matrix. expects normalized quaternion
    inline float4x4 quaternion_2_matrix_ref(float4 quaternion )
    {
        static const float4 identity_r3  = {0.0f, 0.0f, 0.0f, 1.0f};

        float X = get_x(quaternion);
        float Y = get_y(quaternion);
        float Z = get_z(quaternion);
        float W = get_w(quaternion);

        float xx = X * X;
        float xy = X * Y;
        float xz = X * Z;
        float xw = X * W;

        float yy = Y * Y;
        float yz = Y * Z;
        float yw = Y * W;
        
        float zz = Z * Z;
        float zw = Z * W;

        float4x4 m;

        m.r[0] = set ( 1.0f - 2.0f*( yy + zz ), 2.0f * ( xy + zw ), 2.0f * ( xz - yw), 0.0f );
        m.r[1] = set ( 2.0f * ( xy - zw ), 1.0f - 2.0f * (xx + zz), 2.0f * ( yz + xw ), 0.0f );
        m.r[2] = set ( 2.0f * ( xz + yw ), 2.0f * ( yz - xw), 1.0f - 2.0f * ( xx + yy ), 0.0f );
        m.r[3] = identity_r3;

        return m;
    }

    //converts quaterion to matrix. expects normalized quaternion
    inline float4x4 quaternion_2_matrix(float4 quaternion )
    {
        float4 v_1 = mul ( quaternion, splat_x(quaternion) );
        float4 v_2 = mul ( quaternion, splat_y(quaternion) );
        float4 v_3 = mul ( quaternion, splat_z(quaternion) );

        float4 a_1 = shuffle<y,y,y,z> ( v_2, v_1 );
        float4 a_2 = shuffle<z,w,w,w> ( v_3, v_2 ); // zz, zw, yw

        a_1 = swizzle<x,z,w,z>(a_1);     // yy, xy, xz, ?

        float4 a_3 = shuffle<y,x, z, z >( v_1, v_2 );    //xy, xx, yz
        float4 a_4 = shuffle<w,z, w, w >( v_3, v_1 );    //zw, zz, xw

        float4 a_5 = shuffle<x, z, z, z >( v_1, v_2 );    
        float4 a_6 = shuffle< w, w, y, w >( v_1, v_2 );    //xw, xw, yy, yw

        a_5 = swizzle<y,z,x,x>(a_5);     // xz, yz, xx
        a_6 = swizzle<w,x,z,z>(a_6);    //  yw, xw, yy

        static const float4 masks   = {2.0f, -2.0f, 0.0f, 0.0f};
        static const float4 masks_1 = {1.0f, -1.0f, 0.0f, 0.0f};

        float4 mask = masks;
        float4 mask_1 = masks_1;

        float4 m_2 = swizzle<x,x,y,w>(mask_1);  
        float4 m_4 = swizzle<y,x,x,w>(mask_1);
        float4 m_6 = swizzle<x,y,x,w>(mask_1);

        float4 n_1 = swizzle<y,x,x,w>(mask);
        float4 n_2 = swizzle<x,y,x,w>(mask);
        float4 n_3 = swizzle<x,x,y,w>(mask);

        float4 q_1 = mad( m_2, a_2, a_1);
        float4 q_2 = mad( m_4, a_4, a_3);
        float4 q_3 = mad( m_6, a_6, a_5);

        float4 p_1 = swizzle<x,w,w,w>(mask_1);
        float4 p_2 = swizzle<w,x,w,w>(mask_1);
        float4 p_3 = swizzle<w,w,x,w>(mask_1);

        q_1 = mad( n_1, q_1, p_1);
        q_2 = mad( n_2, q_2, p_2);
        q_3 = mad( n_3, q_3, p_3);

        float4x4 m;

        m.r[0] = q_1;
        m.r[1] = q_2;
        m.r[2] = q_3;
        m.r[3] = swizzle<w,w,w,x>(masks_1);

        return m;
    }
}



#endif