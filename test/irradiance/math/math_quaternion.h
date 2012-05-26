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

    inline float4 matrix_2_quaternion_ref(float4x4 m)
    {
        float X = get_x( m.r[0] );
        float Y = get_y( m.r[1] );
        float Z = get_z( m.r[2] );
        float W = get_w( m.r[3] );

        float T = X + Y + Z;

        //m00, m01, m02, m03
        //m10, m11, m12, m13
        //m20, m21, m22, m23
        //m30, m31, m32, m33

        if ( T > 0.0f )
        {

            float k = sqrtf( T + W );
            float w = k / 2.0f;

            float m_21 = get_y(m.r[2]);
            float m_12 = get_z(m.r[1]);

            float m_02 = get_z(m.r[0]);
            float m_20 = get_x(m.r[2]);

            float m_10 = get_x(m.r[1]);
            float m_01 = get_y(m.r[0]);

            float x = (-m_21 + m_12);
            float y = (-m_02 + m_20);
            float z = (-m_10 + m_01);

            x /= 2 * k;
            y /= 2 * k;
            z /= 2 * k;

            return set ( x, y, z, w );
        }
        else
        {
            float m_21 = get_y(m.r[2]);
            float m_12 = get_z(m.r[1]);

            float m_02 = get_z(m.r[0]);
            float m_20 = get_x(m.r[2]);

            float m_10 = get_x(m.r[1]);
            float m_01 = get_y(m.r[0]);

            float x = (m_21 + m_12);
            float y = (m_02 + m_20);
            float z = (m_10 + m_01);

            float w_0 = (-m_21 + m_12);
            float w_1 = (-m_02 + m_20);
            float w_2 = (-m_10 + m_01);


            if ( X > Y )
            {
                if ( X > Z )
                {
                    //0, 1, 2
                    //q[0]

                    float v = sqrtf ( X - Y - Z + W ) / 2.0f ;

                    float q_0 = v;
                    float q_1 = 0.25f  * z / q_0 ;
                    float q_2 = 0.25f  * y / q_0;
                    float q_3 = 0.25f  * w_0 / q_0;

                    return set ( q_0, q_1, q_2, q_3 );

                }
                else
                {
                    //q[2]

                    float v = sqrtf ( - X - Y + Z + W ) / 2.0f;

                    float q_2 = v ;

                    float q_0 = 0.25f  * y / q_2;
                    float q_1 = 0.25f  * x / q_2;
                    float q_3 = 0.25f  * w_2 / q_2;

                    return set ( q_0, q_1, q_2, q_3 );
                }

            }
            else
            {
                if ( Y > Z )
                {
                    //q[1]
                    float v = sqrtf ( - X + Y - Z + W ) / 2.0f ;

                    float q_1 = v;

                    float q_0 = 0.25f  * z / q_1;
                    float q_2 = 0.25f  * x / q_1;
                    float q_3 = 0.25f  * w_1 / q_1;

                    return set ( q_0, q_1, q_2, q_3 );
                }
                else
                {
                    //q[2]

                    float v = sqrtf ( - X - Y + Z + W ) / 2.0f ;

                    float q_2 = v;

                    float q_0 = 0.25f  * y / q_2;
                    float q_1 = 0.25f  * x / q_2;
                    float q_3 = 0.25f  * w_2 / q_2;

                    return set ( q_0, q_1, q_2, q_3 );
                }
            }
        }
   }

    inline float4 matrix_2_quaternion_ref2(float4x4 m)
    {
        static const uint32_t	__declspec( align(16) )	mask_x[4]   = { 0xFFFFFFFF, 0, 0, 0 };
        static const float4 mask_one                                = { 1.0f, -1.0f, 0.0f, 0.0f};
        static const float4 mask_half                               = { 0.5f, 0.5f, 0.5f, 0.5f };
        static const float4 mask_quarter                            = { 0.25f, 0.25f, 0.25f, 0.25f };

        float4  v_mask_x = load4( reinterpret_cast<const float*> ( &mask_x[0] )  )  ;
        float4  v_mask_y = swizzle<y,x,y,y>(v_mask_x);
        float4  v_mask_z = swizzle<y,y,x,y>(v_mask_x);
        float4  v_mask_w = swizzle<y,y,y,x>(v_mask_x);
        
        float4  m_00   = splat_x(m.r[0]); // m00, m00, m00, m00
        float4  m_11   = splat_y(m.r[1]); // m11, m11, m11, m11
        float4  m_22   = splat_z(m.r[2]); // m22, m22, m22, m22

        float4 m_00_x = mul( m_00 , swizzle<x,y,y,x>(mask_one) );    // m00, -m00, -m00, m00
        float4 m_11_x = mul( m_11 , swizzle<y,x,y,x>(mask_one) );    // -m11, m11, -m11, m11
        float4 m_22_x = mul( m_22 , swizzle<y,y,x,x>(mask_one) );    // -m22, -m22, m22, m22
        float4 m_33_x = swizzle<x,x,x,x>(mask_one) ;                 //  m33,  m33, m33, m33

        float4  q_a   = add( m_00_x, add(m_11_x, add(m_22_x, m_33_x) ) );
        float4  q     = mul ( sqrt(q_a), mask_half ) ;                           // q[i] = sqrtf(q[i,i] - q[j,j] - q[k,k] + q[3,3]) / 2.0f; w = sqrtf( q[0,0] + q[1,1] + q[2,2] + q[3,3] ) /2.0f;

        float4  a_0 = swizzle<w, z , y , w> ( m.r[0] );         // 0,   m02, m01, 0
        float4  a_1 = swizzle<z, w , x , w> ( m.r[1] );         // m12, 0,   m10, 0
        float4  a_2 = swizzle<y, x , w , w> ( m.r[2] );         // m21, m20, 0,   0

        // common factors
        float4  a   = add ( a_0, add(a_1, a_2) );               // m12 + m21, m02 + m20, m01 + m10, 

        float4  b_0 = mul ( a_0, swizzle<w,y,x,w>(mask_one) );  // 0,      -m02,    m01,   0
        float4  b_1 = mul ( a_1, swizzle<x,w,y,w>(mask_one) );  // m12,    0,      -m10,    0
        float4  b_2 = mul ( a_2, swizzle<y,x,w,w>(mask_one) );  // -m21,   m20,   0,      0

        // common factors
        float4  b   = add ( b_0, add( b_1, b_2 ) );             // m12 - m21, - m02 + m20, m01 - m10, 0

        float X = get_x( m.r[0] );
        float Y = get_y( m.r[1] );
        float Z = get_z( m.r[2] );

        float T = X + Y + Z;

        if ( T > 0.0f )
        {
            float4 k = swizzle<w,w,w,w>(q);
            float4 xyz = div( b, k );
            xyz = mul ( xyz, mask_quarter );
            float4 w = mul (k, mask_half );
            return select ( xyz, w, v_mask_w) ;
        }
        else
        {
            if ( X > Y )
            {
                if ( X > Z )
                {
                    float4  q0 = swizzle<x,x,x,x>(q);

                    float4  q_012 = swizzle< w, z, y, w > ( a );            // 0, m01 + m10,  m02 + m20, 0
                    float4  w_012 = swizzle< w, w, w, x > ( b );            // 0, 0, 0, -m12 + m21

                    float4 q = add( q_012, w_012 );
                    q = div ( q, q0 );
                    q = mul ( q, mask_quarter) ;
                    q = select ( q, q0, v_mask_x );
                    return q;
                }
                else
                {
                    float4  q2 = swizzle<z,z,z,z>(q);

                    float4  q_120 = swizzle< y, x, w, w > ( a );            // m20 + m02,   m12 + m21,  0,         0
                    float4  w_120 = swizzle< w, w, w, z > ( b );            // 0, 0, 0, m01 - m10

                    float4 q = add( q_120, w_120 );
                    q = div ( q, q2 );
                    q = mul ( q, mask_quarter) ;
                    q = select ( q, q2, v_mask_z );
                    return q;
                }

            }
            else
            {
                if ( Y > Z )
                {
                    float4 q1=swizzle<y,y,y,y>(q);

                    float4  q_201 = swizzle< z, w, x, w > ( a );            // m01 + m10,   0,          m12 + m21, 0
                    float4  w_201 = swizzle< w, w, w, y > ( b );            // 0, 0, 0, -m02 + m20


                    float4 q = add( q_201, w_201 );
                    q = div ( q, q1 );
                    q = mul ( q, mask_quarter) ;
                    q = select ( q, q1, v_mask_y );
                    return q;

                }
                else
                {
                    float4  q2 = swizzle<z,z,z,z>(q);

                    float4  q_120 = swizzle< y, x, w, w > ( a );            // m20 + m02,   m12 + m21,  0,         0
                    float4  w_120 = swizzle< w, w, w, z > ( b );            // 0, 0, 0, m01 - m10

                    float4 q = add( q_120, w_120 );
                    q = div ( q, q2 );
                    q = mul ( q, mask_quarter) ;
                    q = select ( q, q2, v_mask_z );
                    return q;                }
            }
        }
   }

    inline float4 matrix_2_quaternion(float4x4 m) throw()
    {
        static const uint32_t	__declspec( align(16) )	mask_x[4]   = { 0xFFFFFFFF, 0, 0, 0 };
        static const float4 mask_one                                = { 1.0f, -1.0f, 0.0f, 0.0f};
        static const float4 mask_half                               = { 0.5f, 0.5f, 0.5f, 0.5f };
        static const float4 mask_quarter                            = { 0.25f, 0.25f, 0.25f, 0.25f };

        float4  v_mask_x = load4( reinterpret_cast<const float*> ( &mask_x[0] )  )  ;
        float4  v_mask_y = swizzle<y,x,y,y>(v_mask_x);
        float4  v_mask_z = swizzle<y,y,x,y>(v_mask_x);
        float4  v_mask_w = swizzle<y,y,y,x>(v_mask_x);
        
        float4  m_00   = splat_x(m.r[0]); // m00, m00, m00, m00
        float4  m_11   = splat_y(m.r[1]); // m11, m11, m11, m11
        float4  m_22   = splat_z(m.r[2]); // m22, m22, m22, m22

        float4 m_00_x = mul( m_00 , swizzle<x,y,y,x>(mask_one) );    // m00, -m00, -m00, m00
        float4 m_11_x = mul( m_11 , swizzle<y,x,y,x>(mask_one) );    // -m11, m11, -m11, m11
        float4 m_22_x = mul( m_22 , swizzle<y,y,x,x>(mask_one) );    // -m22, -m22, m22, m22
        float4 m_33_x = swizzle<x,x,x,x>(mask_one) ;                 //  m33,  m33, m33, m33

        float4  q_a   = add( m_00_x, add(m_11_x, add(m_22_x, m_33_x) ) );
        float4  q     = mul ( sqrt(q_a), mask_half ) ;                         // q[i] = sqrtf(q[i,i] - q[j,j] - q[k,k] + q[3,3]) / 2.0f; w = sqrtf( q[0,0] + q[1,1] + q[2,2] + q[3,3] ) /2.0f;


        float4  m_cmp_x = mul( m_00 , swizzle<x,w,w,x>(mask_one) );           // m00, 0,     0,   m00
        float4  m_cmp_y = mul( m_11 , swizzle<w,x,w,x>(mask_one) );           // 0,  m11,    0,   m11
        float4  m_cmp_z = mul( m_22 , swizzle<w,w,x,x>(mask_one) );           // 0,  0,      m22, m22

        float4  largest_component = add ( m_cmp_x, add ( m_cmp_y, m_cmp_z) );   // the largest component determines whichi is largest qx,qy,qz,qw

        float4  a_0 = swizzle<w, z , y , w> ( m.r[0] );         // 0,   m02, m01, 0
        float4  a_1 = swizzle<z, w , x , w> ( m.r[1] );         // m12, 0,   m10, 0
        float4  a_2 = swizzle<y, x , w , w> ( m.r[2] );         // m21, m20, 0,   0

        // common factors
        float4  a   = add ( a_0, add(a_1, a_2) );               // m12 + m21, m02 + m20, m01 + m10, 

        float4  b_0 = mul ( a_0, swizzle<w,y,x,w>(mask_one) );  // 0,      -m02,    m01,   0
        float4  b_1 = mul ( a_1, swizzle<x,w,y,w>(mask_one) );  // m12,    0,      -m10,    0
        float4  b_2 = mul ( a_2, swizzle<y,x,w,w>(mask_one) );  // -m21,   m20,   0,      0

        // common factors
        float4  b   = add ( b_0, add( b_1, b_2 ) );             // m12 - m21, - m02 + m20, m01 - m10, 0

        //cyclic permutation
        // i, j, k
        // 0, 1, 2
        // 2, 0, 1
        // 1, 2, 0

        float4  q_012 = swizzle< w, z, y, w > ( a );            // 0,           m01 + m10,  m02 + m20, 0
        float4  q_201 = swizzle< z, w, x, w > ( a );            // m01 + m10,   0,          m12 + m21, 0
        float4  q_120 = swizzle< y, x, w, w > ( a );            // m20 + m02,   m12 + m21,  0,         0

        float4  w_012 = swizzle< w, w, w, x > ( b );            // 0, 0, 0, -m12 + m21
        float4  w_201 = swizzle< w, w, w, y > ( b );            // 0, 0, 0, -m02 + m20
        float4  w_120 = swizzle< w, w, w, z > ( b );            // 0, 0, 0, m01 - m10

        float4  q_0 = add ( q_012, w_012 );
        float4  q_1 = add ( q_201, w_201 );
        float4  q_2 = add ( q_120, w_120 );

        //divide by qi
        q_0 = div ( q_0, swizzle<x,x,x,x>( q ) );
        q_1 = div ( q_1, swizzle<y,y,y,y>( q ) );
        q_2 = div ( q_2, swizzle<z,z,z,z>( q ) );

        q_0 = mul ( q_0, mask_quarter );            // divide by 4
        q_1 = mul ( q_1, mask_quarter );
        q_2 = mul ( q_2, mask_quarter );

        //add qi
        q_0 = select ( q_0, swizzle<x,x,x,x>( q ), v_mask_x );
        q_1 = select ( q_1, swizzle<y,y,y,y>( q ), v_mask_y );
        q_2 = select ( q_2, swizzle<z,z,z,z>( q ), v_mask_z );

        //calucate q3 if qw is the largest       
        float4 q_3 = div ( b, swizzle<w,w,w,w>(q) );
        
        q_3 = mul ( q_3, mask_quarter );
        q_3 = select( q_3, swizzle<w,w,w,w>(q), v_mask_w );

        //select q_0, q_1, q_2, q_3 based on the largest_component

        float4 m_0 = swizzle<x,x,x,x>(largest_component);
        float4 m_1 = swizzle<y,y,y,y>(largest_component);
        float4 m_2 = swizzle<z,z,z,z>(largest_component);
        float4 m_3 = swizzle<w,w,w,w>(largest_component);

        float4 mask_0 = compare_gt( m_0, m_1 ); // q0 > q1 ?
        float4 mask_1 = compare_gt( m_2, m_3 ); // q2 > q3 ?

        float4 q_01 = select ( q_1, q_0, mask_0 );
        float4 q_23 = select ( q_3, q_2, mask_1 );

        float4 m01 = select ( m_1, m_0, mask_0 );   
        float4 m23 = select ( m_3, m_2, mask_1 );
        
        float4 mask_2 = compare_gt( m01, m23 );  // max(q0,q1) > max(q2,q3) ?

        float4 q_0123 = select ( q_23, q_01, mask_2);

        return q_0123;
    }
}



#endif