#ifndef __GXU_ENTITY_FACTORY_H__
#define __GXU_ENTITY_FACTORY_H__

#include <cstdint>
#include <memory>

#include <math/math_vector.h>

#include <gx/gx_indexed_draw_call.h>
#include <gx/gx_render_context.h>
#include <gxu/gxu_static_mesh_entity.h>


namespace gx
{
	class entity;
	class render_context;
}

namespace gxu
{
    gx::indexed_draw_call<2, gx::bit_16> create_lat_lon_sphere( ID3D11Device* device, float radius, uint32_t subdivision_count );

	std::tuple< d3d11::ibuffer_ptr, d3d11::ibuffer_ptr, d3d11::ibuffer_ptr, uint32_t > create_lat_lon_sphere_2( ID3D11Device* device, float radius, uint32_t subdivision_count );

	template <typename factory> inline std::shared_ptr<gx::entity> create_lat_lon_sphere_entity( ID3D11Device* device, const gx::shader_database* context, gx::indexed_draw_call<2, gx::bit_16> draw_call, math::float4 color )
	{
		return std::make_shared< gxu::static_mesh_entity<gx::indexed_draw_call<2, gx::bit_16>, factory::type> > ( draw_call , factory::create( context, color) );
	}

    template <typename factory> inline std::shared_ptr<gx::entity> create_lat_lon_sphere_entity( const gx::shader_database* context, gx::indexed_draw_call<2, gx::bit_16> draw_call, math::float4 kd, math::float4 ks_gloss )
	{
		return std::make_shared< gxu::static_mesh_entity<gx::indexed_draw_call<2, gx::bit_16>, factory::type> > ( draw_call , factory::create( context, kd, ks_gloss) );
	}

	template <typename factory> inline std::shared_ptr<gx::entity> create_lat_lon_sphere_entity( ID3D11Device* device, const gx::shader_database* context, float radius, uint32_t subdivision_count, math::float4 color )
    {
		return create_lat_lon_sphere_entity<factory> ( context, create_lat_lon_sphere( device, radius, subdivision_count) , color );
    }

    template <typename factory> inline std::shared_ptr<gx::entity> create_lat_lon_sphere_entity( ID3D11Device* device, const gx::shader_database* context, float radius, uint32_t subdivision_count, math::float4 kd, math::float4 ks_gloss )
    {
		return create_lat_lon_sphere_entity<factory> ( context, create_lat_lon_sphere( device, radius, subdivision_count) , kd, ks_gloss );
    }
}

#endif