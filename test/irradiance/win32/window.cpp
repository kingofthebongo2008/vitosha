#include "precompiled.h"
#include "window.h"
#include "application.h"


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