#ifndef __GX_SCENE_H__
#define __GX_SCENE_H__

#include <algorithm>
#include <iterator>
#include <memory>
#include <stack>
#include <vector>


#include <util/util_iterator.h>
#include <math/math_matrix.h>
#include <fnd/fnd_world.h>

namespace gx
{
    class scene : public fnd::world
    {
        public:

        enum node_status { clean = 0 , modified = 1 };

        class const_node_iterator;
        class node_iterator;

        class node
        {
            public:
            node() : 
                    m_transform( math::matrix44_identity() )
                    , m_data(nullptr)
                    , m_status(nullptr)
                    , m_node_transform(nullptr)
            {
                m_children.reserve(16);
            }

            node( math::matrix_float44 transform, void* data) : 
                m_transform(transform)
                , m_data(data)
                , m_status(nullptr)
            {
                m_children.reserve(16);
            }

            inline math::matrix_float44 get_transform() const
            {
                return m_transform;
            }

            inline void set_transform(math::matrix_float44 transform)
            {
                m_transform = transform;

                if (m_node_transform)
                {
                    *m_node_transform = transform;
                }

                set_modified();
            }

            inline void* get_data() const
            {
                return m_data;
            }

            template <typename t> t* get_data()
            {
                return m_data;
            }

            template <typename t> const t* get_data() const
            {
                return m_data;
            }

            inline void set_node_status(node_status* status)
            {
                m_status = status;
            }

            inline void set_node_transform(math::matrix_float44* node_transform)
            {
                m_node_transform = node_transform;
            }

            inline std::weak_ptr<node> get_parent()
            {
                return m_parent;
            }

            inline const std::weak_ptr<node> get_parent() const
            {
                return m_parent;
            }

            inline void set_parent(std::weak_ptr<node> parent)
            {
                m_parent = parent;
            }

            inline void add_child_node(std::shared_ptr<node> node)
            {
                m_children.push_back(node);
            }

            inline void remove_child_node(std::shared_ptr<node> node)
            {
                std::remove(std::begin(m_children), std::end(m_children), node);
            }

            private:
            friend class const_node_iterator;
            friend class node_iterator;
            friend class scene;
            
            typedef std::vector< std::shared_ptr<node> > children_container;

            math::matrix_float44        m_transform;
            void*                       m_data;
            children_container          m_children;
            std::weak_ptr<node>         m_parent;
            node_status*                m_status;
            math::matrix_float44*       m_node_transform;

            inline void set_modified()
            {
                if (m_status != nullptr)
                {
                    *m_status = modified;
                }
            }
        };

        class const_node_iterator 
        {
            public:

            typedef const_node_iterator                         this_type;

            typedef std::input_iterator_tag                     iterator_category;
            typedef std::shared_ptr<node>                       value_type;
            typedef std::shared_ptr<const node>*                pointer;
            typedef const std::shared_ptr<const node>&          reference;
            typedef std::ptrdiff_t                              difference_type;

            typedef const std::shared_ptr<const node>*          const_pointer;
            
            this_type() :
                m_end(nullptr)
            {

            }

            explicit this_type( this_type* end) : 
               m_end(end)
            {

            }

            this_type(value_type value, this_type* end) : 
                m_current(value)
                , m_end(end)
            {
                m_iterator_stack.push(value);
            }

            inline const this_type& operator++()
            {
                if (m_iterator_stack.empty())
                {
                    return *m_end;
                }
                else
                {
                    value_type  top = m_iterator_stack.top();
                    m_iterator_stack.pop();

                    std::copy(top->m_children.rbegin(), top->m_children.rend(), util::push_iterator<stack_container> (m_iterator_stack) );

                    if (!m_iterator_stack.empty())
                    {
                        m_current = m_iterator_stack.top();
                    }
                    else
                    {
                        m_current.reset();
                    }

                    return *this;
                }
            }

            inline this_type operator++(int)
            {
                this_type temp = *this;
                ++*this;
                return (temp);
            }

            inline bool operator==(const this_type& right) const
            {
                return (m_current == right.m_current);
            }

            inline bool operator!=(const this_type& right) const
            {
                return (!(*this == right));
            }

            const_pointer operator->() const
            {	
                return &m_current;
            }

            reference operator*() const
            {
                return m_current;
            }

            protected:
            typedef std::stack<value_type, std::vector<value_type> > stack_container;

            std::shared_ptr<const node>                         m_current;
            this_type*                                          m_end;
            stack_container                                     m_iterator_stack;
        };

        class node_iterator 
        {
            public:

            typedef node_iterator                               this_type;

            typedef std::input_iterator_tag                     iterator_category;
            typedef std::shared_ptr<node>                       value_type;
            typedef std::shared_ptr<node>*                      pointer;
            typedef std::shared_ptr<node>&                      reference;
            typedef std::ptrdiff_t                              difference_type;

            this_type() :
                m_end(nullptr)
            {

            }

            explicit this_type( this_type* end) : 
               m_end(end)
            {

            }

            this_type(value_type value, this_type* end) : 
                m_current(value)
                , m_end(end)
            {
                m_iterator_stack.push(value);
            }

            inline const this_type& operator++()
            {
                if (m_iterator_stack.empty())
                {
                    return *m_end;
                }
                else
                {
                    value_type  top = m_iterator_stack.top();
                    m_iterator_stack.pop();

                    std::copy(top->m_children.rbegin(), top->m_children.rend(), util::push_iterator<stack_container> (m_iterator_stack) );

                    if (!m_iterator_stack.empty())
                    {
                        m_current = m_iterator_stack.top();
                    }
                    else
                    {
                        m_current.reset();
                    }

                    return *this;
                }
            }

            inline this_type operator++(int)
            {
                this_type temp = *this;
                ++*this;
                return (temp);
            }

            inline bool operator==(const this_type& right) const
            {
                return (m_current == right.m_current);
            }

            inline bool operator!=(const this_type& right) const
            {
                return (!(*this == right));
            }

            pointer operator->()
            {	
                return &m_current;
            }

            reference operator*()
            {
                return m_current;
            }

            protected:
            typedef std::stack<value_type, std::vector<value_type> > stack_container;

            std::shared_ptr<node>                               m_current;
            this_type*                                          m_end;
            stack_container                                     m_iterator_stack;
        };


        scene();

        inline void build()
        {
            if (m_status == modified)
            {
                do_build();
            }

            set_status(clean);
        }

        void rebuild();

        inline std::shared_ptr<node> get_root()
        {
            return m_root;
        }

        inline const std::shared_ptr<node> get_root() const
        {
            return m_root;
        }

        inline node_iterator begin()
        {
            return node_iterator( get_root(), &m_end ) ;
        }

        inline node_iterator end()
        {
            return node_iterator( std::shared_ptr<node>(), &m_end );
        }

        inline const_node_iterator cbegin() const
        {
            return const_node_iterator( get_root(), const_cast<const_node_iterator*> (&m_cend) ) ;
        }

        inline const_node_iterator cend() const
        {
            return const_node_iterator( std::shared_ptr<node>(), const_cast<const_node_iterator*> (&m_cend) );
        }

        template <typename t> void process(t* processor) const
        {
            processor->submit( &m_world_matrices, &m_data );
        }

        private:
        std::shared_ptr<node>                               m_root;
        node_iterator                                       m_end;
        const_node_iterator                                 m_cend;
        node_status                                         m_status;

        typedef uint32_t flat_node_parent_info;


        std::vector< math::matrix_float44 >                 m_world_matrices;
        std::vector< math::matrix_float44 >                 m_matrices;
        std::vector< void* >                                m_data;
        std::vector< flat_node_parent_info >                m_flat_nodes;

        void do_build();

        inline void set_status(node_status status)
        {
            m_status = status;
        }
    };

    inline void add_node(std::weak_ptr<scene::node> parent, std::shared_ptr<scene::node> child)
    {
        std::shared_ptr<scene::node> lock = parent.lock();
        lock->add_child_node(child);
        child->set_parent(parent);
    }

    inline void remove_node(std::shared_ptr<scene::node> child)
    {
        std::shared_ptr<scene::node> lock = child->get_parent().lock();
        lock->remove_child_node(child);
        child->set_parent( std::weak_ptr<scene::node> () );
    }
}

#endif