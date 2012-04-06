#include "precompiled.h"

#include <win32/application.h>

#include <fnd/fnd_universe.h>
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

	void application::process_user_input()
	{
		std::for_each(begin(m_windows), end(m_windows), [=](window* wnd)
		{  
			wnd->process_user_input();
		}
		) ;
	}

	void application::update()
	{
		m_universe->update_pass_one(0.0f);
		m_universe->update_pass_two(0.0f);
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