#ifndef __GX_LAMBERT_CONSTANT_MATERIAL_H__
#define __GX_LAMBERT_CONSTANT_MATERIAL_H__

#include <dx11/dx11_pointers.h>
#include <math/math_vector.h>

#include <gx/gx_lambert_vertex_shader.h>
#include <gx/gx_lambert_constant_pixel_shader.h>


namespace gx
{
	struct draw_call_context;
	class  render_context;

	class __declspec(align(16)) lambert_constant_material
	{
		public:

		lambert_constant_material::lambert_constant_material ( 
										lambert_vertex_shader							vertex_shader,
										lambert_vertex_shader_constant_buffer			vertex_cbuffer,
										dx11::id3d11inputlayout_ptr						input_layout,
										lambert_constant_pixel_shader					pixel_shader,
										lambert_constant_pixel_shader_constant_buffer	pixel_cbuffer,
										math::vector_float4	diffuse_albedo );

		void apply(draw_call_context* draw_call_context);

		uint16_t get_id() const
		{
			return m_material_id;
		}

		math::vector_float4 get_diffuse_albedo() const
		{
			return m_pixel_cbuffer.get_diffuse_albedo();
		}

		void set_diffuse_albedo(math::vector_float4 value)
		{
			m_pixel_cbuffer.set_diffuse_albedo(value);
		}

		private:

		lambert_vertex_shader							m_vertex_shader;
		lambert_vertex_shader_constant_buffer			m_vertex_cbuffer;

		dx11::id3d11inputlayout_ptr						m_input_layout;
		
		lambert_constant_pixel_shader					m_pixel_shader;
		lambert_constant_pixel_shader_constant_buffer	m_pixel_cbuffer;

		uint16_t										m_material_id;

	};

	lambert_constant_material create_lambert_constant_material( render_context* context, math::vector_float4 diffuse_albedo );


	struct lambert_constant_material_factory
	{
		typedef lambert_constant_material type;

		static lambert_constant_material create(render_context* context, math::vector_float4 color)
		{
			return create_lambert_constant_material(context, color);
		}
	};

}

#endif