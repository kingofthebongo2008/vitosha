#include "precompiled.h"
#include <gx/gx_pipeline.h>

#include <sys/sys_base.h>

namespace gx
{
    pipeline::pipeline()
    {
        m_nodes.reserve(64);
    }

    void pipeline::process()
    {
        do_process(nullptr);
    }

    void* pipeline::do_process(void* input)
    {
        UNUSED_PARAMETER(input);

        if ( !m_nodes.empty() )
        {
            void* result = (*m_nodes.begin())->process(nullptr);

            auto it = m_nodes.begin() + 1;
            auto it_end = m_nodes.end();

            for ( ; it != it_end; ++it)
            {
                result = (*it)->process(result);
            }
        }

        return nullptr;
    }
}