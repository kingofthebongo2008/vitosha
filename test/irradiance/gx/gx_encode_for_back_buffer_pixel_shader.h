#ifndef __GX_ENCODE_FOR_BACK_BUFFER_PIXEL_SHADER_H__
#define __GX_ENCODE_FOR_BACK_BUFFER_PIXEL_SHADER_H__

#include <cstdint>
#include <memory>

#include <d3d11/d3d11_error.h>
#include <d3d11/d3d11_pointers.h>

namespace gx
{
    class encode_for_back_buffer_pixel_shader final
    {
        public:

        explicit encode_for_back_buffer_pixel_shader ( ID3D11Device* device );

        operator const ID3D11PixelShader*() const
        {
            return m_shader.get();
        }

        d3d11::ipixelshader_ptr m_shader;
        const void*             m_code;
        uint32_t                m_code_size;
    };
}

#endif