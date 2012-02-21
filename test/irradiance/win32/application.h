#ifndef __WIN32_APPLICATION_H__
#define __WIN32_APPLICATION_H__

#include <memory>
#include <vector>

#include <boost/noncopyable.hpp>

namespace fnd
{
    class universe;
}

namespace gx
{
    class scene;
}

namespace wnd
{
	class window;

	class application : private boost::noncopyable
	{
		public:

        application();
        ~application();

		void	update();
		void	render();

		void new_window(window* window);
		void delete_window(window* window);

		void set_universe( std::shared_ptr<fnd::universe> universe)
		{
			m_universe = universe;
		}

        void set_scene( std::shared_ptr<gx::scene> scene)
		{
			m_scene = scene;
		}

		private:

		std::vector< window* >          m_windows;
		std::shared_ptr<fnd::universe>  m_universe;
        std::shared_ptr<gx::scene>      m_scene;

	};
}


#endif