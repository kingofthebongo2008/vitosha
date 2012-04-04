#ifndef __FND_UPDATER_H__
#define __FND_UPDATER_H__

#include <algorithm>
#include <memory>
#include <vector>

#include <boost/noncopyable.hpp>

namespace fnd
{
	class updater : private boost::noncopyable
	{
		public:

		virtual ~updater()
		{

		}

		void update_pass_one(float dt)
		{
			on_update_pass_one(dt);
		}

		void update_pass_two(float dt)
		{
			on_update_pass_one(dt);
		}

		private:

		virtual void on_update_pass_one(float dt)
		{
			dt;
		}

		virtual void on_update_pass_two(float dt)
		{
			dt;
		}
	};

    template <typename T, typename U1, typename U2> class typed_updater : public updater
	{
		public:

        typed_updater(U1 u1, U2 u2)
        {
            m_items.reserve(256);
        }

        protected:

        typedef std::shared_ptr<T>      item_ptr;
        typedef std::vector<item_ptr>   item_container;

        item_container                  m_items;
		U1								m_u1;
		U2								m_u2;

        private:

		virtual void on_update_pass_one(float dt)
		{
			std::for_each(begin(m_items),end(m_items),[=](item_ptr item) { m_u1( item, dt); });
		}

		virtual void on_update_pass_two(float dt)
		{
			std::for_each(begin(m_items),end(m_items),[=](item_ptr item) { m_u2( item, dt); } );
		}
	};
}

#endif

