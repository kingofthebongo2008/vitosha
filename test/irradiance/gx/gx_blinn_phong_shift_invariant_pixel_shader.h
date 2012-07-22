#ifndef __GX_BLINN_PHONG_SHIFT_INVARIANT_PIXEL_SHADER_H__
#define __GX_BLINN_PHONG_SHIFT_INVARIANT_PIXEL_SHADER_H__

#include <cstdint>
#include <memory>

#include <dx11/dx11_error.h>
#include <dx11/dx11_pointers.h>

#include <math/math_matrix.h>

#include <gx/gx_constant_buffer_helper.h>

namespace gx
{
	class __declspec(align(16)) blinn_phong_shift_invariant_pixel_shader_constant_buffer 
	{
		public:

		explicit blinn_phong_shift_invariant_pixel_shader_constant_buffer ( ID3D11Device* device );

        math::float4 get_kd() const
		{
			return m_kd;
		}

		void set_kd(math::float4 value)
		{
			m_kd = value;
		}

		math::float4 get_ks_gloss() const
		{
			return m_ks_gloss;
		}

		void set_ks_gloss(math::float4 value)
		{
			m_ks_gloss = value;
		}


		void flush ( ID3D11DeviceContext* context )
		{
			gx::constant_buffer_update(context, m_buffer.get(), (void*) &m_kd, size() );
		}

		void bind_as_vertex_constant_buffer(ID3D11DeviceContext* context)
		{
			context->VSSetConstantBuffers(0, 1, dx11::get_pointer(m_buffer));
		}

		void bind_as_pixel_constant_buffer(ID3D11DeviceContext* context, uint32_t slot)
		{
			context->PSSetConstantBuffers(slot, 1, dx11::get_pointer(m_buffer));
		}

		void bind_as_pixel_constant_buffer(ID3D11DeviceContext* context)
		{
			bind_as_pixel_constant_buffer(context, 0);
		}

		operator ID3D11Buffer*()
		{
			return m_buffer.get();
		}

		operator const ID3D11Buffer*() const
		{
			return m_buffer.get();
		}

		size_t size() const
		{
			return sizeof(m_kd) + sizeof(m_ks_gloss);
		}

		public:

		math::float4		m_kd;			//diffuse albedo
		math::float4		m_ks_gloss;		//specular color + gloss
		dx11::id3d11buffer_ptr	m_buffer;
	};

	class blinn_phong_shift_invariant_pixel_shader  final
    {
		public:

		explicit blinn_phong_shift_invariant_pixel_shader ( ID3D11Device* device );

		operator const ID3D11PixelShader*() const
		{
			return m_shader.get();
		}

		dx11::id3d11pixelshader_ptr							 m_shader;
		const void*											 m_code;
		uint32_t											 m_code_size;
    };
}

#endif