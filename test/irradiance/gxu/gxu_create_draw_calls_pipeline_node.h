#ifndef __GXU_CREATE_DRAW_CALLS_PIPELINE_NODE_H__
#define __GXU_CREATE_DRAW_CALLS_PIPELINE_NODE_H__

#include <utility>
#include <memory>

#include <gx/gx_draw_call_collector.h>
#include <gx/gx_draw_call_collector_context.h>

#include <gx/gx_create_draw_calls_pipeline_node.h>

namespace gxu
{
    template <typename t1 > 
    class create_draw_calls_pipeline_node : public gx::create_draw_calls_pipeline_node
    {
        public:

        explicit create_draw_calls_pipeline_node( const t1& t1 )  :
            //m_t1( std::move(t1) )
            m_t1(t1)
        {

        }

        private:

        t1	m_t1;


        void on_create_draw_calls( const gx::draw_call_collector_context* context, gx::draw_call_collector* collector ) override
        {
            m_t1(context, collector);
        }
    };

    std::shared_ptr < gx::create_draw_calls_pipeline_node > create_collector_pipeline_node( const std::function<void(const gx::draw_call_collector_context*, gx::draw_call_collector*) >& f )
    {
        //make convenient typedef, since this is too big
        typedef create_draw_calls_pipeline_node < std::function< void(const gx::draw_call_collector_context*, gx::draw_call_collector*) > > function_type;

        return std::make_shared < function_type  > (f);//(std::move(f));
    }
}

#endif 