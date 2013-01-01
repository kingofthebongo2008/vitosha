#ifndef _ROOM_H__
#define _ROOM_H__

#include <memory>
#include <vector>
#include <istream>

#include <math/math_vector.h>
#include <d3d11/d3d11_pointers.h>

#include <gx/gx_blinn_phong_shift_invariant_material.h>
#include <gx/gx_entity.h>
#include <gx/gx_indexed_draw_call.h>
#include <gx/gx_transform_position_normal_uv_vertex_shader.h>


namespace gx
{
    class shader_database;
}

class room_entity : public gx::entity
{
	public:
    room_entity
		( 
			gx::indexed_draw_call<2,gx::bit_32>								draw_call,
			gx::blinn_phong_shift_invairant_material						material
		) :

			m_draw_call(draw_call)
		,	m_material(material)
    {
    
    }

    void on_create_draw_calls( gx::draw_call_collector_context* context, gx::draw_call_collector* collector);
	void on_execute_draw_calls( gx::draw_call_context* draw_call_context );

	private:

	gx::indexed_draw_call<2,gx::bit_32>				m_draw_call;
	gx::blinn_phong_shift_invairant_material		m_material;
};

std::shared_ptr<room_entity> create_room_entity( ID3D11Device* device, const gx::shader_database* context, std::istream& in);


#endif
