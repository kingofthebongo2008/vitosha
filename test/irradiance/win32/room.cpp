#include "precompiled.h"

#include "room.h"

#include <d3d11/d3d11_error.h>
#include <d3d11/d3d11_helpers.h>

#include <math/math_half.h>
#include <math/math_graphics.h>

#include <gx/gx_draw_call_collector.h>
#include <gx/gx_draw_call_collector_context.h>
#include <gx/gx_draw_call_context.h>
#include <gx/gx_shader_database.h>

void room_entity::on_create_draw_calls( gx::draw_call_collector_context* context, gx::draw_call_collector* collector)
{
	auto key = gx::create_gbuffer_draw_call(this->m_material.get_id(), gx::get_perspective_transform3_depth(context) );
    collector->add_draw_call(key, context->m_entity_index ) ;
}

void room_entity::on_execute_draw_calls( gx::draw_call_context* draw_call_context )
{
    auto device_context = draw_call_context->m_device_context;

	m_material.apply(draw_call_context);
	m_draw_call.draw(device_context);
}

namespace
{
	enum : uint32_t
	{
		PL_TRIANGLE_LIST = 0,
		PL_TRIANGLE_STRIP,
		PL_TRIANGLE_FAN,
	};

	struct float2
	{
		float x,y;
	};

	struct float3
	{
		float x,y,z;
	};

	struct float4
	{
		float x,y,z,w;
	};

	struct material
	{
		char	 diffuse_mp_file[64];
		char	 bump_map_file[64];
	};

	struct triangle
	{
		uint32_t	m_v[3];
		float3		m_normal;
	};

	struct offset_table
	{
		uint32_t m_material_chunk;
		uint32_t m_vertex_chunk;
		uint32_t m_triangle_chunk;
		uint32_t m_mesh_chunk;
		uint32_t m_bone_chunk;
	};

	struct primitive
	{
		D3D11_PRIMITIVE_TOPOLOGY	m_type;
		std::vector<uint32_t>		m_indices;
	};

	struct animation_key
	{
		uint32_t start_time;
		uint32_t end_time;
		float	 m_values[4];
	};

	struct mesh
	{
		//shading
		char						m_name[64];
		uint32_t					m_material_index;

		//geometry
		uint32_t					m_base_vertex;
		uint32_t					m_vertex_count;
		uint32_t					m_base_triangle;;
		uint32_t					m_triangle_count;

		//skinning
		std::vector<uint32_t>		m_bones;
		std::vector<primitive>		m_primitives;

		//relation ships
		uint32_t					m_id;
		uint32_t					m_parent_id;
		std::vector<uint32_t>		m_children;

		//animation
		std::vector<animation_key>	m_position_keys;
		std::vector<animation_key>	m_rotation_keys;
		std::vector<animation_key>	m_scale_keys;

	};


	void add_to_material_range(uint32_t *indices, uint32_t &index, const int mat, const uint32_t startMesh, const uint32_t meshCount, const mesh* meshes, const triangle* triangles, uint32_t* material_range)
	{
		for (uint32_t mesh = startMesh; mesh < startMesh + meshCount; mesh++)
		{
			uint32_t base  = meshes[mesh].m_base_triangle;
			uint32_t count = meshes[mesh].m_triangle_count;

			for (uint32_t p = 0; p < count; p++)
			{
				indices[index++] = triangles[base + p].m_v[0];
				indices[index++] = triangles[base + p].m_v[1];
				indices[index++] = triangles[base + p].m_v[2];
			}
		}
		material_range[mat + 1] = index;
	}

}


std::shared_ptr<room_entity> create_room_entity( ID3D11Device* device, const gx::shader_database* database, std::istream& in )
{
	uint32_t		version = 0;
	int8_t			header[9] = {"AtiModel"};
	int8_t			read_header[9];
	int32_t			animation_type = 0;
	offset_table	offsets = {};

	in.read((char*) &read_header[0], 8);

	if ( in.eof() )
	{
		return nullptr;
	}
	read_header[8] = '\0';

	if ( _strnicmp( (char*) &read_header[0],  (char*) &header[0], 8 ) !=0 )
	{
		return nullptr;
	}

	in.read( (char*) &animation_type, sizeof(animation_type));
	in.read( (char*) &version, sizeof(version));
	in.read( (char*) &offsets, sizeof(offsets) );

	in.seekg(offsets.m_material_chunk, std::ios_base::beg);

	uint32_t material_count = 0;

	in.read( (char*) &material_count, sizeof(material_count) );

	std::vector<material> materials( material_count );

	for (uint32_t i = 0 ; i < material_count; ++i)
	{
		in.seekg(64, std::ios_base::cur);

		in.seekg(sizeof(float4), std::ios_base::cur);
		in.seekg(sizeof(float4), std::ios_base::cur);
		in.seekg(sizeof(float), std::ios_base::cur);

		in.read( (char*) &materials[i].diffuse_mp_file, 64 );
		in.seekg(64, std::ios_base::cur);

		in.read( (char*) &materials[i].bump_map_file, 64 );
		in.seekg(64, std::ios_base::cur);

		in.seekg(64, std::ios_base::cur);
	}

	in.seekg(offsets.m_vertex_chunk, std::ios_base::beg);
	uint32_t vertex_count = 0;

	in.read( (char*) &vertex_count, sizeof(vertex_count) );

	std::vector<float3> positions( vertex_count );
	std::vector<float3> normal( vertex_count );
	std::vector<float3> tangent( vertex_count );
	std::vector<float3> binormal( vertex_count );
	std::vector<float2> uv( vertex_count );
	std::vector<float4> color( vertex_count );

	in.read( (char*) &positions[0],	vertex_count * sizeof(float3) );
	in.read( (char*) &normal[0],	vertex_count * sizeof(float3) );
	in.read( (char*) &tangent[0],	vertex_count * sizeof(float3) );
	in.read( (char*) &binormal[0],	vertex_count * sizeof(float3) );
	in.read( (char*) &uv[0],		vertex_count * sizeof(float2) );
	in.read( (char*) &color[0],		vertex_count * sizeof(float4) );

	size_t size = positions.size();
	size_t padded_size = 24 * ( ( size + 23 ) / 24 ) ;
	

	for (uint32_t i = 0; i < padded_size - size; ++i)
	{
		float3 pad3 = {0.0f, 0.0f, 0.0f};
		float2 pad2 = {0.0f, 0.0f};
		float4 pad4 = {1.0f, 1.0f,1.0f,1.0f};

		positions.push_back(pad3);
		normal.push_back(pad3);
		tangent.push_back(pad3);
		binormal.push_back(pad3);

		uv.push_back(pad2);
		color.push_back(pad4);
	}


	in.seekg(offsets.m_triangle_chunk, std::ios_base::beg);
	uint32_t triangle_count = 0;

	in.read( (char*) &triangle_count, sizeof(triangle_count) );
	
	std::vector<triangle> triangles(triangle_count);
	in.read( (char*) &triangles[0],	triangle_count * sizeof(triangle) );


	in.seekg(offsets.m_mesh_chunk, std::ios_base::beg);
	uint32_t mesh_count = 0;

	in.read( (char*) &mesh_count, sizeof(mesh_count) );

	std::vector<mesh> meshes(mesh_count);

	for ( uint32_t i = 0; i < mesh_count; ++i)
	{
		meshes[i].m_id = i;
		in.read( (char*) &meshes[i].m_name[0], 64 );
		in.read( (char*) &meshes[i].m_material_index, sizeof(uint32_t) );
		in.read( (char*) &meshes[i].m_base_vertex, sizeof(uint32_t) );
		in.read( (char*) &meshes[i].m_vertex_count, sizeof(uint32_t) );
		in.read( (char*) &meshes[i].m_base_triangle, sizeof(uint32_t) );
		in.read( (char*) &meshes[i].m_triangle_count, sizeof(uint32_t) );

		uint32_t bone_count = 0;
		in.read( (char*) &bone_count, sizeof(uint32_t) );
		meshes[i].m_bones.resize(bone_count);
		if (bone_count > 0 )
		{
			in.read( (char*) &meshes[i].m_bones[0], bone_count * sizeof(uint32_t) );
		}

		in.read( (char*) &meshes[i].m_parent_id, sizeof(uint32_t) );
		uint32_t child_count = 0;
		in.read( (char*) &child_count, sizeof(uint32_t) );
		meshes[i].m_children.resize(child_count);
		if (child_count > 0)
		{
			in.read( (char*) &meshes[i].m_children[0], child_count * sizeof(uint32_t) );
		}

		uint32_t primitive_count = 0;
		in.read( (char*) &primitive_count, sizeof(uint32_t) );
		meshes[i].m_primitives.resize(primitive_count);

		for(uint32_t j = 0; j < primitive_count;++j)
		{
			uint32_t type = 0;
			in.read( (char*) &type, sizeof(uint32_t) );

			switch (type)
			{
				case PL_TRIANGLE_STRIP:
					meshes[i].m_primitives[j].m_type = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
					break;
				case PL_TRIANGLE_LIST:
				default:
					meshes[i].m_primitives[j].m_type = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
					break;
			}

			uint32_t indices_count = 0;
			in.read( (char*) &indices_count, sizeof(uint32_t) );
			meshes[i].m_primitives[j].m_indices.resize(indices_count);
			if (indices_count > 0)
			{
				in.read( (char*) &meshes[i].m_primitives[j].m_indices[0], indices_count * sizeof(uint32_t) );
			}
		}

		uint32_t position_key_count = 0;
		in.read( (char*) &position_key_count, sizeof(uint32_t) );
		meshes[i].m_position_keys.resize(position_key_count);

		if (position_key_count > 0)
		{
			in.read( (char*) &meshes[i].m_position_keys[0], position_key_count * sizeof(animation_key) );
		}


		uint32_t rotation_key_count = 0;
		in.read( (char*) &rotation_key_count, sizeof(uint32_t) );
		meshes[i].m_rotation_keys.resize(rotation_key_count);
		if ( rotation_key_count > 0)
		{
			in.read( (char*) &meshes[i].m_rotation_keys[0], rotation_key_count * sizeof(animation_key) );
		}


		uint32_t scale_key_count = 0;
		in.read( (char*) &scale_key_count, sizeof(uint32_t) );
		meshes[i].m_scale_keys.resize(scale_key_count);

		if (scale_key_count > 0)
		{
			in.read( (char*) &meshes[i].m_scale_keys[0], scale_key_count * sizeof(animation_key) );
		}
	}

	std::vector<math::half> positions_h( 4 * padded_size );
	std::vector<math::half> normals_h( 4 * padded_size );
	std::vector<math::half> uv_h( 2 * padded_size );

	math::convert_3_x_f32_f16_stream( reinterpret_cast<float*> ( &positions[0]), 3 * padded_size, 1.0f, &positions_h[0]);
	math::convert_3_x_f32_f16_stream( reinterpret_cast<float*> ( &normal[0]), 3 * padded_size, 1.0f, &normals_h[0]);
	math::convert_f32_f16_stream( reinterpret_cast<float*> ( &uv[0]), 2 * padded_size, &uv_h[0]);


	//combine normals and uvs
		

	std::vector<math::half> normals_uv( 6 * padded_size);
	uint32_t j = 0;
	uint32_t k = 0;
	for ( uint32_t i = 0; i < 6 * padded_size; i+=6, j+=4, k+=2)
	{
		normals_uv[i] = normals_h[j];
		normals_uv[i+1] = normals_h[j+1];
		normals_uv[i+2] = normals_h[j+2];
		normals_uv[i+3] = normals_h[j+3];
		normals_uv[i+4] = uv_h[k];
		normals_uv[i+5] = uv_h[k+1];
	}

	uint32_t material_range[14];
	std::vector<uint32_t> indices( triangle_count * 3);
	uint32_t index = 0;

	material_range[0] = 0;

	// Untextured materials
	add_to_material_range(&indices[0], index, 0,  19, 1, &meshes[0], &triangles[0], &material_range[0]); // Hand
	add_to_material_range(&indices[0], index, 1,  20, 1, &meshes[0], &triangles[0], &material_range[0]); // Ball
	add_to_material_range(&indices[0], index, 1,  23, 1, &meshes[0], &triangles[0], &material_range[0]); // Horse
	add_to_material_range(&indices[0], index, 1,  25, 1, &meshes[0], &triangles[0], &material_range[0]); // Sci-Fi weirdo
	add_to_material_range(&indices[0], index, 1,  28, 1, &meshes[0], &triangles[0], &material_range[0]); // Bench
	add_to_material_range(&indices[0], index, 1,  30, 1, &meshes[0], &triangles[0], &material_range[0]); // Frame
	add_to_material_range(&indices[0], index, 2,  24, 1, &meshes[0], &triangles[0], &material_range[0]); // Horse stand
	add_to_material_range(&indices[0], index, 2,  26, 1, &meshes[0], &triangles[0], &material_range[0]); // Sci-Fi weirdo stand
	add_to_material_range(&indices[0], index, 2,  32, 1, &meshes[0], &triangles[0], &material_range[0]); // Globe stand
	add_to_material_range(&indices[0], index, 3,  3, 15, &meshes[0], &triangles[0], &material_range[0]); // Ceiling, Pillars, Stands, Wall lights
	add_to_material_range(&indices[0], index, 4,  0,  1, &meshes[0], &triangles[0], &material_range[0]); // Walls
	add_to_material_range(&indices[0], index, 5,  21, 1, &meshes[0], &triangles[0], &material_range[0]); // Teapot
	// Masked materials
	add_to_material_range(&indices[0], index, 6,  27, 1, &meshes[0], &triangles[0], &material_range[0]); // Globe
	// Textured materials
	add_to_material_range(&indices[0], index, 7,  18, 1, &meshes[0], &triangles[0], &material_range[0]); // Ball-horse
	add_to_material_range(&indices[0], index, 8,  22, 1, &meshes[0], &triangles[0], &material_range[0]); // Head
	add_to_material_range(&indices[0], index, 9,  29, 1, &meshes[0], &triangles[0], &material_range[0]); // Picture
	add_to_material_range(&indices[0], index, 10,  1, 1, &meshes[0], &triangles[0], &material_range[0]); // Floor
	// Lightmapped materials
	add_to_material_range(&indices[0], index, 11, 2,  1, &meshes[0], &triangles[0], &material_range[0]); // Ceiling
	add_to_material_range(&indices[0], index, 12, 31, 1, &meshes[0], &triangles[0], &material_range[0]); // Wall light quads

	//create buffers
	auto positions_vb = d3d11::create_immutable_vertex_buffer( device, &positions_h[0], positions_h.size() * sizeof(math::half) );
	auto normals_uvs_vb = d3d11::create_immutable_vertex_buffer( device, &normals_uv[0], normals_uv.size() * sizeof(math::half) );
	auto indices_ib = d3d11::create_immutable_index_buffer(  device, &indices[0], indices.size() * sizeof(uint32_t) );



	if ( in.eof() )
	{
		return nullptr;
	}


	return std::make_shared<room_entity>
    (  
		std::move( gx::create_indexed_draw_call<8,12, gx::bit_32> ( static_cast<uint32_t> ( indices.size() ), positions_vb, normals_uvs_vb, indices_ib )  ),
		std::move( gx::create_blinn_phong_shift_invairant_material( database, math::set( 1.0f, 0.0f, 0.0f, 0.0f), math::set(0.05f, 0.05f, 0.05f, 0.12f )) )
	);
}




