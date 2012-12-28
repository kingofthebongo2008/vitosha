#ifndef __FND_UPDATER_H__
#define __FND_UPDATER_H__

#include <algorithm>
#include <memory>
#include <vector>

#include <boost/noncopyable.hpp>

#include <fnd/fnd_world.h>

namespace fnd
{
	class updater : private boost::noncopyable
	{
		public:

		explicit updater ( uint16_t type) : m_type(type)
		{

		}

		virtual ~updater()
		{

		}

		void update_pass_one(float dt)
		{
			on_update_pass_one(dt);
		}

		void update_pass_two(float dt)
		{
			on_update_pass_two(dt);
		}

		uint16_t get_type() const
		{
			return m_type;
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

		uint16_t m_type;
	};

    template <typename t, typename u1, typename u2> class typed_updater : public updater
	{
		public:

        typed_updater(uint16_t type, u1 u1, u2 u2) : updater(type), m_u1(u1), m_u2(u2)
        {
			m_items.reserve(256);
        }

		void add_item( std::shared_ptr<t> item )
		{
			m_items.push_back ( item );
		}

		void remove_item( std::shared_ptr<t> item )
		{
			m_items.erase( std::remove( std::begin(m_items ), std::end(m_items), updater) );
		}

        protected:

        std::vector< std::shared_ptr<t> >     m_items;
		u1							    	  m_u1;
		u2									  m_u2;

        private:

		virtual void on_update_pass_one(float dt)
		{
			for ( auto& item : m_items )
			{
				m_u1( item.get(), dt) ;
			}
		}

		virtual void on_update_pass_two(float dt)
		{
			for ( auto& item : m_items )
			{
				m_u2( item.get(), dt) ;
			}
		}
	};

	template <typename t> struct null_updater
	{
		void operator()(t* entity, float dt)
		{
			entity;
			dt;
		}
	};

	template <typename t, typename u> class phase_one_updater : public typed_updater<t, u, null_updater<t>>
	{
		private:
		typedef typed_updater<t, u, null_updater<t> > base_type;
		
		public:
		phase_one_updater(uint16_t type, u u) : base_type( type,u, null_updater<t>() )
        {
        }

	};

	template <typename t, typename u> class phase_two_updater : public typed_updater<t, null_updater<t>, u>
	{
		private:
		typedef typed_updater<t, null_updater<t>, u> base_type;
		
		public:

		phase_two_updater(uint16_t type, u u) : base_type( type, null_updater<t>() , u )
        {
        }
	};

	//holds objects split by type and updates them by type
	class typed_world : public fnd::world
	{
		public:

		typed_world()
		{
			m_buckets.reserve(256);
		}

		void add_type ( uint16_t type, std::shared_ptr< fnd::updater> type_updater )
		{
			///??? bug
			//make sure we have enough buckets
			m_buckets.resize( std::max( m_buckets.size(), size_t( type + 1) ) );
			m_buckets[type].reserve(1024);
			m_buckets[type].push_back( type_updater );
		}

		void remove_type(uint16_t type, std::shared_ptr< fnd::updater> type_updater)
		{
			m_buckets[type].erase( std::remove( std::begin(m_buckets[type] ), std::end(m_buckets[type]), type_updater) );
		}

		private:

		typedef std::vector < std::shared_ptr< fnd::updater >  >	items_container;
		std::vector< items_container >								m_buckets;

		virtual void on_update_pass_one(float dt)
		{
			for (auto& items_container : m_buckets)
			{
				for ( auto& updater : items_container)
				{
					updater->update_pass_one(dt);
				}
			}
		}

		virtual void on_update_pass_two(float dt)
		{
			for (auto& items_container : m_buckets)
			{
				for ( auto& updater : items_container)
				{
					updater->update_pass_two(dt);
				}
			}
		}
	};

}

#endif

