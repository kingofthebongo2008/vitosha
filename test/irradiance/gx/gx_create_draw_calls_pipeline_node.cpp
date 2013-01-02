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
        auto in_params = reinterpret_cast<view_pipeline_params*> (input);

        draw_call_collector_context context = { &in_params->m_view_matrix, &in_params->m_projection_matrix };

        draw_call_collector collector(&m_draw_calls);

        collector.begin();

		collector.add_draw_call( gx::create_command_draw_call( gx::command::gbuffer_initialize ) );
		collector.add_draw_call( gx::create_command_draw_call( gx::command::gbuffer_finalize ) );

		collector.add_draw_call( gx::create_command_draw_call( gx::command::light_buffer_initialize ) );
		collector.add_draw_call( gx::create_command_draw_call( gx::command::light_buffer_finalize ) );

        collector.add_draw_call( gx::create_command_draw_call( gx::command::compose_light_buffer ) );

		collector.add_draw_call( gx::create_command_draw_call( gx::command::debug_initialize ) );
		collector.add_draw_call( gx::create_command_draw_call( gx::command::debug_finalize ) );

		for(auto i = static_cast<uint32_t> (0) ; i < in_params->m_data->size(); ++i)
        {
            context.m_wvp = &in_params->m_wvp_matrices->at(i);
            context.m_world_matrix = &in_params->m_world_matrices->at(i);
			context.m_entity_index = i;
            entity* enty = reinterpret_cast<entity*> ( in_params->m_data->at(i)) ;

            enty->create_draw_calls(&context, &collector);
        }

        collector.end();


        draw_calls_pipeline_params out_params = { 
                in_params->m_view_matrix, 
                in_params->m_projection_matrix, 
                in_params->m_inverse_view_matrix, 
                in_params->m_inverse_projection_matrix,

				in_params->m_view_port,
				in_params->m_zf,
				in_params->m_zn,

                in_params->m_wvp_matrices,
                in_params->m_world_matrices,
                &m_draw_calls,
                in_params->m_data
        };

        delete in_params;
        return new draw_calls_pipeline_params(out_params);
    }
}