#include "precompiled.h"

#include <gx/gx_draw_call_collector.h>

#include <algorithm>

namespace gx
{
    draw_call_collector::draw_call_collector( std::vector< draw_call_info >* draw_calls) : m_draw_calls(draw_calls)
    {

    }

    void draw_call_collector::begin()
    {
        m_draw_calls->clear();
        m_draw_calls->reserve(16384);
    }

    void draw_call_collector::end()
    {
        std::sort( std::begin(*m_draw_calls), std::end(*m_draw_calls) );
    }

    void draw_call_collector::add_draw_call( draw_call_key key, uint32_t entity_index )
    {
        draw_call_info info = { key, entity_index };
        m_draw_calls->push_back( info );
    }

}