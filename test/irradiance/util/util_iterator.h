#ifndef __UTILITY_ITERATOR_H__
#define __UTILITY_ITERATOR_H__

#include <iterator>

namespace util
{
    template<class container>
	class push_iterator : public std::iterator<std::output_iterator_tag, void, void, void, void>
	{	

        public:

        typedef push_iterator<container> this_type;
	    typedef container container_type;
	    typedef typename container::const_reference const_reference;
	    typedef typename container::value_type value_type;

	    explicit push_iterator(container& cont)
		    : m_container(&cont)
        {	

	    }

        this_type& operator=(const value_type& value)
        {
            // push value into container
            m_container->push(value);
            return (*this);
        }

	    this_type& operator=(value_type&& value)
        {
            // push value into container
            m_container->push(std::forward<value_type>(value));
            return (*this);
        }

        this_type& operator*()
        {	
            // pretend to return designated value
            return (*this);
        }

        this_type& operator++()
        {	
            // pretend to preincrement
            return (*this);
        }

	    this_type operator++(int)
        {	
            // pretend to postincrement
            return (*this);
        }

        protected:
	    container *m_container;	// pointer to container
    
    };
}

namespace std
{
    template<class container>
	struct _Is_checked_helper< util::push_iterator<container> >
	: public std::true_type
	{	
        // mark push_iterator as checked
	};
}

#endif