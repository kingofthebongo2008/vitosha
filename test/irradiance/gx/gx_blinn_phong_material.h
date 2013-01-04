#ifndef __GX_BLINN_PHONG_MATERIAL_H__
#define __GX_BLINN_PHONG_MATERIAL_H__

#include <d3d11/d3d11_pointers.h>

#include <gx/gx_blinn_phong_pixel_shader.h>

#include <gx/gx_transform_position_normal_uv_vertex_shader.h>


namespace gx
{
	struct draw_call_context;

	struct blinn_phong_texture_set
	{
		blinn_phong_texture_set (
							ID3D11Device*			device,
							d3d11::itexture2d_ptr	diffuse,
							d3d11::itexture2d_ptr	normal,
							d3d11::itexture2d_ptr	specular
					   );

		d3d11::itexture2d_ptr			m_diffuse;
		d3d11::itexture2d_ptr			m_normal;
		d3d11::itexture2d_ptr			m_specular;

		d3d11::ishaderresourceview_ptr	m_diffuse_view;
		d3d11::ishaderresourceview_ptr	m_normal_view;
		d3d11::ishaderresourceview_ptr	m_specular_view;
	};

	struct blinn_phong_shader_set
	{
		blinn_phong_shader_set
                        ( 
                            transform_position_normal_uv_vertex_shader vertex_shader, 
                            blinn_phong_pixel_shader  pixel_shader, 
                            transform_position_normal_uv_vertex_shader_constant_buffer constant_buffer
                          ) :
						  m_vertex_shader (vertex_shader),
						  m_pixel_shader(pixel_shader),
						  m_vertex_constant_buffer(constant_buffer)
		{

		}							

		transform_position_normal_uv_vertex_shader					m_vertex_shader;
		blinn_phong_pixel_shader					                m_pixel_shader;
		transform_position_normal_uv_vertex_shader_constant_buffer	m_vertex_constant_buffer;
	};

	class blinn_phong_material
	{
		public:
		blinn_phong_material::blinn_phong_material ( blinn_phong_texture_set texture_set, blinn_phong_shader_set shader_set );

		void apply(draw_call_context* draw_call_context);

		uint16_t get_id() const
		{
			return m_material_id;
		}

		private:

		blinn_phong_texture_set	m_texture_set;
		blinn_phong_shader_set	m_shader_set;
		uint16_t			    m_material_id;
	};

	blinn_phong_material create_blinn_phong_material( ID3D11Device* device,  blinn_phong_texture_set texture_set ) ;
	blinn_phong_material create_blinn_phong_material( ID3D11Device*	device,  d3d11::itexture2d_ptr	diffuse, d3d11::itexture2d_ptr	normal, d3d11::itexture2d_ptr specular ) ;
}

#endif