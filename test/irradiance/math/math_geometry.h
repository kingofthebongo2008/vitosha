#ifndef __MATH_GEOMETRY_H__
#define __MATH_GEOMETRY_H__

#include <limits>
#include <tuple>

#include <math/math_vector.h>

namespace math
{
    template <uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3> 
    inline bool condition ( float4 v )
    {
        int32_t mask = movemask ( v );
        return mask ==  (  ( i3 << 3 ) | ( i2 << 2 ) |  ( i1 << 1 ) | i0  );
    }

    template <uint32_t i0, uint32_t i1, uint32_t i2> 
    inline bool condition ( float4 v )
    {
        int32_t mask = movemask ( v );
        return mask ==  (  ( i0 << 3 ) | ( i1 << 2 ) |  ( i2 << 1 )  );
    }

    template <uint32_t i0, uint32_t i1> 
    inline bool condition ( float4 v )
    {
        int32_t mask = movemask ( v );
        return mask ==  (  ( i0 << 3 ) | ( i1 << 2 ) );
    }

    template <uint32_t i0> 
    inline bool condition ( float4 v )
    {
        int32_t mask = movemask ( v );
        return mask ==  (  ( i0 << 3 ) );
    }

    inline bool any4 ( float4 v )
    {
        return ! condition<0,0,0,0> ( v );
    }

    inline bool all4 ( float4 v )
    {
        return condition<1,1,1,1> ( v );
    }

    inline bool any3 ( float4 v )
    {
        return ! condition<0,0,0> ( v );
    }

    inline bool all3 ( float4 v )
    {
        return condition<1,1,1> ( v );
    }

    inline bool any2 ( float4 v )
    {
        return ! condition<0,0> ( v );
    }

    inline bool all2 ( float4 v )
    {
        return condition<1,1> ( v );
    }

    inline bool any1 ( float4 v )
    {
        return ! condition<0> ( v );
    }

    inline bool all1 ( float4 v )
    {
        return condition<1> ( v );
    }

    namespace
    {
        template<uint32_t bit> inline bool bit_is_set ( uint32_t value )
        {
            return (value & (1<<bit)) !=0; 
        }
    }

    //sutherland hodgeman clipping.
    //The clip3 function computes the intersection of triangle (v0; v1; v2) with the half-space (x; y; z) * n > 0
    //see Efﬁcient Triangle and Quadrilateral Clipping within Shaders for more details
    //returns number of clipped triangles
    inline std::tuple< uint32_t, float4, float4, float4, float4> clip3( float4 n, float4 v0, float4 v1, float4 v2)
    {
        const float4 d0 = dot3(v0,n);
        const float4 d1 = dot3(v1,n);
        const float4 d2 = dot3(v2,n);

        float4       v3;

        const float4 da = merge_xy( d0, d1 );
        float4       dist = shuffle< x, y, z, z > ( da, d2) ;

        const float4 epsilon2 = splat ( 0.01f );
        const float4 epsilon  = splat ( -0.00001f );

        //all clipped
        if ( !any3 ( compare_ge ( dist, epsilon2) )  ) 
        {
            return std::make_tuple( 0, zero(), zero(), zero(), zero() );
        }

        //none clipped
        if (  all3 (  compare_ge ( dist, epsilon ) ) )
        {
            return std::make_tuple( 3, v0 , v1 , v2 , v0 );
        }

        // There are either 1 or 2 vertices above the clipping plane .

        uint32_t above = movemask ( compare_ge( dist, zero() ) );
        bool     next_is_above;

        if ( bit_is_set<1> ( above ) && ! bit_is_set<0>(above) )
        {
            next_is_above = bit_is_set<2> ( above );

            v3 = v0;
            v0 = v1;
            v1 = v2;
            v2 = v3;

            dist = swizzle<y,z,x,w>(dist);
        }
        else if (  bit_is_set<2> ( above ) && ! bit_is_set<1>(above) ) 
        {
            next_is_above = bit_is_set<0> ( above );
            v3 = v2;
            v2 = v1;
            v1 = v0;
            v0 = v3;

            dist = swizzle<z,x,y,w>(dist);
        }
        else
        {
            next_is_above = bit_is_set<1> ( above );
        }

        float4  a0 = swizzle<z,z,y,w>( dist );
        float4  a1 = swizzle<x,y,x,w>( dist );

        float4  k = div ( a1, sub ( a1, a0 ) );

        v3 = lerp( v0, v2,  get_x( k ) );        

        if (next_is_above)
        {
            // There is a quadrilateral above the plane
            //
            //    v0---------v1
            //      \        |
            //   ....v3......v2'...
            //          \    |
            //            \  |
            //              v2
            v2 = lerp ( v1, v2, get_y( k ) );
            return std::make_tuple( 4, v0 , v1 , v2 , v3 );
        }
        else
        {

            // There is a triangle above the plane
            //
            //            v0
            //           / |
            //         /   |
            //   ....v2'..v1'...
            //      /      |
            //    v2-------v1

            v1 = lerp ( v0, v1, get_z( k ) );
            v2 = v3;
            v3 = v0;

            return std::make_tuple( 3, v0 , v1 , v2 , v3 );
        }
    }

    //four aabb boxes
    struct aabb4
    {
            float4 m_x12;   // xmin,  xmax, xmin, xmax
            float4 m_y12;   // ymin,  ymax, ymin, ymax
            float4 m_z12;   // zmin,  zmax, zmin, zmax

            float4 m_x34;   // xmin,  xmax, xmin, xmax
            float4 m_y34;   // ymin,  ymax, ymin, ymin
            float4 m_z34;   // zmin,  zmax, zmin, zmax
    };

    struct aabb
    {
            float4 m_center;
            float4 m_diagonal;  //must be positive
    };

    struct frustum
    {
        enum plane_type : std::uint32_t
        {
            near_p    = 0,
            far_p     = 1,
            left_p    = 2,
            right_p   = 3,
            up_p      = 4,
            down_p    = 5
        };

        enum plane_count : std::uint32_t
        {
            value = 6
        };

        typedef float4 plane;

        //plane normals are expected to point inside the frustum
        plane   m_planes[6];
    };

    enum frustum_cull_result : uint8_t
    {
        inside = 0,
        intersect = 1,
        outside = 2
    };

    //converts aabb to aabb4 for frustum culling. box_count should be divisible by 4, since we will process aabb's by 4
    void convert_aabb_2_aabb4( const aabb* const __restrict b, aabb4* __restrict aabb4, uint32_t box_count )
    {
        uint32_t j = 0;

        for (uint32_t i = 0; i < box_count; i +=4 )
        {
            const aabb* const __restrict b1 = &b[i];
            const aabb* const __restrict b2 = &b[i+1];
            const aabb* const __restrict b3 = &b[i+2];
            const aabb* const __restrict b4 = &b[i+3];

            float4 b1_min = sub ( b1->m_center, b1->m_diagonal);
            float4 b1_max = add ( b1->m_center, b1->m_diagonal);

            float4 b2_min = sub ( b2->m_center, b2->m_diagonal);
            float4 b2_max = add ( b2->m_center, b2->m_diagonal);

            float4 b3_min = sub ( b3->m_center, b3->m_diagonal);
            float4 b3_max = add ( b3->m_center, b3->m_diagonal);

            float4 b4_min = sub ( b4->m_center, b4->m_diagonal);
            float4 b4_max = add ( b4->m_center, b4->m_diagonal);

            float4 b4_1 = 
                            swizzle <x,z,y,w> (
                            shuffle <x,y,x,y> (
                            swizzle <x,z,x,x> ( shuffle< x, x, x, x> ( b1_min, b2_min ) ),
                            swizzle <x,z,x,x> ( shuffle< x, x, x, x> ( b1_max, b2_max ) )
                                             )
                                             );
            stream ( &aabb4->m_x12, b4_1);

            float4 b4_2 = 
                            swizzle <x,z,y,w> (
                            shuffle <x,y,x,y> (
                            swizzle <x,z,x,x> ( shuffle< y, y, y, y> ( b1_min, b2_min ) ),
                            swizzle <x,z,x,x> ( shuffle< y, y, y, y> ( b1_max, b2_max ) )
                                             )
                                             );

            stream ( &aabb4->m_y12, b4_2);

            float4 b4_3 = 
                            swizzle <x,z,y,w> (
                            shuffle <x,y,x,y> (
                            swizzle <x,z,x,x> ( shuffle< z, z, z, z> ( b1_min, b2_min ) ),
                            swizzle <x,z,x,x> ( shuffle< z, z, z, z> ( b1_max, b2_max ) )
                                             )
                                             );
            stream ( &aabb4->m_z12, b4_3);

            float4 b4_4 = 
                            swizzle <x,z,y,w> (
                            shuffle <x,y,x,y> (
                            swizzle <x,z,x,x> ( shuffle< x, x, x, x> ( b3_min, b4_min ) ),
                            swizzle <x,z,x,x> ( shuffle< x, x, x, x> ( b3_max, b4_max ) )
                                             )
                                             );
            stream ( &aabb4->m_x34, b4_4);

            float4 b4_5 = 
                            swizzle <x,z,y,w> (
                            shuffle <x,y,x,y> (
                            swizzle <x,z,x,x> ( shuffle< y, y, y, y> ( b3_min, b4_min ) ),
                            swizzle <x,z,x,x> ( shuffle< y, y, y, y> ( b3_max, b4_max ) )
                                             )
                                             );
            stream ( &aabb4->m_y34, b4_5);

            float4 b4_6 = 
                            swizzle <x,z,y,w> (
                            shuffle <x,y,x,y> (
                            swizzle <x,z,x,x> ( shuffle< z, z, z, z> ( b3_min, b4_min ) ),
                            swizzle <x,z,x,x> ( shuffle< z, z, z, z> ( b3_max, b4_max ) )
                                             )
                                             );
            stream ( &aabb4->m_z34, b4_6);
            ++j;
        }
    }

    //sphere approximation for the aabb, approximate test, might report intersection, when the object is outside
    frustum_cull_result frustum_cull ( const frustum* const __restrict f, const aabb* const __restrict b )
    {
        float4 m;
        float4 n;

        frustum_cull_result result = frustum_cull_result::inside;

        for (uint32_t i = 0; i < frustum::plane_count::value; ++i)
        {
            float4 p = load4( &f->m_planes[i] );
            
            m = dot4 ( b->m_center, p );
            n = dot3 ( b->m_diagonal, abs(p) );
             
            if ( movemask ( compare_le ( add ( m , n ), zero() ) ) != 0 ) 
            {
                return frustum_cull_result::outside;
            }

            if ( movemask ( compare_le ( sub ( m , n ), zero() ) ) != 0 ) 
            {
                result = frustum_cull_result::intersect;
            }
        }

        return result;
    }

    namespace
    {
        //aabb frustum for 4boxes and 1 plane
        template <
        std::uint32_t shuffle_mask_n_x, 
        std::uint32_t shuffle_mask_n_y,
        std::uint32_t shuffle_mask_n_z,
        std::uint32_t shuffle_mask_p_x,
        std::uint32_t shuffle_mask_p_y,
        std::uint32_t shuffle_mask_p_z
        > 
        
        inline std::tuple< __m128i, bool>  intersect_p_n( float4 plane, const aabb4* __restrict b,  __m128i exclude, __m128i intersect, __m128i intersected )
        {
            //n vertex 
            float4 nx = mul ( shuffle<shuffle_mask_n_x>( b->m_x12, b->m_x34 ), swizzle<x, x, x, x >( plane ) );
            float4 ny = mul ( shuffle<shuffle_mask_n_y>( b->m_y12, b->m_y34 ), swizzle<y, y, y, y> ( plane ) );
            float4 nz = mul ( shuffle<shuffle_mask_n_z>( b->m_z12, b->m_z34 ), swizzle<z, z, z, z> ( plane ) );
            float4 nw = swizzle< w, w, w, w> ( plane ) ; 

            float4 m = add( add (nx, ny), add(nz, nw) );
            float4 result_1c = compare_gt( m, zero() ); 

            //if n vertex is outside, then all boxes are outside
            if ( all4 ( result_1c ) )
            {
                intersected = exclude;
                return std::make_tuple( intersected , false) ;
            }

            __m128i result1iConverted   = _mm_cvtps_epi32 ( result_1c ); 
            __m128i result1iShifted     = _mm_srai_epi32  ( result1iConverted, 31 ); 
            __m128i result1i            = _mm_and_si128   ( result1iShifted, exclude ); 

            //p vertex
            float4 px = mul ( shuffle<shuffle_mask_p_x>( b->m_x12, b->m_x34 ), swizzle<x, x, x, x >( plane ) );
            float4 py = mul ( shuffle<shuffle_mask_p_y>( b->m_y12, b->m_y34 ), swizzle<y, y, y, y> ( plane ) );
            float4 pz = mul ( shuffle<shuffle_mask_p_z>( b->m_z12, b->m_z34 ), swizzle<z, z, z, z> ( plane ) );

            float4 pw = swizzle< w, w, w, w> ( plane ) ; 
            float4 n = add( add(px, py), add(pz, pw) ) ;

            //if p vertex is inside, then intersect
            float4 result2c = compare_gt( n, zero() ); 

            __m128i result2iConverted   = _mm_cvtps_epi32(result2c); 
            __m128i result2iShifted     = _mm_srai_epi32 ( result2iConverted, 31 ); 
            __m128i result2i            = _mm_and_si128  ( result2iShifted, intersect ); 

            intersected = _mm_or_si128(intersected, result2i);
            intersected = _mm_or_si128(intersected, result1i);

            return std::make_tuple( intersected, true ) ;
        }
    }

    void frustum_cull ( const frustum* __restrict f, const aabb4* __restrict b, uint32_t box_count, __m128i* __restrict results)
    {
        __m128i exclude;
        __m128i intersect;

        static const uint32_t __declspec( align(16) )   mask_sign       [4] = { 0xffffffff, 0xffffffff, 0xffffffff, 0 };
        static const uint32_t __declspec( align(16) )   intersect_mask  [4] = { 0x1, 0x1, 0x1, 0x1 };
        static const uint32_t __declspec( align(16) )   exclude_mask    [4] = { 0x2, 0x2, 0x2, 0x2  };

        exclude     = load4i( &exclude_mask[0] );
        intersect   = load4i( &intersect_mask[0] );

        for (std::uint32_t i = 0; i < box_count; ++i)
        {
            //mark as inside
            __m128i intersected = { 0, 0, 0, 0 };

            for (std::uint32_t j = 0; j < 6; ++j)
            {
                float4 plane = load4( &f->m_planes[j] );

                float4 maskedPlane = and(plane, load4( &mask_sign[0]) );
            
                std::int32_t mask = movemask(maskedPlane);

                const aabb4* __restrict box = &b[i];

                if ( mask == 0x0 )	/// min, min, min - max,max,max
                {
                    std::tuple< __m128i, bool>  r = intersect_p_n
                        < 
                            shuffle_mask(x,z,x,z),  
                            shuffle_mask(x,z,x,z),  
                            shuffle_mask(x,z,x,z),  

                            shuffle_mask(y,w,y,w),
                            shuffle_mask(y,w,y,w),
                            shuffle_mask(y,w,y,w)
                        > 
                        
                        ( plane, box, exclude, intersect, intersected);

                    //if all outside, contnue with the next box
                    intersected = std::get<0>(r);

                    if ( !std::get<1>(r) )
                    {
                        goto continue_loop;
                    }
                }

                else if (mask == 0x01) // max, min, min - min, max, max
                {
                    std::tuple< __m128i, bool>  r = intersect_p_n
                        < 
                            shuffle_mask(y,w,y,w),  
                            shuffle_mask(x,z,x,z),  
                            shuffle_mask(x,z,x,z),  

                            shuffle_mask(x,z,x,z),
                            shuffle_mask(y,w,y,w),
                            shuffle_mask(y,w,y,w)
                        > 
                        
                        ( plane, box, exclude, intersect, intersected);

                    //if all outside, contnue with the next box
                    intersected = std::get<0>(r);

                    if ( !std::get<1>(r) )
                    {
                        goto continue_loop;
                    }
                }
                else if ( mask == 0x2 ) // min, max, min - max, min, max
                {
                    std::tuple< __m128i, bool>  r = intersect_p_n
                        < 
                            shuffle_mask(x,z,x,z),  
                            shuffle_mask(y,w,y,w),  
                            shuffle_mask(x,z,x,z),  

                            shuffle_mask(y,w,y,w),
                            shuffle_mask(x,z,x,z),
                            shuffle_mask(y,w,y,w)
                        > 
                        
                        ( plane, box, exclude, intersect, intersected);

                    //if all outside, contnue with the next box
                    intersected = std::get<0>(r);

                    if ( !std::get<1>(r) )
                    {
                        goto continue_loop;
                    }

                }
                else if ( mask == 0x3 ) // max, max, min - min, min, max
                {
                    std::tuple< __m128i, bool>  r = intersect_p_n
                        < 
                            shuffle_mask(y,w,y,w),  
                            shuffle_mask(y,w,y,w),  
                            shuffle_mask(x,z,x,z),  

                            shuffle_mask(x,z,x,z),
                            shuffle_mask(x,z,x,z),
                            shuffle_mask(y,w,y,w)
                        > 
                        
                        ( plane, box, exclude, intersect, intersected);

                    //if all outside, contnue with the next box
                    intersected = std::get<0>(r);

                    if ( !std::get<1>(r) )
                    {
                        goto continue_loop;
                    }

                } else if ( mask == 0x4 ) // min, min, max - max, max, min
                {
                    std::tuple< __m128i, bool>  r = intersect_p_n
                        < 
                            shuffle_mask(x,z,x,z),  
                            shuffle_mask(x,z,x,z),  
                            shuffle_mask(y,w,y,w),  

                            shuffle_mask(y,w,y,w),
                            shuffle_mask(y,w,y,w),
                            shuffle_mask(x,z,x,z)
                        > 
                        
                        ( plane, box, exclude, intersect, intersected);

                        //if all outside, contnue with the next box
                        intersected = std::get<0>(r);

                        if ( !std::get<1>(r) )
                        {
                            goto continue_loop;
                        }

                } else if ( mask == 0x5 ) //max, min, max - min, max, min
                {
                    std::tuple< __m128i, bool>  r = intersect_p_n
                        < 
                            shuffle_mask(y,w,y,w),  
                            shuffle_mask(x,z,x,z),  
                            shuffle_mask(y,w,y,w),  

                            shuffle_mask(x,z,x,z),
                            shuffle_mask(y,w,y,w),
                            shuffle_mask(x,z,x,z)
                        > 
                        
                        ( plane, box, exclude, intersect, intersected);

                        //if all outside, contnue with the next box
                        intersected = std::get<0>(r);

                        if ( !std::get<1>(r) )
                        {
                            goto continue_loop;
                        }

                } else if ( mask == 0x6 ) //min, max, max - max, min, min 
                {
                        std::tuple< __m128i, bool>  r = intersect_p_n
                        < 
                            shuffle_mask(x,z,x,z),  
                            shuffle_mask(y,w,y,w),  
                            shuffle_mask(y,w,y,w),  

                            shuffle_mask(y,w,y,w),
                            shuffle_mask(x,z,x,z),
                            shuffle_mask(x,z,x,z)
                        > 
                        
                        ( plane, box, exclude, intersect, intersected);

                        //if all outside, contnue with the next box
                        intersected = std::get<0>(r);

                        if ( !std::get<1>(r) )
                        {
                            goto continue_loop;
                        }

                } else if ( mask == 0x7) //max,max,max - min, min, min
                {
                        std::tuple< __m128i, bool>  r = intersect_p_n
                        < 
                            shuffle_mask(y,w,y,w),  
                            shuffle_mask(y,w,y,w),  
                            shuffle_mask(y,w,y,w),  

                            shuffle_mask(x,z,x,z),
                            shuffle_mask(x,z,x,z),
                            shuffle_mask(x,z,x,z)
                        > 
                        
                        ( plane, box, exclude, intersect, intersected);

                        //if all outside, contnue with the next box
                        intersected = std::get<0>(r);

                        if ( !std::get<1>(r) )
                        {
                            goto continue_loop;
                        }
                }
            }

            continue_loop:
            _mm_stream_si128(results, intersected);
            results++;
        }
    }
}

#endif
