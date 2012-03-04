#include "precompiled.h"
#include <gx/gx_execute_draw_calls_pipeline_node.h>

#include <gx/gx_draw_call_collector.h>
#include <gx/gx_draw_call_collector_context.h>
#include <gx/gx_draw_call_context.h>
#include <gx/gx_draw_calls_pipeline_params.h>

#include <gx/gx_entity.h>

#include <gx/gx_pipeline_params.h>

#include <gx/gx_thread_render_context.h>

#include <gx/gx_view.h>

namespace gx
{
	namespace
	{
		inline gx::draw_call_context create_draw_call_context(ID3D11DeviceContext* device_context, draw_calls_pipeline_params* in_params)
		{
			gx::draw_call_context draw_call_context =

			{ 
														device_context, 
														&in_params->m_view_matrix,
														&in_params->m_projection_matrix,
														&in_params->m_inverse_view_matrix,
														&in_params->m_inverse_projection_matrix
			};

			return draw_call_context;
		}

	}

    void*   execute_draw_calls_pipeline_node::do_process(void* input)
    {
        draw_calls_pipeline_params* in_params = reinterpret_cast<draw_calls_pipeline_params*> (input);

		gx::render_context::thread_render_context_container::iterator begin	= m_render_context->begin();
		gx::render_context::thread_render_context_container::iterator end	= m_render_context->end();
		gx::render_context::thread_render_context_container::iterator it	= begin;
		uint32_t thread_context_count = static_cast<uint32_t> (end - begin);

		uint32_t draw_calls_per_thread = static_cast<uint32_t> ( in_params->m_draw_calls->size() / thread_context_count );

		std::vector< uint32_t > range_min;
		std::vector< uint32_t > range_max;

		range_min.reserve(32);
		range_max.reserve(32);

		for (uint32_t i = 0; i < thread_context_count; ++i)
		{
			range_min.push_back( i * draw_calls_per_thread );
			range_max.push_back( (i+1) * draw_calls_per_thread );
		}
		range_max[thread_context_count - 1 ] = static_cast<uint32_t> ( in_params->m_draw_calls->size() ) ;


		m_render_context->begin_frame();

		for (uint32_t i = 0; it != end; ++i, ++it)
		{
			(*it)->begin_frame();

			uint32_t start_item = range_min[i];
			uint32_t end_item = range_max[i];

			ID3D11DeviceContext* device_context = (*it)->get_device_context().get();

			gx::draw_call_context draw_call_context = create_draw_call_context( device_context, in_params);
														
			for (uint32_t j = start_item; j < end_item; ++j )
			{
				draw_call_context.m_pvw_matrix = &in_params->m_pvw_matrices->operator[](j);

				gx::draw_call_info key = in_params->m_draw_calls->operator[](j);

				gx::entity* enty = reinterpret_cast<entity*> ( in_params->m_data->operator[](key.m_entity_index) );

				enty->execute_draw_calls(&draw_call_context);
			}

			(*it)->end_frame();
		}

		m_render_context->end_frame();

		delete in_params;
		return nullptr;
    }
}