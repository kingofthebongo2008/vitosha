#ifndef __GX_PIPELINE_H__
#define __GX_PIPELINE_H__

#include <algorithm>
#include <vector>

namespace gx
{
    class pipeline_node
    {
        public:

        virtual ~pipeline_node()
        {

        }

        inline void*   process(void* input)
        {
            return do_process(input);
        }

        private:

        virtual void*   do_process(void* input) = 0;
    };

    class pipeline
    {
        public:
        pipeline();

        void add_node( std::shared_ptr<pipeline_node> node)
        {
            m_nodes.push_back(node);
        }

        void remove_node( std::shared_ptr<pipeline_node> node )
        {
            std::remove(std::begin(m_nodes), std::end(m_nodes), node);
        }

        void process();

        private:

        std::vector< std::shared_ptr<pipeline_node> >   m_nodes;
    };
}

#endif