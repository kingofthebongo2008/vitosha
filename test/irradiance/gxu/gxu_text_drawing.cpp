#include "precompiled.h"

#include <gxu/gxu_text_drawing.h>

#include <string>
#include <sys/sys_spin_lock.h>

#include <gx/gx_draw_call_collector.h>
#include <gx/gx_draw_call_context.h>


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

                 draw_string_info( math::float4 position, const std::wstring& string, const font* font) :
                    m_position_ps(position)
                    , m_string(string)
                    , m_font(font)
                {

                }

                draw_string_info( math::float4 position, std::wstring&& string, const font* font) :
                    m_position_ps(position)
                    , m_string(std::move(string) )
                    , m_font(font)
                {

                }
            };


            sys::spinlock_fas                   g_string_info_lock;
            std::vector< draw_string_info >&    get_string_info()
            {
                static std::vector< draw_string_info > s_string_info;
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
            sys::lock<sys::spinlock_fas> lock( g_string_info_lock );
            get_string_info().emplace_back( draw_string_info( position_ps, std::wstring( text ) , f ) ) ;
        }

        void draw_string ( const font* f, const std::wstring& text, math::float4 position_ps )
        {
            sys::lock<sys::spinlock_fas> lock( g_string_info_lock );
            get_string_info().emplace_back( draw_string_info( position_ps, text, f ) ) ;
        }

        void draw_string ( const font* f, std::wstring&& text, math::float4 position_ps )
        {
            sys::lock<sys::spinlock_fas> lock( g_string_info_lock );
            get_string_info().emplace_back( draw_string_info( position_ps, std::move(text), f ) ) ;
        }

        void draw_debug_string( const wchar_t* text, math::float4 position_ps )
        {
            draw_string( g_debug_font.get(), text, position_ps);
        }

        void draw_debug_string( const std::wstring& text, math::float4 position_ps )
        {
            draw_string( g_debug_font.get(), text, position_ps);
        }

        void draw_debug_string( std::wstring&& text, math::float4 position_ps )
        {
            draw_string( g_debug_font.get(), std::move(text), position_ps);
        }

        namespace
        {
            std::vector< draw_string_info > swap_string_info()
            {
                std::vector< draw_string_info > info;
                info.reserve(1024);

                sys::lock<sys::spinlock_fas> lock( g_string_info_lock );
                get_string_info().swap(info);
                return info;
            }

            class draw_call
            {
                public:
                explicit draw_call ( std::vector< draw_string_info > && info ) :
                    m_info(std::move(info))
                {

                }


                void execute_draw_call( gx::draw_call_context* context )
                {
                    DirectX::SpriteBatch    b(context->m_device_context);
                    font*                   f = nullptr;

                    for ( auto& item : m_info )
                    {
                        if ( item.m_font != f )
                        {
                            if (f != nullptr)
                            {
                                b.End();
                            }
                            b.Begin();
                            f = const_cast<font*> (item.m_font);
                        }

                        item.m_font->get_font()->DrawString( &b, item.m_string.c_str(), item.m_position_ps );
                    }

                    if (f != nullptr)
                    {
                        b.End();
                    }

                   delete this;

                }

                private:

                //no stack allocation
                ~draw_call()
                {

                }

                std::vector< draw_string_info > m_info;
            };
        }

        void create_debug_text_draw_calls( gx::draw_call_collector* collector)
        {
            //swap the contents while holding lock as fast as possible
            std::vector< draw_string_info > info = swap_string_info();

            if (!info.empty())
            {
                std::sort( std::begin(info), std::end(info), 
                [ = ]  ( const draw_string_info& info1, const draw_string_info& info2 )
                {
                    return info1.m_font < info2.m_font;
                });


                draw_call* call = new draw_call( std::move(info) );

                gx::draw_call_key key = gx::create_debug_draw_call( 0, 0.0f , std::bind( &draw_call::execute_draw_call, call, std::placeholders::_1 ) );
                collector->add_draw_call( key ) ;
            }
        }
    }
}