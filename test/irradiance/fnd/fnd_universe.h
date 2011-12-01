#ifndef __FND_UNIVERSE_H__
#define __FND_UNIVERSE_H__

#include <algorithm>
#include <memory>
#include <vector>

#include "fnd/fnd_world.h"

namespace fnd
{
	class universe
	{
		public:

		virtual ~universe()
		{

		}

		void update(float dt)
		{
			on_update(dt);
		}

		void add_world(std::shared_ptr<fnd::world> world)
		{

		}

		void remove_world(std::shared_ptr<fnd::world> world)
		{

		}

		protected:
		typedef std::shared_ptr<fnd::world> world_ptr;

		std::vector< std::shared_ptr<fnd::world> > m_worlds;

		virtual void on_update(float dt)
		{
			std::for_each(begin(m_worlds),end(m_worlds),[=](world_ptr world) { world->update(dt);});
		}
	};
}

#endif

