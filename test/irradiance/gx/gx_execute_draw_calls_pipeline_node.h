#ifndef __GX_EXECUTE_DRAW_CALLS_PIPELINE_NODE_H__
#define __GX_EXECUTE_DRAW_CALLS_PIPELINE_NODE_H__

#include <vector>

#include <gx/gx_pipeline.h>

namespace gx
{
    class view;

    class execute_draw_calls_pipeline_node : public pipeline_node
    {
        public:

        private:
        void*   do_process(void* input);
    };
}

#endif