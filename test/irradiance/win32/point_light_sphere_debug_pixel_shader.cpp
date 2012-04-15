#include "precompiled.h"
#include "point_light_sphere_debug_pixel_shader.h"

#include "shader_point_light_sphere_debug_ps_compiled.hlsl"

point_light_sphere_debug_pixel_shader::point_light_sphere_debug_pixel_shader ( ID3D11Device* device )
{
	dx11::throw_if_failed<dx11::create_pixel_shader> (device->CreatePixelShader( shader_point_light_sphere_debug_ps, sizeof(shader_point_light_sphere_debug_ps), nullptr, dx11::get_pointer(m_shader)));
	m_code = &shader_point_light_sphere_debug_ps[0];
	m_code_size = sizeof(shader_point_light_sphere_debug_ps);
}
