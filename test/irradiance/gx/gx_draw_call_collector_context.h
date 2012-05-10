#ifndef __GX_DRAW_CALL_COLLECTOR_CONTEXT_H__
#define __GX_DRAW_CALL_COLLECTOR_CONTEXT_H__

#include <cstdint>

#include <math/math_graphics.h>
#include <math/math_matrix.h>

namespace gx
{
   struct draw_call_collector_context
   {
        math::float4x4*    m_view_matrix;
        math::float4x4*    m_projection_matrix;

        math::float4x4*    m_world_matrix;
        math::float4x4*    m_wvp;
		uint32_t			 m_entity_index;
   };

   inline float get_perspective_transform3_depth( const draw_call_collector_context* const context )
   {
	   //we assume that (0,0,0) is the center (pivot) of the object. 
		auto ety_pos_os = math::set(0.0f, 0.0f, 0.0f, 1.0f);
		auto ety_pos_ss = math::perspective_transform3(ety_pos_os, *context->m_wvp );
		return ety_pos_ss.m128_f32[3];
   }

}

#endif