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

    inline float4   quaternion_length( float4 q1)
    {
        return length4( q1 );
    }

    inline float4   quaternion_mul( float4 q1, float4 q2)
    {
        return one();
    }

    inline float4   quaternion_normal_angle( float4 normal, float angle)
    {
        float sin_angle = sinf(0.5f * angle);
        float cos_angle = cosf(0.5f * angle);

        static const uint32_t __declspec( align(16) )		mask_w[4] = { 0, 0, 0, 0xFFFFFFFF };
        static const float4	                        identity_r3 = {0.0f, 0.0f, 0.0f, 1.0f};

        float4 n = select( normal, identity_r3, reinterpret_cast< const float4*> (&mask_w)[0] ) ;

        float4   v = set ( sin_angle, sin_angle, sin_angle, cos_angle );

        return mul ( v, n );
    }

    inline float4   quaternion_axis_angle( float4 axis, float angle)
    {
        float4 normal = normalize3(axis);
        return quaternion_normal_angle( normal, angle );
    }

    inline std::tuple< float4, float >    quaternion_to_axis_angle( float4 q)
    {
        static const uint32_t __declspec( align(16) )		mask_xyz[4] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0  };
        std::make_tuple< float4, float > ( and(q,  reinterpret_cast< const float4*> (&mask_xyz)[0] ) , 2 * acosf( q.m128_f32[3] )  );
    }

    inline float4   quaternion_normalize( float4 q1)
    {
        return normalize4( q1 );
    }


}



#endif