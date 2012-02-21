#ifndef __GX_FINAL_PIPELINE_NODE_H__
#define __GX_FINAL_PIPELINE_NODE_H__

#include <gx/gx_pipeline.h>

namespace gx
{
    class final_pipeline_node : public pipeline_node
    {
        private:
        void*   do_process(void* input);
    };
}

#endif