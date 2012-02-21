#include "precompiled.h"

#include <gx/gx_final_pipeline_node.h>
#include <gx/gx_pipeline_params.h>


namespace gx
{
    void* final_pipeline_node::do_process(void* input)
    {
        view_pipeline_params* params = reinterpret_cast<view_pipeline_params*> (input);
        delete params;

        return nullptr;
    }
}