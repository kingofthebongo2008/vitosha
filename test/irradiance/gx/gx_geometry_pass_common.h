#ifndef __GX_GEOMETRY_PASS_COMMON_H__
#define __GX_GEOMETRY_PASS_COMMON_H__

#include <cstdint>

#include <math/math_matrix.h>


namespace gx
{
	 enum cbuffer_frequency : uint8_t
	 {
		 slot_per_pass = 0,
	     slot_per_draw_call = 1
	 };

	 struct reprojection_params
	 {
		 float a;
		 float b;
		 float c;
		 float d;
	 };

	 struct cbuffer_constants_per_pass
	 {
		 math::float4x4 m_v;	//view matrix;
		 math::float4x4 m_p;	//projection matrix;

		 reprojection_params m_reprojection_params;
	 };
}

#endif