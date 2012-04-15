#include "precompiled.h"
#include "point_light_sphere_debug_vertex_shader.h"

#include "shader_point_light_sphere_debug_vs_compiled.hlsl"

point_light_sphere_debug_vertex_shader::point_light_sphere_debug_vertex_shader ( ID3D11Device* device )
{
	dx11::throw_if_failed<dx11::create_pixel_shader> (device->CreateVertexShader( shader_point_light_sphere_debug_vs, sizeof(shader_point_light_sphere_debug_vs), nullptr, dx11::get_pointer(m_shader)));
	m_code = &shader_point_light_sphere_debug_vs[0];
	m_code_size = sizeof(shader_point_light_sphere_debug_vs);
}
