#include "precompiled.h"

#include <win32/window.h>
#include <win32/application.h>


namespace wnd
{
	window::~window()
	{

	}

	void window::destroy()
	{
		m_application.delete_window(this);
		delete this;
	}
}