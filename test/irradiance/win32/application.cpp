#include "precompiled.h"

#include <win32/application.h>
#include <win32/window.h>

#include <fnd/fnd_universe.h>

#include <gx/gx_scene.h>
#include <gx/gx_view.h>

#include <gx/gx_pipeline.h>
#include <gx/gx_scene_pipeline_node.h>
#include <gx/gx_view_pipeline_node.h>
#include <gx/gx_create_draw_calls_pipeline_node.h>
#include <gx/gx_execute_draw_calls_pipeline_node.h>
#include <gx/gx_final_pipeline_node.h>

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
        gx::pipeline    pipeline;
        gx::view        view;

        pipeline.add_node( std::make_shared< gx::scene_pipeline_node>(m_scene.get()) );
        pipeline.add_node( std::make_shared< gx::view_pipeline_node>(&view) );
        pipeline.add_node( std::make_shared< gx::create_draw_calls_pipeline_node>() );
        pipeline.add_node( std::make_shared< gx::execute_draw_calls_pipeline_node>() );
        pipeline.add_node( std::make_shared< gx::final_pipeline_node>() );

        pipeline.process();

        std::for_each(begin(m_windows), end(m_windows), [=](window* wnd){ wnd->render();} ) ;
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