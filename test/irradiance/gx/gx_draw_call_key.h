#ifndef __GX_DRAW_CALL_KEY_H__
#define __GX_DRAW_CALL_KEY_H__

#include <cstdint>
#include <memory>

namespace gx
{
	struct full_screen_layer
    {
        typedef enum
        {
            game = 0,
            effect,
            hud
        } type;
    };

	struct view_port_layer
    {
        typedef enum
        {
            world = 0,
			water_reflections,
            sky,
            effect
        } type;
    };

    struct translucency
    {
        typedef enum 
        {
            opaque = 0,
            additive = 1,
            subtractive = 2
        } type;
    };

    struct command
    {
        typedef enum : uint8_t
        {
            depth_initialize = 0,
			depth_draw_call,
			depth_finalize,
			
			visibility_initialize,
			visibility_draw_call,
			visibility_finalize,

			shadows_initialize,
			shadows_draw_call,
			shadows_finalize,

			gbuffer_initialize,
			gbuffer_opaque_draw_call,
			gbuffer_finalize,

			light_buffer_initialize,
			light_buffer_draw_call,
			light_buffer_finalize,

            debug_initialize,
			debug_draw_call,
			debug_finalize

        } type;
    };

	struct command_header
	{
		uint8_t				m_full_screen_layer	: 2;
		uint8_t				m_view_port : 3;	
		uint8_t				m_view_port_layer : 3;
		command::type		m_command;
	};

	struct opaque_key_data
	{
		command_header	m_header;

		uint16_t		m_material_id;
		uint32_t		m_depth : 24;
		uint8_t			m_material_pass;
	};

	struct translucency_key_data
	{
		command_header	m_header;
		uint32_t		m_depth : 24;
		uint16_t		m_material_id;
		uint16_t		m_material_pass : 14;
	};

	struct command_key_data
	{
		command_header	m_header;
		uint8_t			m_sequence_number;
		uint32_t		m_command;
	};

	class draw_call_key
    {
        public:

        draw_call_key()
        {
			m_data.m_key[0] = 0;
			m_data.m_key[1] = 0;
        }

		explicit draw_call_key(opaque_key_data opaque)
		{
			m_data.m_key[0] = 0;
			m_data.m_key[1] = 0;

			m_data.m_opaque = opaque;
		}

        explicit draw_call_key(command_key_data command)
		{
			m_data.m_key[0] = 0;
			m_data.m_key[1] = 0;

			m_data.m_command = command;
		}
           
        translucency::type get_translucency_type() const
        {
            return translucency::opaque;
        }

		uint32_t get_depth() const
        {
            return 0;
        }

        uint32_t get_material_id() const
        {
            return 0;
        }

		command::type get_command() const
		{
			return static_cast<command::type> (m_data.m_header.m_command);
		}

		union 
		{
			uint16_t				m_key[8];
			command_header			m_header;
			opaque_key_data			m_opaque;
			translucency_key_data	m_translucency;
			command_key_data		m_command;
		} m_data;
    };

    inline bool operator<(const draw_call_key key1, const draw_call_key key2)
    {
		return std::memcmp( &key1.m_data.m_key[0], &key2.m_data.m_key[0], 8 ) < 0  ;
    }

	inline draw_call_key create_gbuffer_draw_call ( uint32_t material_id, uint32_t material_pass, uint32_t depth )
    {
		opaque_key_data opaque_key = {};

		//setup some default values
		opaque_key.m_header.m_command = command::gbuffer_opaque_draw_call;

		opaque_key.m_material_id = static_cast<uint16_t>(material_id);
		opaque_key.m_material_pass = static_cast<uint8_t> (material_pass);
		opaque_key.m_depth = depth;

		return draw_call_key ( opaque_key );
	}

	inline draw_call_key create_gbuffer_draw_call ( uint32_t material_id, uint32_t material_pass, float depth )
    {
		uint32_t integer_depth = static_cast<uint32_t> ( depth * ( ( 1 << 24 ) - 1) );
        return create_gbuffer_draw_call( material_id, material_pass, integer_depth);
    }

	inline draw_call_key create_gbuffer_draw_call ( uint32_t material_id, float depth )
    {
        return create_gbuffer_draw_call( material_id, 0, depth);
    }

	inline draw_call_key create_debug_draw_call( uint32_t material_id, float depth )
    {
		draw_call_key key = create_gbuffer_draw_call( material_id, depth);

		//setup some default values
		key.m_data.m_header.m_command = command::debug_draw_call;
		return key;
    }

    inline draw_call_key create_light_draw_call()
    {
		command_key_data light_key = {};
		light_key.m_header.m_command = command::light_buffer_draw_call;
		return draw_call_key(light_key);
    }

	inline draw_call_key create_command_draw_call( command::type command  )
    {
		opaque_key_data opaque_key = {};

		opaque_key.m_header.m_command = command;

		opaque_key.m_material_id = 0xffff;
		opaque_key.m_depth = 0xffffffff;
		opaque_key.m_material_pass = 0xff;

		return draw_call_key ( opaque_key );
    }
}

#endif