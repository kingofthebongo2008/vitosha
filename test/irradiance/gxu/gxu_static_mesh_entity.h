#ifndef __GXU_MESH_ENTITY_H__
#define __GXU_MESH_ENTITY_H__

#include <cstdint>

#include <math/math_graphics.h>

#include <gx/gx_draw_call_context.h>
#include <gx/gx_draw_call_collector.h>
#include <gx/gx_draw_call_collector_context.h>

#include <gx/gx_entity.h>
#include <gx/gx_indexed_draw_call.h>



namespace gxu
{
	struct null_material
	{
		void apply(gx::draw_call_context*)
		{

		}

		uint16_t get_id() const
		{
			return 0;
		}
	};

	template < typename material > 
    class static_mesh_entity : public gx::entity
    {
        public:
		static_mesh_entity(gx::indexed_draw_call draw_call, material material) : m_draw_call(draw_call), m_material(material)
		{

		}

        private:

		gx::indexed_draw_call   m_draw_call;
		material				m_material;

        void on_create_draw_calls( gx::draw_call_collector_context* context, gx::draw_call_collector* collector)
		{
			uint16_t material_id = m_material.get_id();
			
			//we assume that (0,0,0) is the center (pivot) of the object. 
			math::vector_float4 ety_pos_os = math::set(0.0f, 0.0f, 0.0f, 1.0f);
			math::vector_float4 ety_pos_ss = math::perspective_transform3(ety_pos_os, *context->m_wvp );
			
			gx::draw_call_key key = gx::create_gbuffer_opaque_key(material_id, ety_pos_ss.m128_f32[3]);
		    collector->add_draw_call(key, context->m_entity_index) ;
    	}

        void on_execute_draw_calls(gx::draw_call_context* context)
		{
			m_material.apply(context) ;
			m_draw_call.draw(context->m_device_context);
		}
    };
}

#endif