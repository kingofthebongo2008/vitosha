#ifndef __FND_UPDATER_H__
#define __FND_UPDATER_H__

#include <algorithm>
#include <memory>
#include <vector>

namespace fnd
{
	class updater
	{
		public:

		virtual ~updater()
		{

		}

		void update(float dt)
		{
			on_update(dt);
		}

		private:

		virtual void on_update(float dt)
		{
			dt;
		}
	};

    template <typename T> class typed_updater : public updater
	{
		public:

        typed_updater()
        {
            m_items.reserve(256);
        }

        protected:

        typedef std::shared_ptr<T>      item_ptr;
        typedef std::vector<item_ptr>   item_container;

        item_container                  m_items;

        private:

		virtual void on_update(float dt)
		{
			std::for_each(begin(m_items),end(m_items),[=](item_ptr item) { item->update(dt);});
		}
	};
}

#endif

