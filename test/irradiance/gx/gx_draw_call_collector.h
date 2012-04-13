#ifndef __GX_DRAW_CALL_COLLECTOR_H__
#define __GX_DRAW_CALL_COLLECTOR_H__

#include <cstdint>
#include <vector>

#include <gx/gx_draw_call_key.h>
#include <gx/gx_indexed_draw_call.h>

namespace gx
{
    class entity;

    struct draw_call_info
    {
        draw_call_key       m_key;
        uint16_t            m_entity_index;
		uint16_t			m_entity_draw_call_index;
    };


   //collects draw calls for later sorting
   class draw_call_collector
   {
        public:

        draw_call_collector( std::vector< draw_call_info >* draw_calls); 

        void begin();
        void end();

        void add_draw_call( draw_call_key key, uint32_t entity_index, uint32_t entity_draw_call_index ) ;

        private:

        std::vector<  draw_call_info >* m_draw_calls;
   };

   inline bool operator<( const draw_call_info info1, const draw_call_info& info2)
   {
       return info1.m_key < info2.m_key;
   }

}

#endif