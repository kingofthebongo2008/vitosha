#ifndef __GX_DRAW_CALL_KEY_H__
#define __GX_DRAW_CALL_KEY_H__

#include <cstdint>

namespace gx
{
    struct translucency
    {
        typedef enum 
        {
            opaque = 0,
            additive = 1,
            subtractive = 2
        } type;
    };

    struct pass
    {
        typedef enum
        {
            depth = 0,
            visibility = 1,
            shadows = 2,
            gbuffer = 3,
            lighting = 4
        } type;
    };

    struct full_screen_layer
    {
        typedef enum
        {
            game = 0,
            effect = 1,
            hud = 2
        } type;
    };

    struct view_layer
    {
        typedef enum
        {
            world = 0,
            sky = 1,
            effect = 2
        } type;
    };

    class draw_call_key
    {
        public:

        draw_call_key() : m_data(0)
        {

        }
           
        inline translucency::type get_translucency_type() const
        {
            return translucency::opaque;
        }

        inline void set_translucency_type( translucency::type type)
        {
            uint64_t mask = 0xC000000000000000L;
            uint64_t not_mask = ~mask;
            uint64_t t = m_data & not_mask;
            m_data = t | ( (uint64_t) type << 63);
        }

        inline uint32_t get_depth() const
        {
            return 0;
        }

        inline void set_depth(uint32_t depth)
        {
            uint64_t mask = 0x0FFFFFFFF;
            uint64_t not_mask = ~mask;
            uint64_t t = m_data & not_mask;
            m_data = t | depth;
        }

        inline uint32_t get_material_id() const
        {
            return 0;
        }

        inline void set_material_id(uint32_t material_id)
        {
            uint64_t mask = 0x3FFFFFFF00000000L;
            uint64_t not_mask = ~mask;
            uint64_t t = m_data & not_mask;
            m_data = t | ( (uint64_t) material_id << 61);
        }

        uint64_t m_data;
    };

    inline draw_call_key create_draw_call_key( translucency::type type, uint32_t material_id, uint32_t depth)
    {
        draw_call_key key;
        key.m_data = ( (uint64_t) type << 63) | ( (uint64_t) material_id << 61) | depth;
        return key;
    }

    inline draw_call_key create_draw_call_key( translucency::type type, uint32_t material_id, float depth)
    {
        uint32_t integer_depth = static_cast<uint32_t> ( depth * ( ( 1 << 24 ) - 1) );
        return create_draw_call_key(type, material_id, integer_depth);
    }

    inline draw_call_key create_opaque_key ( uint32_t material_id, float depth)
    {
        return create_draw_call_key( translucency::opaque, material_id, depth);
    }

    inline draw_call_key create_additive_key( uint32_t material_id, float depth)
    {
        return create_draw_call_key( translucency::additive, material_id, 1.0f - depth);
    }

    inline draw_call_key create_subtractive_key( uint32_t material_id, float depth)
    {
        return create_draw_call_key( translucency::subtractive, material_id, 1.0f - depth);
    }
    
    inline bool operator<(draw_call_key key1, draw_call_key key2)
    {
        return key1.m_data < key2.m_data;
    }
}

#endif