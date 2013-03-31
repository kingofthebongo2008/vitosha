#ifndef __GX_CREATE_DRAW_CALLS_PIPELINE_NODE_H__
#define __GX_CREATE_DRAW_CALLS_PIPELINE_NODE_H__

#include <vector>

#include <gx/gx_draw_call_collector.h>
#include <gx/gx_pipeline.h>

namespace gx
{
    struct draw_call_collector_context;

    class create_draw_calls_pipeline_node : public pipeline_node
    {
        public:

        create_draw_calls_pipeline_node();

        private:

        std::vector< draw_call_info > m_draw_calls;

        void*   do_process(void* input);

        virtual void on_create_draw_calls( const draw_call_collector_context* context, draw_call_collector* collector ); 
    };
}

#endif