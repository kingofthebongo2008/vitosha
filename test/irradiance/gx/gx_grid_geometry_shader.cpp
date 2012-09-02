#include "precompiled.h"
#include <gx/gx_grid_geometry_shader.h>

namespace gx
{
	#include "gx_shader_grid_gs_compiled.hlsl"

	grid_geometry_shader::grid_geometry_shader ( ID3D11Device* device )
	{
		dx::throw_if_failed<d3d11::create_geometry_shader> (device->CreateGeometryShader( gx_shader_grid_gs, sizeof(gx_shader_grid_gs), nullptr, dx::get_pointer(m_shader)));
		m_code = &gx_shader_grid_gs[0];
		m_code_size = sizeof(gx_shader_grid_gs);
	}
}