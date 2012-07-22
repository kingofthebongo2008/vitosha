#ifndef __GX_LAMBERT_PIXEL_SHADER_H__
#define __GX_LAMBERT_PIXEL_SHADER_H__

#include <cstdint>
#include <memory>

#include <dx11/dx11_error.h>
#include <dx11/dx11_pointers.h>

#include <math/math_matrix.h>

namespace gx
{
	class lambert_pixel_shader
    {
		public:
		explicit lambert_pixel_shader ( ID3D11Device* device );

		operator const ID3D11PixelShader*() const
		{
			return m_shader.get();
		}

		dx11::id3d11pixelshader_ptr							 m_shader;
		const void*											 m_code;
		uint32_t										 m_code_size;
    };
}

#endif