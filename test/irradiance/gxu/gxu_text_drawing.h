#ifndef __GXU_TEXT_DRAWING_H__
#define __GXU_TEXT_DRAWING_H__

#include <cstdint>
#include <future>

#include <math/math_vector.h>
#include <d3d11/d3d11_pointers.h>

#include <directxtk/inc/spritefont.h>

namespace gx
{
    class draw_call_collector;
    class shader_database;    
}

namespace gxu
{
    namespace text
    {
        class font 
        {
            public:

            explicit font(DirectX::SpriteFont* font ) : m_font(font)
            {

            }

            font( font&& o ) : m_font(o.m_font)
            {
                o.m_font = nullptr;
            }

            font& operator=(font&& o)
            {
                font( std::move(o) ).swap(*this);
                return *this;
            }

            ~font()
            {
                delete m_font;
            }

            DirectX::SpriteFont*    get_font() const
            {
                return m_font;
            }

            private:

            font();
            font(const font& o); 
            const font& operator=(const font& o);

            void swap(font& o)
            {
                std::swap(m_font, o.m_font);
            }

            DirectX::SpriteFont*    m_font;
        };

        std::unique_ptr<font> create_font( ID3D11Device* device, const uint8_t* data, size_t data_size);

        inline std::unique_ptr<font> create_debug_font( ID3D11Device* device )
        {
            extern std::uint8_t   debug_sprite_font[];
            //extern const uint8_t debug_sprite_font[];
            extern size_t debug_sprite_font_size;
            return create_font(device, &debug_sprite_font[0], debug_sprite_font_size);
        }

        //async
        inline std::future< std::unique_ptr<font> > load_font(ID3D11Device*, const wchar_t* )
        {
           return std::async( [=] 
           {
                std::unique_ptr<font> f( new font(nullptr) );
                return std::move(f);
           }
           );
        }

        void draw_string ( const font* f, const wchar_t* text, math::float4 position_ps );
        void draw_string ( const font* f, const std::wstring& text, math::float4 position_ps );
        void draw_string ( const font* f, std::wstring&& text, math::float4 position_ps );

        void draw_debug_string( const wchar_t* text, math::float4 position_ps );
        void draw_debug_string( const std::wstring& text, math::float4 position_ps );
        void draw_debug_string( std::wstring&& text, math::float4 position_ps );


        void initialize(ID3D11Device* device, gx::shader_database* shader_database );
        void shutdown();

        class initializer
        {
            public:

            explicit initializer(ID3D11Device* device, gx::shader_database* shader_database)
            {
                initialize(device, shader_database);
            }

            ~initializer()
            {
                shutdown();
            }
        };


        //must be called once in a while to flush the draw calls
        void create_debug_text_draw_calls( gx::draw_call_collector* collector);
    }
}

#endif