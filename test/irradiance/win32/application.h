#ifndef __WIN32_APPLICATION_H__
#define __WIN32_APPLICATION_H__

#include <memory>
#include <vector>

#include <boost/noncopyable.hpp>

#include "fnd/fnd_universe.h"

namespace wnd
{
	class window;

	class application : private boost::noncopyable
	{
		public:
		application();

		void	update();
		void	render();

		void new_window(window* window);
		void delete_window(window* window);

		void set_universe( std::shared_ptr<fnd::universe> universe)
		{
			m_universe = universe;
		}

		private:

		std::vector< window* > m_windows;
		std::shared_ptr<fnd::universe> m_universe;

	};
}


#endif