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

    template < typename gbuffer_draw_call, typename depth_draw_call, typename material, typename depth_material > 
    class static_mesh_entity : public gx::entity
    {
        typedef static_mesh_entity<gbuffer_draw_call, depth_draw_call, material, depth_material > this_type;

        public:
        static_mesh_entity(gbuffer_draw_call draw_call, depth_draw_call draw_call2, material material, depth_material depth_material) : 
            m_draw_call(draw_call)
            , m_draw_call2(draw_call2)
            , m_material(material)
            , m_depth_material(depth_material)
        {

        }

        private:
        gbuffer_draw_call   m_draw_call;
        depth_draw_call     m_draw_call2;
        material	        m_material;
        depth_material      m_depth_material;

        void on_create_draw_calls( gx::draw_call_collector_context* context, gx::draw_call_collector* collector)
        {
            auto material_id = m_material.get_id();
            auto depth = gx::get_perspective_transform3_depth(context);

            auto key_gbuffer = gx::create_gbuffer_draw_call(material_id, depth, std::bind( &this_type::on_execute_draw_calls, this, std::placeholders::_1 )  );
            auto key_depth   = gx::create_depth_draw_call( depth, std::bind( &this_type::on_execute_depth_draw_calls, this, std::placeholders::_1 )  );

            collector->add_draw_call(key_gbuffer, context->m_entity_index ) ;
            collector->add_draw_call(key_depth, context->m_entity_index ) ;
        }

        void on_execute_draw_calls(gx::draw_call_context* context)
        {
            m_material.apply(context) ;
            m_draw_call.draw(context->m_device_context);
        }

        void on_execute_depth_draw_calls(gx::draw_call_context* context)
        {
            m_depth_material.apply(context);
            m_draw_call2.draw(context->m_device_context);
        }
    };
}

#endif