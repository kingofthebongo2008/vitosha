#ifndef __GX_GBUFFER_DT_NG_SC_GC_MATERIAL_H__
#define __GX_GBUFFER_DT_NG_SC_GC_MATERIAL_H__

#include <math/math_vector.h>
#include <d3d11/d3d11_pointers.h>

#include <gx/gx_gbuffer_dt_ng_sc_gc_pixel_shader.h>

#include <gx/gx_transform_position_normal_uv_vertex_shader.h>


namespace gx
{
    struct draw_call_context;
    class shader_database;

    struct gbuffer_dt_ng_sc_gc_texture_set
    {
        gbuffer_dt_ng_sc_gc_texture_set (
                            ID3D11Device*           device,
                            d3d11::itexture2d_ptr   diffuse
                       );

        d3d11::itexture2d_ptr           m_diffuse;
        d3d11::ishaderresourceview_ptr  m_diffuse_view;
    };

    struct gbuffer_dt_ng_sc_gc_shader_set
    {
        gbuffer_dt_ng_sc_gc_shader_set
                        ( 
                            transform_position_normal_uv_vertex_pipeline vertex_pipeline, 
                            gbuffer_dt_ng_sc_gc_pixel_shader  pixel_shader, 
                            gbuffer_dt_ng_sc_gc_pixel_shader_constant_buffer cbuffer
                          ) :
                          m_vertex_pipeline (vertex_pipeline),
                          m_pixel_shader(pixel_shader),
                          m_pixel_cbuffer(cbuffer)
        {

        }

        transform_position_normal_uv_vertex_pipeline        m_vertex_pipeline;
        gbuffer_dt_ng_sc_gc_pixel_shader                    m_pixel_shader;
        gbuffer_dt_ng_sc_gc_pixel_shader_constant_buffer    m_pixel_cbuffer;
    };

    class gbuffer_dt_ng_sc_gc_material
    {
        public:
        gbuffer_dt_ng_sc_gc_material ( gbuffer_dt_ng_sc_gc_texture_set texture_set, gbuffer_dt_ng_sc_gc_shader_set shader_set, math::float4 ks_gloss, bool has_alpha_test );

        void apply(draw_call_context* draw_call_context);

        uint16_t get_id() const
        {
            return m_material_id;
        }

        math::float4 get_ks_gloss() const
        {
            return m_ks_gloss;
        }

        void set_ks_gloss(math::float4 value)
        {
            m_ks_gloss = value;
        }

        bool get_has_alpha_test() const
        {
            return m_has_alpha_test;
        }

        void set_has_alpha_test(bool value)
        {
            m_has_alpha_test = value;
        }

        private:
        gbuffer_dt_ng_sc_gc_texture_set m_texture_set;
        gbuffer_dt_ng_sc_gc_shader_set  m_shader_set;
        math::float4                    m_ks_gloss;
        bool                            m_has_alpha_test;
        uint16_t                        m_material_id;
    };

    gbuffer_dt_ng_sc_gc_material create_gbuffer_dt_ng_sc_gc_material( ID3D11Device* device, const shader_database* context,  d3d11::itexture2d_ptr diffuse, math::float4 ks_gloss, bool has_alpha_test ) ;
}

#endif