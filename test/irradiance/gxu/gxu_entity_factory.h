#ifndef __GXU_ENTITY_FACTORY_H__
#define __GXU_ENTITY_FACTORY_H__

#include <cstdint>
#include <memory>

struct ID3D11Device;

namespace gx
{
    class indexed_draw_call;
    class entity;
}

namespace gxu
{
    gx::indexed_draw_call create_lat_lon_sphere( ID3D11Device* device, float radius, uint32_t subdivision_count );
    std::shared_ptr<gx::entity> create_lat_lon_sphere_entity( ID3D11Device* device, float radius, uint32_t subdivision_count );
}

#endif