#ifndef __GX_GRID_GEOMETRY_SHADER_H__
#define __GX_GRID_GEOMETRY_SHADER_H__

#include <cstdint>
#include <memory>

#include <d3d11/d3d11_error.h>
#include <d3d11/d3d11_pointers.h>

#include <math/math_matrix.h>

#include <gx/gx_constant_buffer_helper.h>

namespace gx
{
	class grid_geometry_shader
    {
		public:

		explicit grid_geometry_shader ( ID3D11Device* device );

		operator d3d11::igeometryshader_ptr()
		{
			return m_shader;
		}

		operator ID3D11GeometryShader*()
		{
			return m_shader.get();
		}

		operator const ID3D11GeometryShader*() const
		{
			return m_shader.get();
		}

		d3d11::igeometryshader_ptr  m_shader;
		const void*						m_code;
		uint32_t						m_code_size;
    };
}

#endif