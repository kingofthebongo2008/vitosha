#ifndef _FLOOR_H__
#define _FLOOR_H__

#include <memory>
#include <vector>

#include <math/math_vector.h>
#include <d3d11/d3d11_pointers.h>

#include <gx/gx_blinn_phong_shift_invariant_pixel_shader.h>
#include <gx/gx_entity.h>
#include <gx/gx_grid_geometry_shader.h>
#include <gx/gx_material_database.h>
#include <gx/gx_shader_database.h>
#include <gx/gx_transform_position_normal_vertex_shader.h>


namespace gx
{
    class render_context;
}

class floor_entity : public gx::entity
{
	public:

    floor_entity (
                    gx::transform_position_normal_vertex_pipeline                   vertex_pipeline, 
                    gx::grid_geometry_shader                                        geometry_shader,
                    gx::blinn_phong_shift_invariant_pixel_shader				    pixel_shader,
					gx::blinn_phong_shift_invariant_pixel_shader_constant_buffer	pixel_cbuffer,
                    std::tuple<d3d11::ibuffer_ptr, d3d11::ibuffer_ptr>      buffers
                    ) : 
    m_vertex_pipeline(vertex_pipeline)
    ,m_geometry_shader( geometry_shader)
    ,m_pixel_shader(pixel_shader)
    ,m_pixel_cbuffer(pixel_cbuffer)
    ,m_point ( std::get<0> ( buffers ) )
    ,m_stream_out(  std::get<1> ( buffers ) )
    {
        m_material_id = create_material_id(this);
    }

    void on_create_draw_calls( gx::draw_call_collector_context* context, gx::draw_call_collector* collector);
	void on_execute_draw_calls( gx::draw_call_context* draw_call_context );

    private:

    gx::transform_position_normal_vertex_pipeline                   m_vertex_pipeline;
    gx::grid_geometry_shader                                        m_geometry_shader;
    gx::blinn_phong_shift_invariant_pixel_shader					m_pixel_shader;
	gx::blinn_phong_shift_invariant_pixel_shader_constant_buffer	m_pixel_cbuffer;
    d3d11::ibuffer_ptr                                          m_point;    
    d3d11::ibuffer_ptr                                          m_stream_out;    

    uint16_t                                                        m_material_id;
};

std::shared_ptr<floor_entity> create_floor_entity( ID3D11Device* device, gx::shader_database* context, uint32_t width, uint32_t height, uint32_t subdivision_count );


#endif
