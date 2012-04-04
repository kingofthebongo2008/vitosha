#ifndef __GX_PINHOLE_CAMERA_H__
#define __GX_PINHOLE_CAMERA_H__

#include <cstdint>

#include <math/math_graphics.h>

namespace gx
{
    class pinhole_camera
    {
        public:

		pinhole_camera()
		{
			//view parameters
			m_position = math::set(0.0f, 0.0f, 0.0f, 1.0f);
			m_look_at_direction = math::set(0.0f, 0.0f, 1.0f, 0.0f);
			m_up = math::set(0.0f, 1.0f, 0.0f, 0.0f);

			//perspective parameters
			m_fov = 3.1415f / 4.0f;
			m_aspect_ratio = 16.0f / 9.0f;
			m_near = 0.005f; //meters
			m_far = 400.0f;	//meters
		}


		pinhole_camera(
						math::vector_float4 position
						, math::vector_float4 look_at_direction
						, math::vector_float4 up

						, float				fov
						, float				aspect_ratio
						, float				near_z
						, float				far_z)

						:

						m_position( position	)
						, m_look_at_direction(look_at_direction)
						, m_up(up)
						, m_fov(fov)
						, m_aspect_ratio(aspect_ratio)
						, m_near(near_z)
						, m_far(far_z) 
						{

						}

		//view parameters
		math::vector_float4 get_position() const
		{
			return m_position;
		}

		math::vector_float4 get_look_at_direction() const
		{
			return m_look_at_direction;
		}

		math::vector_float4 get_up() const
		{
			return m_up;
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
		void set_position( math::vector_float4 position)
		{
			m_position = position;
		}

		void set_look_at_direction(math::vector_float4 look_at_direction)
		{
			m_look_at_direction = look_at_direction;
		}

		void set_up(math::vector_float4 up)
		{
			m_up = up;
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

		private:

		//view parameters
		math::vector_float4 m_position;
		math::vector_float4 m_look_at_direction;
		math::vector_float4 m_up;

		//perspective parameters
		float				m_fov;
		float				m_aspect_ratio;
		float				m_near;
		float				m_far;

	};

	inline math::matrix_float44 create_view_matrix(const pinhole_camera * camera)
	{
		return math::look_at_lh(camera->get_position(), camera->get_look_at_direction(), math::normalize3( camera->get_up() ) );
	}

	inline math::matrix_float44 create_perspective_matrix(const pinhole_camera * camera)
	{
		return math::perspective_fov_lh(camera->get_fov(), camera->get_aspect_ratio(), camera->get_near(), camera->get_far() );
	}

	inline math::matrix_float44 create_wvp_matrix(const pinhole_camera * camera)
	{
		return math::mul( create_view_matrix(camera), create_perspective_matrix(camera) );
	}

}

#endif