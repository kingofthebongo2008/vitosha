#include "precompiled.h"

#include <gxu/gxu_entity_factory.h>

#include <d3d11/d3d11_helpers.h>

#include <math/math_functions.h>
#include <math/math_half.h>
#include <math/math_matrix.h>

#include <gx/gx_color_material.h>
#include <gx/gx_indexed_draw_call.h>
#include <gx/gx_render_context.h>

#include <gxu/gxu_static_mesh_entity.h>

namespace gxu
{
	namespace
	{
		struct position
		{
			float m_x;
			float m_y;
			float m_z;
			float m_w;
			position ( float x, float y, float z, float w) : m_x(x), m_y(y), m_z(z), m_w(w) {}
			position(){}
		};

		struct normal
		{
			float m_x;
			float m_y;
			float m_z;
			float m_w;

			normal ( float x, float y, float z, float w) : m_x(x), m_y(y), m_z(z), m_w(w) {}
			normal() {}
		};

		struct uv
		{
			float m_u;
			float m_v;

			uv ( float u, float v) : m_u(u), m_v(v) {}
			uv() {}
		};

		struct normal_uv
		{
			normal	m_normal;
			uv		m_uv;
		};

	}

    gx::indexed_draw_call_2 create_lat_lon_sphere( ID3D11Device* device , float radius, uint32_t subdivision_count )
    {
		d3d11::ibuffer_ptr positions;
        d3d11::ibuffer_ptr normals_uvs;
        d3d11::ibuffer_ptr indices;
		

		subdivision_count = subdivision_count + 1;

		auto vertical_segments		=  subdivision_count; 
		auto horizontal_segments	=  subdivision_count * 2;

		const float pi				= 3.141592654f;
		const float two_pi			= 6.283185307f;
		const float pi_div_two		= 1.570796327f;

		uint32_t			  vertex_count = 0;
		std::vector<position> positions_v;
		std::vector<normal_uv> normals_uvs_v;

		positions_v.resize( (vertical_segments + 1 ) * ( horizontal_segments + 1) );
		normals_uvs_v.resize( (vertical_segments + 1  ) * ( horizontal_segments + 1 ) );

		for ( uint32_t i = 0; i < vertical_segments + 1 ; ++i)
		{
			float v = ( 1.0f - i ) / vertical_segments;
			float lattitude = ( i * pi / vertical_segments ) - pi_div_two;

			float dxz;
			float dy;

			dy = sinf(lattitude);
			dxz = cosf(lattitude);

			uint32_t horizontal_segments_div_4 = ( (horizontal_segments + 1) / 4) * 4;

			//iterate on 4 vertices at the same time
			for (uint32_t j = 0; j < horizontal_segments_div_4 ; j+=4, vertex_count+=4 )
			{
				auto v_1 = math::set(static_cast<float> (j), static_cast<float> (j+1), static_cast<float> (j+2), static_cast<float> (j+3) );
				auto v_2 = math::splat(static_cast<float> (horizontal_segments) );

				auto u = math::div(v_1, v_2);

				auto v_3 = math::splat( two_pi );
				auto v_4 = math::splat( static_cast<float> (horizontal_segments) );
				auto v_5 =  math::splat( pi );

				auto v_6 =  math::mul( v_1, v_3 );
				auto v_7 =  math::div( v_6, v_4 );
				auto longitude =  math::sub(v_7, v_5);

				auto dx =  math::sin(longitude);
				auto dz =  math::cos(longitude);

				auto v_8 =  math::splat( dxz );

				dx = math::mul(dx, v_8);
				dz = math::mul(dz, v_8);

				auto v_9 =  math::splat( radius );
				auto v_10 =  math::splat( dy );

				auto x =  math::mul(dx, v_9);
				auto y =  math::mul(v_10, v_9);
				auto z =  math::mul(dz, v_9);
				auto one =  math::one();

				math::float4x4 p_1;

				p_1.r[0] = x;
				p_1.r[1] = y;
				p_1.r[2] = z;
				p_1.r[3] = one;

				//store positions
				auto p_2 = math::transpose(p_1);
				auto address = &positions_v[vertex_count];
				math::store44( reinterpret_cast<float*> (address), p_2);

				//sse 1,2,3 requires memory addresses to be 16 byte aligned. we cannot guarantee this, so we store byte by byte
				auto uv_address = &normals_uvs_v[vertex_count];
				uv_address->m_normal = normal( dx.m128_f32[0], v_10.m128_f32[0],  dz.m128_f32[0], 1.0f);
				uv_address->m_uv = uv( u.m128_f32[0], v );

				uv_address++;
				uv_address->m_normal = normal(dx.m128_f32[1], v_10.m128_f32[1], dz.m128_f32[1], 1.0f);
				uv_address->m_uv = uv( u.m128_f32[1], v );

				uv_address++;
				uv_address->m_normal = normal( dx.m128_f32[2], v_10.m128_f32[2], dz.m128_f32[2], 1.0f);
				uv_address->m_uv = uv( u.m128_f32[2], v );

				uv_address++;
				uv_address->m_normal = normal( dx.m128_f32[3], v_10.m128_f32[3], dz.m128_f32[3], 1.0f);
				uv_address->m_uv = uv( u.m128_f32[3], v );
			}

			//iterate on the remainder element by element
			for (uint32_t j = horizontal_segments_div_4 ; j < horizontal_segments + 1 ; ++j, ++vertex_count )
			{
				auto u = static_cast<float>  (j)  / horizontal_segments;
				auto longitude = ( j * two_pi / horizontal_segments )  - pi; 

				float dx;
				float dz;

				dx = sinf(longitude);
				dz = cosf(longitude);

				dx = dx * dxz;
				dz = dz * dxz;

				float x, y, z;

				x = dx * radius;
				y = dy * radius;
				z = dz * radius;

				//store the position
				position* address = &positions_v[vertex_count];
				*address = position ( x, y, z, 1.0f);

				//store normal and uv
				normal_uv* uv_address = &normals_uvs_v[vertex_count];
				uv_address->m_normal = normal(dx, dy, dz, 1.0f );
				uv_address->m_uv.m_u = u;
				uv_address->m_uv.m_v = v;
			}
		}

		std::vector<uint16_t> indices_v;
		indices_v.reserve((vertical_segments + 1 ) * ( horizontal_segments +  1 ) * 3 ) ;

		//generate triangles
		uint32_t stride = horizontal_segments + 1 ;

		for ( auto i = static_cast<uint32_t>(0) ; i < vertical_segments; ++i)
		{
			for (auto j = static_cast<uint32_t>(0); j < horizontal_segments + 1; ++j)
			{
				uint32_t next_i = i + 1;
				uint32_t next_j = (j + 1 ) % stride;

				indices_v.push_back( static_cast<uint16_t> (i * stride + j) );
				indices_v.push_back( static_cast<uint16_t> (next_i * stride + j) );
				indices_v.push_back( static_cast<uint16_t> (i * stride + next_j) ); 

				indices_v.push_back ( static_cast<uint16_t> (i * stride + next_j) );
				indices_v.push_back ( static_cast<uint16_t> (next_i * stride + j) );
				indices_v.push_back ( static_cast<uint16_t> (next_i * stride + next_j) );
			}
		}

		//convert to half to save memory on the gpu
		//conversion library operates on elements divisible by 8 to improve speed

		std::vector< math::half > positions_h_v;
		positions_h_v.resize ( 8 * ( (positions_v.size() * 4 + 7 ) / 8 ) );	//allocate more space than we need
		math::convert_f32_f16_stream( reinterpret_cast<float*> ( &positions_v[0]) , positions_h_v.size(), &positions_h_v[0] );

		std::vector< math::half > normals_uvs_h_v;
		normals_uvs_h_v.resize (  8 * ( ( normals_uvs_v.size() * 6 + 7) / 8 ) );
		math::convert_f32_f16_stream( reinterpret_cast<float*> ( &normals_uvs_v[0]) , normals_uvs_h_v.size(), &normals_uvs_h_v[0] );
		
		positions = d3d11::create_immutable_vertex_buffer( device, &positions_h_v[0], positions_v.size() * 4 * sizeof(math::half) );
		normals_uvs = d3d11::create_immutable_vertex_buffer( device, &normals_uvs_h_v[0], normals_uvs_v.size() * 6 * sizeof(math::half) );

		indices = d3d11::create_immutable_index_buffer(  device, &indices_v[0], indices_v.size() * sizeof(uint16_t) );

		return gx::create_indexed_draw_call<8,12> ( static_cast<uint32_t> ( indices_v.size() ), positions, normals_uvs, indices ) ;
    }

	std::tuple< d3d11::ibuffer_ptr, d3d11::ibuffer_ptr, d3d11::ibuffer_ptr, uint32_t > create_lat_lon_sphere_2( ID3D11Device* device, float radius, uint32_t subdivision_count )
	{
		d3d11::ibuffer_ptr positions;
        d3d11::ibuffer_ptr normals_uvs;
        d3d11::ibuffer_ptr indices;
		

		subdivision_count = subdivision_count + 1;

		auto vertical_segments		=  subdivision_count; 
		auto horizontal_segments	=  subdivision_count * 2;

		const float pi				= 3.141592654f;
		const float two_pi			= 6.283185307f;
		const float pi_div_two		= 1.570796327f;

		uint32_t			  vertex_count = 0;
		std::vector<position> positions_v;
		std::vector<normal_uv> normals_uvs_v;

		positions_v.resize( (vertical_segments + 1 ) * ( horizontal_segments + 1) );
		normals_uvs_v.resize( (vertical_segments + 1  ) * ( horizontal_segments + 1 ) );

		for ( uint32_t i = 0; i < vertical_segments + 1 ; ++i)
		{
			float v = ( 1.0f - i ) / vertical_segments;
			float lattitude = ( i * pi / vertical_segments ) - pi_div_two;

			float dxz;
			float dy;

			dy = sinf(lattitude);
			dxz = cosf(lattitude);

			uint32_t horizontal_segments_div_4 = ( (horizontal_segments + 1) / 4) * 4;

			//iterate on 4 vertices at the same time
			for (uint32_t j = 0; j < horizontal_segments_div_4 ; j+=4, vertex_count+=4 )
			{
				auto v_1 = math::set(static_cast<float> (j), static_cast<float> (j+1), static_cast<float> (j+2), static_cast<float> (j+3) );
				auto v_2 = math::splat(static_cast<float> (horizontal_segments) );

				auto u = math::div(v_1, v_2);

				auto v_3 = math::splat( two_pi );
				auto v_4 = math::splat( static_cast<float> (horizontal_segments) );
				auto v_5 =  math::splat( pi );

				auto v_6 =  math::mul( v_1, v_3 );
				auto v_7 =  math::div( v_6, v_4 );
				auto longitude =  math::sub(v_7, v_5);

				auto dx =  math::sin(longitude);
				auto dz =  math::cos(longitude);

				auto v_8 =  math::splat( dxz );

				dx = math::mul(dx, v_8);
				dz = math::mul(dz, v_8);

				auto v_9 =  math::splat( radius );
				auto v_10 =  math::splat( dy );

				auto x =  math::mul(dx, v_9);
				auto y =  math::mul(v_10, v_9);
				auto z =  math::mul(dz, v_9);
				auto one =  math::one();

				math::float4x4 p_1;

				p_1.r[0] = x;
				p_1.r[1] = y;
				p_1.r[2] = z;
				p_1.r[3] = one;

				//store positions
				auto p_2 = math::transpose(p_1);
				auto address = &positions_v[vertex_count];
				math::store44( reinterpret_cast<float*> (address), p_2);

				//sse 1,2,3 requires memory addresses to be 16 byte aligned. we cannot guarantee this, so we store byte by byte
				auto uv_address = &normals_uvs_v[vertex_count];
				uv_address->m_normal = normal( dx.m128_f32[0], v_10.m128_f32[0],  dz.m128_f32[0], 1.0f);
				uv_address->m_uv = uv( u.m128_f32[0], v );

				uv_address++;
				uv_address->m_normal = normal(dx.m128_f32[1], v_10.m128_f32[1], dz.m128_f32[1], 1.0f);
				uv_address->m_uv = uv( u.m128_f32[1], v );

				uv_address++;
				uv_address->m_normal = normal( dx.m128_f32[2], v_10.m128_f32[2], dz.m128_f32[2], 1.0f);
				uv_address->m_uv = uv( u.m128_f32[2], v );

				uv_address++;
				uv_address->m_normal = normal( dx.m128_f32[3], v_10.m128_f32[3], dz.m128_f32[3], 1.0f);
				uv_address->m_uv = uv( u.m128_f32[3], v );
			}

			//iterate on the remainder element by element
			for (uint32_t j = horizontal_segments_div_4 ; j < horizontal_segments + 1 ; ++j, ++vertex_count )
			{
				auto u = static_cast<float>  (j)  / horizontal_segments;
				auto longitude = ( j * two_pi / horizontal_segments )  - pi; 

				float dx;
				float dz;

				dx = sinf(longitude);
				dz = cosf(longitude);

				dx = dx * dxz;
				dz = dz * dxz;

				float x, y, z;

				x = dx * radius;
				y = dy * radius;
				z = dz * radius;

				//store the position
				position* address = &positions_v[vertex_count];
				*address = position ( x, y, z, 1.0f);

				//store normal and uv
				normal_uv* uv_address = &normals_uvs_v[vertex_count];
				uv_address->m_normal = normal(dx, dy, dz, 1.0f );
				uv_address->m_uv.m_u = u;
				uv_address->m_uv.m_v = v;
			}
		}

		std::vector<uint16_t> indices_v;
		indices_v.reserve((vertical_segments + 1 ) * ( horizontal_segments +  1 ) * 3 ) ;

		//generate triangles
		uint32_t stride = horizontal_segments + 1 ;

		for ( auto i = static_cast<uint32_t>(0) ; i < vertical_segments; ++i)
		{
			for (auto j = static_cast<uint32_t>(0); j < horizontal_segments + 1; ++j)
			{
				uint32_t next_i = i + 1;
				uint32_t next_j = (j + 1 ) % stride;

				indices_v.push_back( static_cast<uint16_t> (i * stride + j) );
				indices_v.push_back( static_cast<uint16_t> (next_i * stride + j) );
				indices_v.push_back( static_cast<uint16_t> (i * stride + next_j) ); 

				indices_v.push_back ( static_cast<uint16_t> (i * stride + next_j) );
				indices_v.push_back ( static_cast<uint16_t> (next_i * stride + j) );
				indices_v.push_back ( static_cast<uint16_t> (next_i * stride + next_j) );
			}
		}

		//convert to half to save memory on the gpu
		//conversion library operates on elements divisible by 8 to improve speed

		std::vector< math::half > positions_h_v;
		positions_h_v.resize ( 8 * ( (positions_v.size() * 4 + 7 ) / 8 ) );	//allocate more space than we need
		math::convert_f32_f16_stream( reinterpret_cast<float*> ( &positions_v[0]) , positions_h_v.size(), &positions_h_v[0] );

		std::vector< math::half > normals_uvs_h_v;
		normals_uvs_h_v.resize (  8 * ( ( normals_uvs_v.size() * 6 + 7) / 8 ) );
		math::convert_f32_f16_stream( reinterpret_cast<float*> ( &normals_uvs_v[0]) , normals_uvs_h_v.size(), &normals_uvs_h_v[0] );
		
		positions = d3d11::create_immutable_vertex_buffer( device, &positions_h_v[0], positions_v.size() * 4 * sizeof(math::half) );
		normals_uvs = d3d11::create_immutable_vertex_buffer( device, &normals_uvs_h_v[0], normals_uvs_v.size() * 6 * sizeof(math::half) );

		indices = d3d11::create_immutable_index_buffer(  device, &indices_v[0], indices_v.size() * sizeof(uint16_t) );

		return std::make_tuple( positions, normals_uvs, indices, static_cast<uint32_t> (indices_v.size()) );
    }

 }