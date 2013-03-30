#include "precompiled.h"

#include <string>

#include <sys/sys_spin_lock.h>
#include <gxu/gxu_text_drawing.h>

namespace gxu
{
    namespace text
    {

        std::unique_ptr<font> create_font( ID3D11Device* device, const uint8_t* data, size_t data_size)
        {
            DirectX::SpriteFont* sprite_font = new DirectX::SpriteFont(device, data, data_size);
            return std::unique_ptr< font > ( new font(sprite_font ) );
        }

        namespace
        {
            struct draw_string_info
            {
                math::float4    m_position_ps;
                std::wstring    m_string;
                const font*     m_font;

                draw_string_info()
                {

                }

                draw_string_info( math::float4 position, std::wstring&& string, const font* font) :
                    m_position_ps(position)
                    , m_string(string)
                    , m_font(font)
                {

                }
            };


            sys::spinlock_fas                   g_string_info_lock;
            std::vector< draw_string_info >&    get_string_info()
            {
                static std::vector< draw_string_info > s_string_info(1024);
                return s_string_info;
            }

            std::unique_ptr<font>                g_debug_font;

            void draw_string( ID3D11DeviceContext*, const font*, const wchar_t*, math::float4  )
            {
        
            }
        }

        void initialize(ID3D11Device* device,  ::gx::shader_database*  )
        {
            g_debug_font = create_debug_font( device );
        }

        void shutdown()
        {
            g_debug_font.reset();
        }

        void draw_string ( const font* f, const wchar_t* text, math::float4 position_ps )
        {
            //this should go in thread local storage
            sys::lock<sys::spinlock_fas> lock( g_string_info_lock );
            get_string_info().emplace_back( draw_string_info( position_ps, std::wstring( text) , f ) ) ;
        }
    }
}