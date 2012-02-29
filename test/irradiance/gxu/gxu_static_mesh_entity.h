#ifndef __GXU_MESH_ENTITY_H__
#define __GXU_MESH_ENTITY_H__

#include <cstdint>

#include <gx/gx_entity.h>
#include <gx/gx_indexed_draw_call.h>

namespace gxu
{
    class static_mesh_entity : public gx::entity
    {
        public:

        static_mesh_entity(gx::indexed_draw_call draw_call);
                

        private:
        gx::indexed_draw_call   m_draw_call;

        void on_create_draw_calls( gx::draw_call_collector_context* context, gx::draw_call_collector* collector);
        void on_execute_draw_calls(gx::draw_call_context* context);
    };
}

#endif