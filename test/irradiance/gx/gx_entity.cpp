#include "precompiled.h"

#include <gx/gx_entity.h>

#include <sys/sys_base.h>


namespace gx
{
    entity::entity()
    {

    }

    entity::~entity()
    {

    }

    void entity::on_create_draw_calls( draw_call_collector_context* context, draw_call_collector* collector)
    {
        UNUSED_PARAMETER(context);
        UNUSED_PARAMETER(collector);
    }

    void entity::on_create_shadow_draw_calls( draw_call_collector_context* context, draw_call_collector* collector)
    {
        UNUSED_PARAMETER(context);
        UNUSED_PARAMETER(collector);
    }

    void entity::on_create_visibility_draw_calls( draw_call_collector_context* context, draw_call_collector* collector)
    {
        UNUSED_PARAMETER(context);
        UNUSED_PARAMETER(collector);
    }

    void entity::on_create_depth_draw_calls(draw_call_collector_context* context,  draw_call_collector* collector)
    {
        UNUSED_PARAMETER(context);
        UNUSED_PARAMETER(collector);
    }

    void entity::on_execute_draw_calls(draw_call_context* context)
    {
        UNUSED_PARAMETER(context);
    }

}