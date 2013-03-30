#include "precompiled.h"
#include <gx/gx_execute_draw_calls_pipeline_node.h>

#include <gx/gx_debug_draw.h>
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
        gx::per_view_data create_per_view_data( const draw_calls_pipeline_params& in_params )
        {
            gx::per_view_data  per_view_data =
            {
                in_params.m_view_matrix,
                in_params.m_projection_matrix,
                in_params.m_inverse_view_matrix,
                in_params.m_inverse_projection_matrix,
                in_params.m_view_port,
                in_params.m_zn,
                in_params.m_zf
            };

            return std::move(per_view_data);
        }

        void process_command( gx::render_context* render_context, ID3D11DeviceContext* device_context, gx::command::type command, const draw_calls_pipeline_params& in_params ) 
        {
            switch (command)
            {
                case gx::command::depth_initialize:
                {
                    render_context->select_depth_pass(device_context, create_per_view_data(in_params));
                    break;
                }

                case gx::command::depth_finalize:
                {
                    render_context->end_depth_pass(device_context, create_per_view_data(in_params));
                    break;
                }

                case gx::command::gbuffer_initialize:
                {
                    render_context->select_gbuffer(device_context, create_per_view_data(in_params));
                    break;
                }

                case gx::command::gbuffer_finalize:
                {
                    render_context->end_gbuffer(device_context, create_per_view_data(in_params));
                    break;
                }

                case gx::command::light_buffer_initialize:
                {
                    render_context->select_light_buffer(device_context, create_per_view_data(in_params));
                    break;
                }

                case gx::command::light_buffer_finalize:
                {
                    render_context->end_light_buffer(device_context, create_per_view_data(in_params));
                    break;
                }
                
                case gx::command::compose_light_buffer:
                {
                    render_context->compose_light_buffer(device_context, create_per_view_data(in_params));
                    break;
                }

                case gx::command::debug_initialize:
                {
                    render_context->select_debug_target(device_context, create_per_view_data(in_params));
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

        m_render_context->reset_state(device_context);

        gx::draw_call_context draw_call_context = create_draw_call_context( device_context, in_params);

        for (uint32_t j = 0; j < in_params->m_draw_calls->size(); ++j )
        {
            gx::draw_call_info info = in_params->m_draw_calls->at(j);
            uint16_t		   index = info.m_entity_index;

            if (index != gx::draw_call_info::invalid_entity)
            {
                draw_call_context.m_wvp_matrix = &in_params->m_wvp_matrices->at(index);
                draw_call_context.m_world_matrix = &in_params->m_world_matrices->at(index);

                gx::draw_call& call = info.m_key.get_draw_call();
                call(&draw_call_context, info.m_key.get_user_data() );
            }
            else
            {
                gx::command::type command = info.m_key.get_command();
                process_command( m_render_context, device_context, command, *in_params);
            }
        }

        //2. Render test quad

        //m_render_context->select_back_buffer_target(device_context);
        /*
        gx::debug_draw_diffuse_gbuffer( device_context, m_render_context, math::mul(  math::scaling_xy( 0.5f ), math::translation_xy( -0.5f ) ) );
        
        gx::debug_draw_normal_gbuffer( device_context, m_render_context, math::mul(  math::scaling_xy( 0.5f ), math::translation( 0.5f, -0.5f, 0.0f ) ) );
        
        //gx::debug_draw_specular_gbuffer( device_context, m_render_context, math::mul(  math::scaling_xy( 0.5f ) , math::translation_xy( 0.5f ) ) );
        
        gx::debug_draw_depth_buffer( device_context, m_render_context, in_params->m_projection_matrix, math::mul(  math::scaling( 0.5f, 0.5f, 1.0f) , math::translation( -0.5f,  0.5f, 0.0f ) ) );
        
        */
        
        //course
        /*
        //gx::debug_draw_light_buffer( device_context, m_render_context, math::mul(  math::scaling_xy( 0.5f ) , math::translation( -0.5f, 0.5f, 0.0f ) ) );
        
        */

        //gx::debug_draw_depth_gbuffer( device_context, m_render_context, math::mul(  math::scaling( 0.5f, 0.5f, 1.0f) , math::translation( -0.5f,  0.5f, 0.0f ) ) );
        gx::debug_draw_depth_buffer( device_context, m_render_context, in_params->m_projection_matrix , math::mul(  math::scaling( 0.5f, 0.5f, 1.0f) , math::translation( -0.5f,  0.5f, 0.0f ) ) );



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

        m_render_context->select_depth_pass( m_render_context->front()->get_device_context().get(), create_per_view_data(*in_params) );

        for (auto i = 0; it != end; ++i, ++it)
        {
            (*it)->begin_frame();

            auto start_item = range_min[i];
            auto end_item = range_max[i];

            auto device_context = (*it)->get_device_context().get();

            m_render_context->select_depth_pass( device_context, create_per_view_data(*in_params) );

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