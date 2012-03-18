#ifndef __GX_LAMBERT_MATERIAL_H__
#define __GX_LAMBERT_MATERIAL_H__

#include <dx11/dx11_pointers.h>

namespace gx
{
	struct draw_call_context;

	struct lambert_texture_set
	{
		lambert_texture_set (
							ID3D11Device*				device,
							dx11::id3d11texture2d_ptr	diffuse,
							dx11::id3d11texture2d_ptr	normal
					   );

		dx11::id3d11texture2d_ptr			m_diffuse;
		dx11::id3d11texture2d_ptr			m_normal;

		dx11::id3d11shaderresourceview_ptr	m_diffuse_view;
		dx11::id3d11shaderresourceview_ptr	m_normal_view;
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