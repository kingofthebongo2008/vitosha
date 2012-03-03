#include "precompiled.h"

#include <win32/application.h>
#include <win32/window.h>

#include <algorithm>


namespace wnd
{
	application::application()
	{
		m_windows.reserve(6);
	}

    application::~application()
    {
    }

	void application::update()
	{

	}

	void application::render()
	{
        std::for_each(begin(m_windows), end(m_windows), [=](window* wnd)
		{  
			wnd->render();
		}
		) ;
	}

	void application::new_window(window* window)
	{
		m_windows.push_back(window);
	}

	void application::delete_window(window* window)
	{
		auto wnd = std::find(begin(m_windows), end(m_windows), window);

		if (wnd != end(m_windows))
		{
			auto _end = end(m_windows)-1;
			std::swap(wnd, _end);
			m_windows.pop_back();
		}
	}

}