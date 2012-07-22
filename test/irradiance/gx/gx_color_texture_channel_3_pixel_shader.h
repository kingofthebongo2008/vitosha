#ifndef __GX_COLOR_TEXTURE_CHANNEL_3_PIXEL_SHADER_H__
#define __GX_COLOR_TEXTURE_CHANNEL_3_PIXEL_SHADER_H__

#include <cstdint>

#include <dx11/dx11_error.h>
#include <dx11/dx11_pointers.h>


namespace gx
{
	class color_texture_channel_3_pixel_shader final
    {
		public:

		explicit color_texture_channel_3_pixel_shader ( ID3D11Device* device );

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