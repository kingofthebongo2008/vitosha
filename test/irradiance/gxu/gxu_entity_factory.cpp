#include "precompiled.h"

#include <gxu/gxu_entity_factory.h>

#include <dx11/dx11_helpers.h>

#include <math/math_half.h>

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
		};

		struct normal
		{
			float m_x;
			float m_y;
			float m_z;
			float m_w;

			normal ( float x, float y, float z, float w) : m_x(x), m_y(y), m_z(z), m_w(w) {}
		};

		struct uv
		{
			float m_u;
			float m_v;

			uv ( float u, float v) : m_u(u), m_v(v) {}
		};

		struct normal_uv
		{
			normal	m_normal;
			uv		m_uv;
		};

	}

    gx::indexed_draw_call create_lat_lon_sphere( ID3D11Device* device , float radius, uint32_t subdivision_count )
    {
		subdivision_count;
		radius;
		device;

		dx11::id3d11buffer_ptr positions;
        dx11::id3d11buffer_ptr normals_uvs;
        dx11::id3d11buffer_ptr indices;
		gx::indexed_draw_call::index_info info = {};

		subdivision_count = subdivision_count + 1;

		uint32_t vertical_segments		=  subdivision_count; 
		uint32_t horizontal_segments	=  subdivision_count * 2;

		const float pi				= 3.141592654f;
		const float two_pi			= 6.283185307f;
		const float one_div_pi		= 0.318309886f;
		const float one_div_two_pi	= 0.159154943f;
		const float pi_div_two		= 1.570796327f;
		const float pi_div_four		= 0.785398163f;

		std::vector<position> positions_v;
		std::vector<normal_uv> normals_uvs_v;

		positions_v.reserve( (vertical_segments + 1 ) * ( horizontal_segments + 1) );
		normals_uvs_v.reserve( (vertical_segments + 1  ) * ( horizontal_segments + 1 ) );

		for ( uint32_t i = 0; i < vertical_segments + 1 ; ++i)
		{
			float v = ( 1.0f - i ) / vertical_segments;
			float lattitude = ( i * pi / vertical_segments ) - pi_div_two;

			float dxz;
			float dy;

			dy = sinf(lattitude);
			dxz = cosf(lattitude);

			for (uint32_t j = 0 ; j < horizontal_segments + 1 ; ++j )
			{
				float u = static_cast<float>  (j)  / horizontal_segments;
				float longitude = ( j * two_pi / horizontal_segments )  - pi; 

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

				positions_v.push_back( position ( x, y, z, 1.0f) );

				normal n ( dx, dy, dz, 1.0f );
				uv	   uv( u, v ) ;

				normal_uv nuv = { n, uv };

				normals_uvs_v.push_back ( nuv );
			}
		}

		std::vector<uint16_t> indices_v;
		indices_v.reserve((vertical_segments + 1 ) * ( horizontal_segments +  1 ) * 3 ) ;

		uint32_t stride = horizontal_segments + 1 ;

		for ( uint32_t i = 0 ; i < vertical_segments; ++i)
		{
			for (uint32_t j = 0; j < horizontal_segments + 1; ++j)
			{
				uint32_t next_i = i + 1;
				uint32_t next_j = (j + 1 ) % stride;

				indices_v.push_back( i * stride + j );
				indices_v.push_back( next_i * stride + j );
				indices_v.push_back( i * stride + next_j ); 

				indices_v.push_back ( i * stride + next_j );
				indices_v.push_back ( next_i * stride + j );
				indices_v.push_back ( next_i * stride + next_j );
			}
		}

		std::vector< math::half > positions_h_v;
		positions_h_v.resize ( 8 * ( (positions_v.size() * 4 + 7 ) / 8 ) );
		math::convert_f32_f16_stream( reinterpret_cast<float*> ( &positions_v[0]) , positions_h_v.size(), &positions_h_v[0] );

		std::vector< math::half > normals_uvs_h_v;
		normals_uvs_h_v.resize (  8 * ( ( normals_uvs_v.size() * 6 + 7) / 8 ) );
		math::convert_f32_f16_stream( reinterpret_cast<float*> ( &normals_uvs_v[0]) , normals_uvs_h_v.size(), &normals_uvs_h_v[0] );
		
		positions = dx11::create_vertex_buffer( device, &positions_h_v[0], positions_v.size() * 4 * sizeof(math::half) );
		normals_uvs = dx11::create_vertex_buffer( device, &normals_uvs_h_v[0], normals_uvs_v.size() * 6 * sizeof(math::half) );

		indices = dx11::create_index_buffer(  device, &indices_v[0], indices_v.size() * sizeof(uint16_t) );

		info.m_index_count = indices_v.size();
		info.m_vertex_size_0 = 8;
		info.m_vertex_size_1 = 12;

        return gx::indexed_draw_call(info, positions, normals_uvs, indices);
    }

	std::shared_ptr<gx::entity> create_lat_lon_sphere_entity( gx::render_context* context, gx::indexed_draw_call draw_call, math::vector_float4 color )
	{
		return std::make_shared< gxu::static_mesh_entity<gx::color_material> > ( draw_call , gx::create_color_material(context, color) );
	}

	std::shared_ptr<gx::entity> create_lat_lon_sphere_entity( gx::render_context* context, float radius, uint32_t subdivision_count, math::vector_float4 color )
    {
		return create_lat_lon_sphere_entity ( context, create_lat_lon_sphere(context->get_device(), radius, subdivision_count) , color );
    }
 }