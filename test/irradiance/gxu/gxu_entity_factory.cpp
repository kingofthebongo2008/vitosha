#include "precompiled.h"

#include <gxu/gxu_entity_factory.h>

#include <gx/gx_indexed_draw_call.h>
#include <gxu/gxu_static_mesh_entity.h>

namespace gxu
{
    gx::indexed_draw_call create_lat_lon_sphere( ID3D11Device* device , float radius, uint32_t subdivision_count )
    {
        D3D11_BUFFER_DESC desc = {};

        dx11::id3d11buffer_ptr positions;
        dx11::id3d11buffer_ptr normals_uvs;
        dx11::id3d11buffer_ptr indices;

        gx::indexed_draw_call::index_info info = {};

        return gx::indexed_draw_call(info, positions, normals_uvs, indices);
    }

    std::shared_ptr<gx::entity> create_lat_lon_sphere_entity( ID3D11Device* device, float radius, uint32_t subdivision_count )
    {
        return std::make_shared< gxu::static_mesh_entity >( create_lat_lon_sphere(device, radius, subdivision_count) );
    }
 }