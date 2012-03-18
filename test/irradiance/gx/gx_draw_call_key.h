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

    struct view_port_layer
    {
        typedef enum
        {
            world = 0,
            sky = 1,
            effect = 2
        } type;
    };

	struct translucency_key_data
	{
		uint8_t		m_full_screen_layer	: 2;
		uint8_t		m_view_port : 3;			
		uint8_t		m_view_port_layer : 2;
		uint8_t		m_pass : 3;
		uint8_t		m_translucency : 2;

		uint32_t	m_depth : 24;
		uint16_t	material_id : 16;
		uint16_t	material_pass : 14;
	};

	struct opaque_key_data
	{
		uint8_t		m_full_screen_layer	: 2;
		uint8_t		m_view_port : 3;			
		uint8_t		m_view_port_layer : 2;
		uint8_t		m_pass : 3;
		uint8_t		m_translucency : 2;

		uint16_t	material_id : 16;
		uint16_t	material_pass : 14;
		uint32_t	m_depth : 24;
	};

	struct command_key_data
	{
		uint8_t		m_full_screen_layer	: 2;
		uint8_t		m_view_port : 3;
		uint8_t		m_view_port_layer : 2;
		uint8_t		m_pass : 3;
		uint8_t		m_translucency : 2;

		uint32_t	m_sequence_number : 8;
		uint64_t	m_command : 44;
	};

	class draw_call_key
    {
        public:

        draw_call_key()
        {
			m_data.m_key = 0;
        }

		explicit draw_call_key(opaque_key_data opaque)
		{
			m_data.m_opaque = opaque;
		}
           
        inline translucency::type get_translucency_type() const
        {
            return translucency::opaque;
        }

		inline uint32_t get_depth() const
        {
            return 0;
        }

        inline uint32_t get_material_id() const
        {
            return 0;
        }

		union 
		{
			uint64_t				m_key;
			opaque_key_data			m_opaque;
			translucency_key_data	m_translucency;
			command_key_data		m_command;
		} m_data;
    };

    inline bool operator<(draw_call_key key1, draw_call_key key2)
    {
		return key1.m_data.m_key < key2.m_data.m_key;
    }

	inline draw_call_key create_gbuffer_opaque_key ( uint32_t material_id, uint32_t material_pass, uint32_t depth )
    {
		opaque_key_data opaque_key;

		//setup some default values
		opaque_key.m_full_screen_layer = full_screen_layer::game;
		opaque_key.m_view_port = 0;
		opaque_key.m_view_port_layer = view_port_layer::world;

		opaque_key.m_pass = pass::gbuffer;
		opaque_key.m_translucency = translucency::opaque;

		opaque_key.material_id = material_id;
		opaque_key.material_pass = material_pass;
		opaque_key.m_depth = depth;

		return draw_call_key ( opaque_key );
	}

	inline draw_call_key create_gbuffer_opaque_key ( uint32_t material_id, uint32_t material_pass, float depth )
    {
		uint32_t integer_depth = static_cast<uint32_t> ( depth * ( ( 1 << 24 ) - 1) );
        return create_gbuffer_opaque_key( material_id, material_pass, integer_depth);
    }

	inline draw_call_key create_gbuffer_opaque_key ( uint32_t material_id, float depth )
    {
        return create_gbuffer_opaque_key( material_id, 0, depth);
    }

    inline draw_call_key create_additive_key( uint32_t material_id, float depth)
    {
        return create_gbuffer_opaque_key( material_id, 1.0f - depth);
    }

    inline draw_call_key create_subtractive_key( uint32_t material_id, float depth)
    {
		return create_gbuffer_opaque_key( material_id, 1.0f - depth);
    }
}

#endif