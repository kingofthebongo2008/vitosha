#ifndef __MESH_UTILS_H__
#define __MESH_UTILS_H__

#include <cstdint>


namespace mesh
{
	struct vertex
	{
		float x;
		float y;
		float z;
	};

	struct winded_edge
	{


	};

	struct half_edge
	{


	};

	enum validation_result : std::uint32_t
	{
		success = 0,
		invalid_number_of_triangles,
		degenerate_triangles_exist,
		duplicate_or_overlapping_triangles,
		mesh_is_not_manifold,
		inconsistent_triangle_orientation,
		self_intersecting_triangles,
		folded_edges
	};

	
	validation_result validate( const vertex* vertices, const std::uint32_t vertex_stride, const std::uint32_t vertex_count, void* indices, const std::uint32_t index_stride, const std::uint32_t index_count);

}



#endif
