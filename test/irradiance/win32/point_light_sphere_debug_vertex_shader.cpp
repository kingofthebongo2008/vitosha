#include "precompiled.h"
#include "point_light_sphere_debug_vertex_shader.h"

point_light_sphere_debug_vertex_shader_constant_buffer::point_light_sphere_debug_vertex_shader_constant_buffer ( ID3D11Device* device )
{
	D3D11_BUFFER_DESC desc = {};

	desc.ByteWidth = static_cast<UINT> ( size() );
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	dx11::throw_if_failed<dx11::create_buffer_exception> (device->CreateBuffer(&desc, nullptr, dx11::get_pointer(m_buffer) ));
}


#include "shader_point_light_sphere_debug_vs_compiled.hlsl"


point_light_sphere_debug_vertex_shader::point_light_sphere_debug_vertex_shader ( ID3D11Device* device )
{
	dx11::throw_if_failed<dx11::create_pixel_shader> (device->CreateVertexShader( shader_point_light_sphere_debug_vs, sizeof(shader_point_light_sphere_debug_vs), nullptr, dx11::get_pointer(m_shader)));
	m_code = &shader_point_light_sphere_debug_vs[0];
	m_code_size = sizeof(shader_point_light_sphere_debug_vs);
}
