#ifndef __GX_COLOR_TEXTURE_PIXEL_SHADER_H__
#define __GX_COLOR_TEXTURE_PIXEL_SHADER_H__

#include <cstdint>

#include <d3d11/d3d11_error.h>
#include <d3d11/d3d11_pointers.h>


namespace gx
{
	class color_texture_pixel_shader final
    {
		public:

		explicit color_texture_pixel_shader ( ID3D11Device* device );

		operator const ID3D11PixelShader*() const
		{
			return m_shader.get();
		}

		d3d11::ipixelshader_ptr			m_shader;
		const void*						m_code;
		uint32_t						m_code_size;
    };
}

#endif