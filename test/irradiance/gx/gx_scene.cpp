#include "precompiled.h"
#include <gx/gx_scene.h>

namespace gx
{
    scene::scene() : 
        m_end ( &m_end)
        , m_cend( &m_cend)
        , m_status(clean)
    {

        m_matrices.reserve(4096);
        m_data.reserve(4096);

        m_root = std::make_shared<node>( );
        rebuild();
    }

    void scene::do_build()
    {

    }

    void scene::rebuild()
    {

    }
}