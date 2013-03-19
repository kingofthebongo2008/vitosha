#ifndef __GX_DEPTH_PREPASS_MATERIAL_H__
#define __GX_DEPTH_PREPASS_MATERIAL_H__

#include <d3d11/d3d11_pointers.h>
#include <math/math_vector.h>
#include <gx/gx_transform_position_vertex_shader.h>

namespace gx
{
    struct draw_call_context;
    class  shader_database;

    class __declspec(align(16)) depth_prepass_material
    {
        public:

        depth_prepass_material
                        ( 
                            transform_position_vertex_shader                            vertex_shader,
                            transform_position_vertex_shader_constant_buffer            vertex_cbuffer,
                            transform_position_input_layout                             input_layout
                        );

        void apply(draw_call_context* draw_call_context);

        uint16_t get_id() const
        {
            return 0;
        }

        private:

        transform_position_vertex_shader                    m_vertex_shader;
        transform_position_vertex_shader_constant_buffer    m_vertex_cbuffer;
        transform_position_input_layout                     m_input_layout;
    };

    depth_prepass_material create_depth_prepass_material( const shader_database* context );

    struct depth_prepass_material_factory
    {
        typedef depth_prepass_material type;

        static depth_prepass_material create(const shader_database* context )
        {
            return create_depth_prepass_material(context);
        }
    };
}

#endif