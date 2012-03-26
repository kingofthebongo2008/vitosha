#ifndef __GXU_ENTITY_FACTORY_H__
#define __GXU_ENTITY_FACTORY_H__

#include <cstdint>
#include <memory>

#include <math/math_vector.h>

struct ID3D11Device;

namespace gx
{
	class entity;
    class indexed_draw_call;
	class render_context;
}

namespace gxu
{
    gx::indexed_draw_call create_lat_lon_sphere( ID3D11Device* device, float radius, uint32_t subdivision_count );
	std::shared_ptr<gx::entity> create_lat_lon_sphere_entity(  gx::render_context* context, gx::indexed_draw_call draw_call, math::vector_float4 color );
	std::shared_ptr<gx::entity> create_lat_lon_sphere_entity(  gx::render_context* context, float radius, uint32_t subdivision_count, math::vector_float4 color );
}

#endif