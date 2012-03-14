#include "precompiled.h"
#include <gx/gx_create_draw_calls_pipeline_node.h>

#include <gx/gx_draw_call_collector.h>
#include <gx/gx_draw_call_collector_context.h>

#include <gx/gx_draw_calls_pipeline_params.h>

#include <gx/gx_entity.h>
#include <gx/gx_pipeline_params.h>
#include <gx/gx_view.h>

namespace gx
{
    create_draw_calls_pipeline_node::create_draw_calls_pipeline_node()
    {
        m_draw_calls.reserve(4096);
    }

    void*   create_draw_calls_pipeline_node::do_process(void* input)
    {
        view_pipeline_params* in_params = reinterpret_cast<view_pipeline_params*> (input);

        draw_call_collector_context context = { &in_params->m_view_matrix, &in_params->m_projection_matrix };

        draw_call_collector collector(&m_draw_calls);

        collector.begin();

        for(uint32_t i = 0 ; i < in_params->m_data->size(); ++i)
        {
            context.m_wvp = &in_params->m_wvp_matrices->operator[](i);
            context.m_world_matrix = &in_params->m_world_matrices->operator[](i);
			context.m_entity_index = i;
            entity* enty = reinterpret_cast<entity*> ( in_params->m_data->operator[](i) ) ;

            enty->create_draw_calls(&context, &collector);
        }

        collector.end();


        draw_calls_pipeline_params out_params = { 
                in_params->m_view_matrix, 
                in_params->m_projection_matrix, 
                in_params->m_inverse_view_matrix, 
                in_params->m_inverse_projection_matrix, 
                in_params->m_wvp_matrices,
                in_params->m_world_matrices,
                &m_draw_calls,
                in_params->m_data
        };

        delete in_params;
        return new draw_calls_pipeline_params(out_params);
    }
}