#ifndef __GX_TRANSFORM_POSITION_NORMAL_UV_SHADER_H__
#define __GX_TRANSFORM_POSITION_NORMAL_UV_SHADER_H__

#include <cstdint>
#include <tuple>

#include <d3d11/d3d11_pointers.h>

#include <math/math_matrix.h>

#include <gx/gx_constant_buffer_helper.h>

namespace gx
{
    class transform_position_normal_uv_vertex_shader_constant_buffer 
    {
        public:

        explicit transform_position_normal_uv_vertex_shader_constant_buffer ( ID3D11Device* device );

        void set_w(math::float4x4 value)
        {
            m_w = value;
        }

        void set_normal_transform(math::float4x4 value)
        {
            m_normal_transform = value;
        }

        void flush ( ID3D11DeviceContext* context )
        {
            constant_buffer_update(context, m_buffer.get(), (void*) &m_w, size() );
        }

        void bind_as_vertex_constant_buffer(ID3D11DeviceContext* context)
        {
            context->VSSetConstantBuffers(1, 1, dx::get_pointer(m_buffer));
        }

        inline size_t size() const
        {
             return sizeof(m_w) + sizeof(m_normal_transform);
        }


        operator ID3D11Buffer*()
        {
            return m_buffer.get();
        }

        operator const ID3D11Buffer*() const
        {
            return m_buffer.get();
        }

        private:

        d3d11::ibuffer_ptr  m_buffer;

        math::float4x4      m_w;
        math::float4x4      m_normal_transform;
    };

    class transform_position_normal_uv_vertex_shader
    {
        public:

        explicit transform_position_normal_uv_vertex_shader ( ID3D11Device* device );

        operator const ID3D11VertexShader*() const
        {
            return m_shader.get();
        }

        d3d11::ivertexshader_ptr    m_shader;
        const void*                 m_code;
        uint32_t                    m_code_size;
    };

    class transform_position_normal_uv_input_layout
    {
        public:

        transform_position_normal_uv_input_layout( ID3D11Device* device, transform_position_normal_uv_vertex_shader* shader);

        operator d3d11::iinputlayout_ptr()
        {
            return m_input_layout;
        }

        operator ID3D11InputLayout*()
        {
            return m_input_layout.get();
        }

        operator const ID3D11InputLayout*() const
        {
            return m_input_layout.get();
        }

        d3d11::iinputlayout_ptr	m_input_layout;
    };

    typedef std::tuple < transform_position_normal_uv_vertex_shader, transform_position_normal_uv_vertex_shader_constant_buffer,  transform_position_normal_uv_input_layout  > transform_position_normal_uv_vertex_pipeline;
}

#endif