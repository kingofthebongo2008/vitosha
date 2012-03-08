#ifndef __GX_SCREEN_SPACE_PIXEL_SHADER_H__
#define __GX_SCREEN_SPACE_PIXEL_SHADER_H__

#include <cstdint>
#include <memory>

#include <dx11/dx11_error.h>
#include <dx11/dx11_pointers.h>

#include <math/math_matrix.h>

namespace gx
{
	class screen_space_pixel_shader
    {
		public:
		explicit screen_space_pixel_shader ( dx11::id3d11device_ptr device );

		operator dx11::id3d11pixelshader_ptr()
		{
			return m_shader;
		}

		operator ID3D11PixelShader*()
		{
			return m_shader.get();
		}

		operator const ID3D11PixelShader*() const
		{
			return m_shader.get();
		}

		dx11::id3d11pixelshader_ptr							 m_shader;
		const void*											 m_code;
		std::uint32_t										 m_code_size;
    };
}

#endif