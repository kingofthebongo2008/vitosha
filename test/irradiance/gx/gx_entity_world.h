#ifndef __GX_ENTITY_WORLD_H__
#define __GX_ENTITY_WORLD_H__

#include <algorithm>
#include <memory>
#include <vector>

#include <fnd/fnd_world.h>
#include <gx/gx_entity.h>


namespace gx
{
    //acts as a storage for entites
    class entity_world : public fnd::world
    {
        public:

        entity_world()
        {
            m_entities.reserve(4096);
        }

        void add_entity( std::shared_ptr< gx::entity> entity)
        {
            m_entities.push_back(entity);
        }

        void remove_entity( std::shared_ptr< gx::entity> entity )
        {
            m_entities.erase( std::remove( std::begin(m_entities), std::end(m_entities), entity) );
        }

        std::vector < std::shared_ptr< gx::entity> > m_entities;
    };
}

#endif