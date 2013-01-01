#ifndef __GX_LAMBERT_SHIFT_INVARIANT_MATERIAL_H__
#define __GX_LAMBERT_SHIFT_INVARIANT_MATERIAL_H__

#include <d3d11/d3d11_pointers.h>
#include <math/math_vector.h>

#include <gx/gx_lambert_shift_invariant_pixel_shader.h>
#include <gx/gx_transform_position_normal_uv_vertex_shader.h>

namespace gx
{
	struct draw_call_context;
	class  render_context;

	class __declspec(align(16)) lambert_shift_invairant_material
	{
		public:

		lambert_shift_invairant_material::lambert_shift_invairant_material
        ( 
								        const transform_position_normal_uv_vertex_pipeline          vertex_pipeline,
										const lambert_shift_invariant_pixel_shader					pixel_shader,
										const lambert_shift_invariant_pixel_shader_constant_buffer	pixel_cbuffer,

										math::float4	diffuse_albedo

        );

		void apply(draw_call_context* draw_call_context);

        uint16_t get_id() const
		{
			return m_material_id;
		}

		math::float4 get_diffuse_albedo() const
		{
			return m_pixel_cbuffer.get_diffuse_albedo();
		}

		void set_diffuse_albedo(math::float4 value)
		{
			m_pixel_cbuffer.set_diffuse_albedo(value);
		}

		private:
        transform_position_normal_uv_vertex_pipeline            m_vertex_pipeline;

        lambert_shift_invariant_pixel_shader					m_pixel_shader;
		lambert_shift_invariant_pixel_shader_constant_buffer	m_pixel_cbuffer;

        uint16_t										        m_material_id;
	};

	lambert_shift_invairant_material create_lambert_shift_invairant_material( render_context* context, math::float4 diffuse_albedo );

	struct lambert_shift_invairant_material_factory
	{
		typedef lambert_shift_invairant_material type;

		static lambert_shift_invairant_material create(render_context* context, math::float4 color)
		{
			return create_lambert_shift_invairant_material(context, color);
		}
	};

}

#endif