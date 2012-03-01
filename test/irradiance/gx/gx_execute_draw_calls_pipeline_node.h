#ifndef __GX_EXECUTE_DRAW_CALLS_PIPELINE_NODE_H__
#define __GX_EXECUTE_DRAW_CALLS_PIPELINE_NODE_H__

#include <vector>

#include <gx/gx_pipeline.h>

#include <dx11/dx11_render_context.h>

namespace gx
{
    class view;

    class execute_draw_calls_pipeline_node : public pipeline_node
    {
        public:

		explicit execute_draw_calls_pipeline_node(dx11::render_context* render_context) : m_render_context (render_context)
		{

		}

        private:

        dx11::render_context* m_render_context;
        void*   do_process(void* input);
    };
}

#endif