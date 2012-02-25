#ifndef __GX_RENDERER_H__
#define __GX_RENDERER_H__

#include <vector>

#include <math/math_matrix.h>

#include <gx/gx_pipeline.h>

namespace gx
{
    class view;

    class view_pipeline_node : public pipeline_node
    {
        public:

        view_pipeline_node(view* view) : m_view(view)
        {
            
        }

        private:

        void*   do_process(void* input);
        view*   m_view;

        std::vector< math::matrix_float44 > m_pvw_matrices;

    };
}

#endif