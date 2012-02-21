#ifndef __GX_SCENE_PIPELINE_NODE_H__
#define __GX_SCENE_PIPELINE_NODE_H__

#include <vector>
#include <math/math_matrix.h>

#include <gx/gx_pipeline.h>

namespace gx
{
    class scene;

    class scene_pipeline_node : public pipeline_node
    {
        public:

        scene_pipeline_node(scene* scene) : m_scene(scene)
        {

        }

        void scene_pipeline_node::submit( const std::vector< math::matrix_float44>* world_matrices, const std::vector<void*>* data );

        private:

        void*   do_process(void* input);

        scene*                             m_scene;

        std::vector< math::matrix_float44> m_world_matrices;
        std::vector< void* >               m_data;
    };
}

#endif