#ifndef __GX_LAMBERT_MATERIAL_H__
#define __GX_LAMBERT_MATERIAL_H__

#include <d3d11/d3d11_pointers.h>

namespace gx
{
	struct draw_call_context;

	struct lambert_texture_set
	{
		lambert_texture_set (
							ID3D11Device*				device,
							d3d11::itexture2d_ptr	diffuse,
							d3d11::itexture2d_ptr	normal
					   );

		d3d11::itexture2d_ptr			m_diffuse;
		d3d11::itexture2d_ptr			m_normal;

		d3d11::ishaderresourceview_ptr	m_diffuse_view;
		d3d11::ishaderresourceview_ptr	m_normal_view;
	};

	class lambert_material
	{
		public:
		lambert_material ( lambert_texture_set texture_set ) : m_texture_set(texture_set)
		{

		}

		void apply(draw_call_context* draw_call_context);

		private:

		lambert_texture_set m_texture_set;
	};

}

#endif