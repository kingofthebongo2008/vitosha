#include "precompiled.h"
#include <gx/gx_execute_draw_calls_pipeline_node.h>

#include <gx/gx_color.h>
#include <gx/gx_draw_call_collector.h>
#include <gx/gx_draw_call_collector_context.h>
#include <gx/gx_draw_call_context.h>
#include <gx/gx_draw_calls_pipeline_params.h>

#include <gx/gx_entity.h>

#include <gx/gx_pipeline_params.h>
#include <gx/gx_screen_space_quad.h>
#include <gx/gx_thread_render_context.h>
#include <gx/gx_view.h>

#include <math/math_graphics.h>

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

	namespace
	{
		void process_command( gx::render_context* render_context, ID3D11DeviceContext* device_context, gx::command::type command ) 
		{
			switch (command)
			{
				case gx::command::gbuffer_initialize:
				{
					render_context->select_gbuffer(device_context);
					break;
				}

				case gx::command::gbuffer_finalize:
				{
					render_context->end_gbuffer(device_context);
					break;
				}

				case gx::command::debug_initialize:
				{
					render_context->select_back_buffer_target(device_context);
					break;
				}

				case gx::command::debug_finalize:
				{
					break;
				}

				default:
				{
					break;
				}
			}
		}
	}

	void*   execute_draw_calls_pipeline_node::do_process(void* input)
    {
        draw_calls_pipeline_params* in_params = reinterpret_cast<draw_calls_pipeline_params*> (input);

		ID3D11DeviceContext* device_context = m_render_context->get_immediate_context();

		m_render_context->clear_buffers(device_context);

		gx::draw_call_context draw_call_context = create_draw_call_context( device_context, in_params);

		for (uint32_t j = 0; j < in_params->m_draw_calls->size(); ++j )
		{
			gx::draw_call_info info = in_params->m_draw_calls->at(j);
			uint16_t		   index = info.m_entity_index;

			if (index != gx::draw_call_info::invalid_entity)
			{
				draw_call_context.m_wvp_matrix = &in_params->m_wvp_matrices->at(index);
				draw_call_context.m_world_matrix = &in_params->m_world_matrices->at(index);

				draw_call_context.m_entity_draw_call_index = info.m_entity_draw_call_index;
				gx::entity* enty = reinterpret_cast<entity*> ( in_params->m_data->at(index) );

				enty->execute_draw_calls(&draw_call_context);
			}
			else
			{
				gx::command::type command = info.m_key.get_command();
				process_command( m_render_context, device_context, command);
			}
		}

		//2. Render test quad
		
		m_render_context->select_back_buffer_target(device_context);
		
		math::vector_float4 color = color::blue();
		m_render_context->m_color_pixel_shader_cbuffer.update(device_context, color);
		m_render_context->m_color_pixel_shader_cbuffer.bind_as_pixel_constant_buffer(device_context);

		
		ID3D11ShaderResourceView* resources[1] = { 
													m_render_context->m_gbuffer_render_data.m_render_set.m_diffuse_view.get()
												};

		device_context->PSSetShaderResources(0, 1, resources );

		device_context->PSSetShader(m_render_context->m_color_texture_pixel_shader, nullptr, 0);
	
		math::matrix_float44 m1 = math::translation(-0.5f, -0.5f, 0.0f);
		math::matrix_float44 m2 = math::scaling(0.5f, 0.5f, 1.0f);
		math::matrix_float44 m3 = math::mul(m2, m1);
		draw_screen_space_quad(device_context, m_render_context, m3);
		

		delete in_params;
		return nullptr;
    }

    void*  execute_draw_calls_pipeline_node::do_process1(void* input)
    {
        auto in_params = reinterpret_cast<draw_calls_pipeline_params*> (input);

		auto begin	= m_render_context->begin();
		auto end	= m_render_context->end();
		auto it	= begin;
		auto thread_context_count = static_cast<uint32_t> (end - begin);

		auto draw_calls_per_thread = static_cast<uint32_t> ( in_params->m_draw_calls->size() / thread_context_count );

		std::vector< uint32_t > range_min;
		std::vector< uint32_t > range_max;

		range_min.reserve(32);
		range_max.reserve(32);

		for (auto i = static_cast<uint32_t> (0) ; i < thread_context_count; ++i)
		{
			range_min.push_back( i * draw_calls_per_thread );
			range_max.push_back( (i+1) * draw_calls_per_thread );
		}
		range_max[thread_context_count - 1 ] = static_cast<uint32_t> ( in_params->m_draw_calls->size() ) ;

		m_render_context->begin_frame();

		m_render_context->select_depth_pass( m_render_context->front()->get_device_context().get() );

		for (auto i = 0; it != end; ++i, ++it)
		{
			(*it)->begin_frame();

			auto start_item = range_min[i];
			auto end_item = range_max[i];

			auto device_context = (*it)->get_device_context().get();

			m_render_context->select_depth_pass( device_context );

			auto draw_call_context = create_draw_call_context( device_context, in_params);
														
			for (auto j = start_item; j < end_item; ++j )
			{
				draw_call_context.m_wvp_matrix = &in_params->m_wvp_matrices->operator[](j);
				draw_call_context.m_world_matrix = &in_params->m_world_matrices->operator[](j);

				auto key = in_params->m_draw_calls->operator[](j);
				auto enty = reinterpret_cast<entity*> ( in_params->m_data->operator[](key.m_entity_index) );

				enty->execute_draw_calls(&draw_call_context);
			}

			(*it)->end_frame();
		}

		m_render_context->end_frame();

		delete in_params;
		return nullptr;
    }
}