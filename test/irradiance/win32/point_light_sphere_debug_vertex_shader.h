#ifndef __GX_POINT_LIGHT_SPHERE_DEBUG_VERTEX_SHADER_H__
#define __GX_POINT_LIGHT_SPHERE_DEBUG_VERTEX_SHADER_H__

#include <cstdint>

#include <dx11/dx11_error.h>
#include <dx11/dx11_pointers.h>

class point_light_sphere_debug_vertex_shader
{
	public:

	explicit point_light_sphere_debug_vertex_shader ( ID3D11Device* device );

	operator dx11::id3d11vertexshader_ptr()
	{
		return m_shader;
	}

	operator ID3D11VertexShader*()
	{
		return m_shader.get();
	}

	operator const ID3D11VertexShader*() const
	{
		return m_shader.get();
	}

	dx11::id3d11vertexshader_ptr	m_shader;
	const void*						m_code;
	uint32_t						m_code_size;
};

#endif