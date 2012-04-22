#ifndef __GX_BLINN_PHONG_SHIFT_INVARIANT_MATERIAL_H__
#define __GX_BLINN_PHONG_SHIFT_INVARIANT_MATERIAL_H__

#include <dx11/dx11_pointers.h>
#include <math/math_vector.h>

#include <gx/gx_blinn_phong_shift_invariant_pixel_shader.h>
#include <gx/gx_transform_position_normal_uv_vertex_shader.h>

namespace gx
{
	struct draw_call_context;
	class  render_context;

	class __declspec(align(16)) blinn_phong_shift_invairant_material
	{
		public:
		blinn_phong_shift_invairant_material::blinn_phong_shift_invairant_material
										( 
								            transform_position_normal_uv_vertex_pipeline                vertex_pipeline,

											blinn_phong_shift_invariant_pixel_shader					pixel_shader,
											blinn_phong_shift_invariant_pixel_shader_constant_buffer	pixel_cbuffer,

											math::vector_float4	kd,
											math::vector_float4	ks_gloss

										);

		void apply(draw_call_context* draw_call_context);

		uint16_t get_id() const
		{
			return m_material_id;
		}

		math::vector_float4 get_kd() const
		{
			return m_pixel_cbuffer.get_kd();
		}

		void set_kd(math::vector_float4 value)
		{
			m_pixel_cbuffer.set_kd(value);
		}

		math::vector_float4 get_ks_gloss() const
		{
			return m_pixel_cbuffer.get_kd();
		}

		void set_ks_gloss(math::vector_float4 value)
		{
			m_pixel_cbuffer.set_ks_gloss(value);
		}

		private:

        transform_position_normal_uv_vertex_pipeline                m_vertex_pipeline;
		
		blinn_phong_shift_invariant_pixel_shader					m_pixel_shader;
		blinn_phong_shift_invariant_pixel_shader_constant_buffer	m_pixel_cbuffer;

		uint16_t													m_material_id;

	};

	blinn_phong_shift_invairant_material create_blinn_phong_shift_invairant_material( render_context* context, math::vector_float4 kd, math::vector_float4 ks_gloss );

	struct blinn_phong_shift_invairant_material_factory
	{
		typedef blinn_phong_shift_invairant_material type;

		static blinn_phong_shift_invairant_material create(render_context* context, math::vector_float4 kd, math::vector_float4 ks_gloss )
		{
			return create_blinn_phong_shift_invairant_material(context, kd, ks_gloss);
		}
	};
}

#endif