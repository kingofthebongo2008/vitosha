#ifndef _ROOM_H__
#define _ROOM_H__

#include <algorithm>
#include <memory>
#include <vector>
#include <istream>
#include <iterator>

#include <math/math_vector.h>
#include <d3d11/d3d11_pointers.h>

#include <gx/gx_blinn_phong_shift_invariant_material.h>
#include <gx/gx_entity.h>
#include <gx/gx_gbuffer_dt_ng_sc_gc_material.h>
#include <gx/gx_indexed_draw_call.h>
#include <gx/gx_transform_position_normal_uv_vertex_shader.h>


namespace gx
{
    class shader_database;
}

class room_entity : public gx::entity
{
	public:

	struct material
	{
		math::float4 m_diffuse;
		math::float4 m_specular;
	};

	typedef gx::indexed_draw_call<2,gx::bit_32> draw_call;


    template < typename const_iterator_draw_calls> room_entity
		( 
			const_iterator_draw_calls								draw_calls_begin,
			const_iterator_draw_calls								draw_calls_end,
			gx::blinn_phong_shift_invairant_material				material,
			const std::vector<room_entity::material>&				materials,
			const std::vector<gx::gbuffer_dt_ng_sc_gc_material>&	textured_materials
		) :
			m_material(material)
			, m_materials(std::move(materials))
			, m_draw_calls(draw_calls_begin, draw_calls_end)
			, m_textured_materials(std::move(textured_materials))
    {
    }

	template < typename const_iterator_draw_calls> room_entity
		( 
			const_iterator_draw_calls								draw_calls_begin,
			const_iterator_draw_calls								draw_calls_end,
			gx::blinn_phong_shift_invairant_material				material,
			std::vector<room_entity::material>&&					materials,
			std::vector<gx::gbuffer_dt_ng_sc_gc_material>&&			textured_materials
		) :
			m_material(material)
			, m_materials(std::move(materials))
			, m_draw_calls(draw_calls_begin, draw_calls_end)
			, m_textured_materials(std::move(textured_materials))
	{
	}

    void on_create_draw_calls( gx::draw_call_collector_context* context, gx::draw_call_collector* collector);
	void on_execute_draw_calls( gx::draw_call_context* draw_call_context );

	private:
	gx::blinn_phong_shift_invairant_material		m_material;

	std::vector<material>							m_materials;
	std::vector<draw_call>							m_draw_calls;
	std::vector<gx::gbuffer_dt_ng_sc_gc_material>	m_textured_materials;

	void											draw(gx::draw_call_context*, uint32_t material_index, uint32_t draw_call_index);
	void											draw_textured_material(gx::draw_call_context* draw_call_context, uint32_t material_index, uint32_t draw_call_index);
};

std::shared_ptr<room_entity> create_room_entity( ID3D11Device* device, const gx::shader_database* context, std::istream& in);


#endif
