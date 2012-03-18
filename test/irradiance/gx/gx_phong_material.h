#ifndef __GX_PHONG_MATERIAL_H__
#define __GX_PHONG_MATERIAL_H__

#include <dx11/dx11_pointers.h>

#include <gx/gx_phong_pixel_shader.h>
#include <gx/gx_phong_vertex_shader.h>


namespace gx
{
	struct draw_call_context;

	struct phong_texture_set
	{
		phong_texture_set (
							ID3D11Device*				device,
							dx11::id3d11texture2d_ptr	diffuse,
							dx11::id3d11texture2d_ptr	normal,
							dx11::id3d11texture2d_ptr	specular
					   );

		dx11::id3d11texture2d_ptr			m_diffuse;
		dx11::id3d11texture2d_ptr			m_normal;
		dx11::id3d11texture2d_ptr			m_specular;

		dx11::id3d11shaderresourceview_ptr	m_diffuse_view;
		dx11::id3d11shaderresourceview_ptr	m_normal_view;
		dx11::id3d11shaderresourceview_ptr	m_specular_view;
	};

	struct phong_shader_set
	{
		phong_shader_set( phong_vertex_shader vertex_shader, 
						  phong_pixel_shader  pixel_shader, 
						  phong_vertex_shader_constant_buffer constant_buffer) :
						  m_vertex_shader (vertex_shader),
						  m_pixel_shader(pixel_shader),
						  m_vertex_constant_buffer(constant_buffer)
		{

		}							

		phong_vertex_shader					m_vertex_shader;
		phong_pixel_shader					m_pixel_shader;
		phong_vertex_shader_constant_buffer	m_vertex_constant_buffer;
	};

	class phong_material
	{
		public:
		phong_material::phong_material ( phong_texture_set texture_set, phong_shader_set shader_set );

		void apply(draw_call_context* draw_call_context);

		uint16_t get_id() const
		{
			return m_material_id;
		}

		private:

		phong_texture_set	m_texture_set;
		phong_shader_set	m_shader_set;
		uint16_t			m_material_id;
	};

	phong_material create_phong_material( ID3D11Device* device,  phong_texture_set texture_set ) ;
	phong_material create_phong_material( ID3D11Device*	device,  dx11::id3d11texture2d_ptr	diffuse, dx11::id3d11texture2d_ptr	normal, dx11::id3d11texture2d_ptr specular ) ;
}

#endif