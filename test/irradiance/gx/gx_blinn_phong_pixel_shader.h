#ifndef __GX_BLINN_PHONG_PIXEL_SHADER_H__
#define __GX_BLINN_PHONG_PIXEL_SHADER_H__

#include <cstdint>
#include <memory>

#include <d3d11/d3d11_error.h>
#include <d3d11/d3d11_pointers.h>

#include <math/math_matrix.h>

namespace gx
{
	class blinn_phong_pixel_shader final
    {
		public:
		explicit blinn_phong_pixel_shader ( ID3D11Device* device );

		operator const ID3D11PixelShader*() const
		{
			return m_shader.get();
		}

		d3d11::ipixelshader_ptr	m_shader;
		const void*					m_code;
		uint32_t					m_code_size;
    };
}

#endif