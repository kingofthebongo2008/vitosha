#ifndef __GX_COPY_DEPTH_PIXEL_SHADER_H__
#define __GX_COPY_DEPTH_PIXEL_SHADER_H__

#include <cstdint>

#include <dx11/dx11_error.h>
#include <dx11/dx11_pointers.h>


namespace gx
{
	class copy_depth_pixel_shader
    {
		public:

		explicit copy_depth_pixel_shader ( ID3D11Device* device );

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

		dx11::id3d11pixelshader_ptr		m_shader;
		const void*						m_code;
		uint32_t						m_code_size;
    };
}

#endif