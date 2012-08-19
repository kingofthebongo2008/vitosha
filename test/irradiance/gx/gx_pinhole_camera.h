#ifndef __GX_PINHOLE_CAMERA_H__
#define __GX_PINHOLE_CAMERA_H__

#include <cstdint>

#include <math/math_graphics.h>

namespace gx
{
    struct pinhole_camera
    {
        public:

		pinhole_camera()
		{
			//view parameters
			m_view_position_ws = math::set(0.0f, 0.0f, 0.0f, 1.0f);
			m_view_direction_ws = math::set(0.0f, 0.0f, 1.0f, 0.0f);
			m_view_up_ws = math::set(0.0f, 1.0f, 0.0f, 0.0f);

			//perspective parameters
			m_fov = 3.1415f / 4.0f;
			m_aspect_ratio = 16.0f / 9.0f;
			m_near = 0.1f; //meters
			m_far = 400.0f;	//meters
		}


		pinhole_camera(
						math::float4 position
						, math::float4 look_at_direction
						, math::float4 up

						, float				fov
						, float				aspect_ratio
						, float				near_z
						, float				far_z)

						:

						m_view_position_ws( position	)
						, m_view_direction_ws(look_at_direction)
						, m_view_up_ws(up)
						, m_fov(fov)
						, m_aspect_ratio(aspect_ratio)
						, m_near(near_z)
						, m_far(far_z) 
						{

						}

		//view parameters
		math::float4 get_position() const
		{
			return m_view_position_ws;
		}

		math::float4 get_view_direction() const
		{
			return m_view_direction_ws;
		}

		math::float4 get_up() const
		{
			return m_view_up_ws;
		}

		//perspective parameters
		float				get_fov() const
		{
			return m_fov;
		}

		float				get_aspect_ratio() const
		{
			return m_aspect_ratio;
		}

		float				get_near() const
		{
			return m_near;
		}

		float				get_far() const
		{
			return m_far;
		}

		//view parameters
		void set_view_position( math::float4 position_ws)
		{
			m_view_position_ws = position_ws;
		}

		void set_view_direction(math::float4 view_direction_ws)
		{
			m_view_direction_ws = view_direction_ws;
		}

		void set_view_up(math::float4 up_ws)
		{
			m_view_up_ws = up_ws;
		}

		//perspective parameters
		void set_fov(float	fov)
		{
			m_fov = fov;
		}

		void set_aspect_ratio(float	aspect_ratio)
		{
			m_aspect_ratio = aspect_ratio;
		}

		void set_near(float	value)
		{
			m_near = value;
		}

		void set_far(float	value)
		{
			m_far = value;
		}

		public:

		//view parameters
		math::float4 m_view_position_ws;
		math::float4 m_view_direction_ws;
		math::float4 m_view_up_ws;

		//perspective parameters
		float				m_fov;
		float				m_aspect_ratio;
		float				m_near;
		float				m_far;
	};

	inline math::float4x4 create_view_matrix(const pinhole_camera * camera)
	{
		using namespace math;
		return view(camera->get_position(), normalize3( camera->get_view_direction() ) , normalize3( camera->get_up() ) );
	}

	inline math::float4x4 create_inverse_view_matrix(const pinhole_camera * camera)
	{
		using namespace math;
		return inverse_view(camera->get_position(), normalize3( camera->get_view_direction() ) , normalize3( camera->get_up() ) );
	}

	inline math::float4x4 create_perspective_matrix(const pinhole_camera * camera)
	{
		return math::perspective_fov_lh(camera->get_fov(), camera->get_aspect_ratio(), camera->get_near(), camera->get_far() );
	}

	inline math::float4x4 create_inverse_perspective_matrix(const pinhole_camera * camera)
	{
		return math::inverse_perspective_fov_lh(camera->get_fov(), camera->get_aspect_ratio(), camera->get_near(), camera->get_far() );
	}

	inline math::float4x4 create_vp_matrix(const pinhole_camera * camera)
	{
		return math::mul( create_view_matrix(camera), create_perspective_matrix(camera) );
	}

	inline math::float4x4 create_inverse_vp_matrix(const pinhole_camera * camera)
	{
		return math::mul( create_inverse_perspective_matrix(camera), create_inverse_view_matrix(camera) );
	}

	inline void rotate(pinhole_camera* camera, math::float4 quaternion)
	{
		auto up = camera->get_up();
		auto direction = camera->get_view_direction();

		camera->set_view_up( math::rotate_vector3(up, quaternion));
		camera->set_view_direction( math::rotate_vector3(direction, quaternion));
	}

}

#endif