#include "precompiled.h"

#include <gxu/gxu_static_mesh_entity.h>


#include <gx/gx_draw_call_context.h>
#include <gx/gx_draw_call_collector.h>
#include <gx/gx_draw_call_collector_context.h>

namespace gxu
{
    static_mesh_entity::static_mesh_entity( gx::indexed_draw_call draw_call ) :
        m_draw_call(draw_call)
    {

    }

    void static_mesh_entity::on_create_draw_calls( gx::draw_call_collector_context* context, gx::draw_call_collector* collector)
    {
        gx::draw_call_key key = gx::create_opaque_key(0, 0.0f);
        collector->add_draw_call(key, context->m_entity_index);
    }

    void static_mesh_entity::on_execute_draw_calls(gx::draw_call_context* context)
    {
        //set vertex, pixel shader, upload constants and issue a draw call
		m_draw_call.draw(context->m_device_context);
    }
}