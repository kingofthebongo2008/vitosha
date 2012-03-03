#include "precompiled.h"
#include <gx/gx_execute_draw_calls_pipeline_node.h>

#include <gx/gx_draw_call_collector.h>
#include <gx/gx_draw_call_collector_context.h>
#include <gx/gx_draw_calls_pipeline_params.h>

#include <gx/gx_entity.h>

#include <gx/gx_pipeline_params.h>
#include <gx/gx_view.h>

namespace gx
{
    void*   execute_draw_calls_pipeline_node::do_process(void* input)
    {
        draw_calls_pipeline_params* in_params = reinterpret_cast<draw_calls_pipeline_params*> (input);

		m_render_context->begin_frame();


        m_render_context->end_frame();

        delete in_params;

        return nullptr;
    }
}