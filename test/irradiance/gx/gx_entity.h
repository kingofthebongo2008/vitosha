#ifndef __GX_ENTITY_H__
#define __GX_ENTITY_H__

#include <cstdint>
#include <boost/noncopyable.hpp>


namespace gx
{
    class draw_call_collector;
    struct draw_call_collector_context ;

    struct draw_call_context;

    class entity : private boost::noncopyable
    {
        public:

        entity()
        {

        }

        virtual ~entity();

        void create_draw_calls( draw_call_collector_context* context, draw_call_collector* collector );
        void execute_draw_calls(draw_call_context* context);

        private:

        virtual void on_create_draw_calls( draw_call_collector_context* context, draw_call_collector* collector);
        virtual void on_execute_draw_calls(draw_call_context* context);
    };


    inline void entity::create_draw_calls( draw_call_collector_context* context, draw_call_collector* collector )
    {
        on_create_draw_calls(context, collector);
    }

    inline void entity::execute_draw_calls(draw_call_context* context)
    {
        on_execute_draw_calls(context);
    }
}

#endif