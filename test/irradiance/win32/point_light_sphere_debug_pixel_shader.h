#ifndef __GX_POINT_LIGHT_SPHERE_DEBUG_PIXEL_SHADER_H__
#define __GX_POINT_LIGHT_SPHERE_DEBUG_PIXEL_SHADER_H__

#include <cstdint>

#include <d3d11/d3d11_error.h>
#include <d3d11/d3d11_pointers.h>

class point_light_sphere_debug_pixel_shader 
{
	public:

	explicit point_light_sphere_debug_pixel_shader ( ID3D11Device* device );

	operator d3d11::ipixelshader_ptr()
	{
		return m_shader;
	}

	operator const ID3D11PixelShader*() const
	{
		return m_shader.get();
	}

	d3d11::ipixelshader_ptr	m_shader;
	const void*					m_code;
	uint32_t					m_code_size;
};

#endif