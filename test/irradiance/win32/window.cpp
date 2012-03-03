#include "precompiled.h"

#include <win32/window.h>

#include <memory>

#include <win32/application.h>

#include <fnd/fnd_universe.h>

#include <gx/gx_create_draw_calls_pipeline_node.h>
#include <gx/gx_execute_draw_calls_pipeline_node.h>
#include <gx/gx_final_pipeline_node.h>
#include <gx/gx_pipeline.h>
#include <gx/gx_render_context.h>
#include <gx/gx_scene.h>
#include <gx/gx_scene_pipeline_node.h>

#include <gx/gx_view.h>
#include <gx/gx_view_pipeline_node.h>




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

	void window::render()
	{
		if (m_occluded_by_another_window)
		{
			HRESULT hr = m_swap_chain->Present(0, DXGI_PRESENT_TEST );

			if ( hr == S_OK)
			{
				m_occluded_by_another_window = false;
			}

			if (hr != DXGI_STATUS_OCCLUDED)
			{
				dx11::throw_if_failed<dx11::d3d11_exception>(hr);		
			}
		}
		else
		{
			render_frame();

			HRESULT hr = m_swap_chain->Present(0,0);

			if (hr == DXGI_STATUS_OCCLUDED)
			{
				m_occluded_by_another_window = true;
			}
			else
			{
				dx11::throw_if_failed<dx11::d3d11_exception>(hr);		
			}
		}
	}

	void window::render_frame()
	{
		gx::pipeline    pipeline;
		gx::view        view;

		pipeline.add_node( std::make_shared< gx::scene_pipeline_node>(m_scene.get()) );

		pipeline.add_node( std::make_shared< gx::view_pipeline_node>(&view) );
		pipeline.add_node( std::make_shared< gx::create_draw_calls_pipeline_node>() );
		pipeline.add_node( std::make_shared< gx::execute_draw_calls_pipeline_node>(m_render_context) );

		pipeline.add_node( std::make_shared< gx::final_pipeline_node>() );
		pipeline.process();
	}

	void window::resize_window(std::uint32_t width, std::uint32_t height)
	{
		using namespace dx11;
		DXGI_SWAP_CHAIN_DESC desc = {};

		//disable dxgi errors
		width = std::max(width, (uint32_t)(8));
		height = std::max(height, (uint32_t)(8));

		m_render_context->release_swap_chain_buffers();

		throw_if_failed<d3d11_exception>(m_swap_chain->GetDesc(&desc));
		throw_if_failed<d3d11_exception>(m_swap_chain->ResizeBuffers(desc.BufferCount, width, height,  desc.BufferDesc.Format, desc.Flags));

		m_render_context->create_swap_chain_buffers();
	}
}