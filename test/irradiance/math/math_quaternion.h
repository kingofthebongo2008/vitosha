#ifndef __MATH_QUATERNION_H__
#define __MATH_QUATERNION_H__

#include <intrin.h>
#include <cstdint>

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
        float4 length = dot4( q1, q1 );
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

    inline std::tuple< float4, float >    quaternion_to_axis_angle( float4 q )
    {
        static const uint32_t __declspec( align(16) )		mask_xyz[4] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0  };
        std::make_tuple< float4, float > ( and(q,  reinterpret_cast< const float4*> (&mask_xyz)[0] ) , 2 * acosf( q.m128_f32[3] )  );
    }

    inline float4   quaternion_normalize( float4 q1 )
    {
        return normalize4( q1 );
    }
}



#endif