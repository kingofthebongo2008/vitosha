#include "precompiled.h"

#include <win32/irradiance.h>

#include <windowsx.h>

#include <sys/sys_base.h>

#include <d3d11/d3d11_system.h>
#include <fnd/fnd_universe.h>

#include <gx/gx_initializer.h>
#include <gx/gx_render_context.h>


#include <win32/application.h>
#include <win32/window.h>

#include <iostream>
#include <fstream>
#include <math/math_functions.h>
#include <math/math_half.h>
#include <math/math_quaternion.h>
#include <math/xnamath.h>
#include <iostream>
#include <io/io_console.h>
#include <io/io_std_console_notifier.h>

#include <d2d/d2d_helpers.h>

#define MAX_LOADSTRING 100

// Global Variables:
static HINSTANCE hInst;                                 // current instance
static TCHAR szTitle[MAX_LOADSTRING];                   // The title bar text
static TCHAR szWindowClass[MAX_LOADSTRING];             // the main window class name

// Forward declarations of functions included in this code module:
static ATOM             MyRegisterClass(HINSTANCE hInstance, LPCTSTR szWindowClass);
static HWND             InitInstance(HINSTANCE);
static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
static INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

using namespace wnd;

extern std::shared_ptr<gx::scene> universe_bootstrap(  gx::render_context* render_context, d3d11::system_context context, std::shared_ptr<fnd::universe> universe );


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    try
    {
        io::console::runner runner;
        io::console::register_thread_info_helper helper_i;
        io::console::register_notifier_helper helper_n ( std::make_shared<io::console::std_notifier>( ) );

        io::console::write(L"test");

        UNUSED_PARAMETER(hInstance);
        UNUSED_PARAMETER(nCmdShow);

        UNUSED_PARAMETER(hPrevInstance);
        UNUSED_PARAMETER(lpCmdLine);

    
        // Initialize global strings
        LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
        LoadString(hInstance, IDC_IRRADIANCE, szWindowClass, MAX_LOADSTRING);
        MyRegisterClass(hInstance, szWindowClass);

        HWND hwnd = InitInstance (hInstance);

        if (hwnd == 0)
        {
            return FALSE;
        }

        RECT r;
        ::GetClientRect(hwnd, &r);

        math::details2::generate_tables();

        //1. Initialize static graphic data (singletons)
        gx::initializer			initializer;

        //2. Create a default view port
        gx::view_port			view_port( r.left, r.top, r.right - r.left, r.bottom - r.top );
        //3. Create direct3d11 structures
        d3d11::system_context	context = d3d11::create_system_context(hwnd);

        std::shared_ptr<gx::shader_database>		shader_database = std::make_shared<gx::shader_database>( context.m_device.get() );
        //4. Create render contexts
        gx::render_context		render_context(context, shader_database, 3, view_port);
        //5. Create application with a window and worlds with data
        application				application;

        std::shared_ptr<fnd::universe> universe = std::make_shared<fnd::universe>();
        std::shared_ptr<gx::scene> scene = universe_bootstrap( &render_context, context, universe );

        application.set_universe(universe);
        application.set_scene(scene);

        gx::target_render_resource d2d_resource	= gx::create_target_render_resource( context.m_device.get(), view_port.get_width(), view_port.get_height(), DXGI_FORMAT_B8G8R8A8_UNORM );

        window* wnd = new window( hwnd, application, context.m_swap_chain, &render_context, d2d_resource );
        application.new_window(wnd);
        wnd->set_scene(scene);
        ::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(wnd) );

        HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IRRADIANCE));
        MSG msg = {};

        ::ShowWindow(hwnd, nCmdShow);
        ::UpdateWindow(hwnd);

        while (msg.message != WM_QUIT)
        {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                else
                {
                    application.process_user_input();
                    application.update();
                    application.render();
                }
            }
            else
            {
                    application.process_user_input();
                    application.update();
                    application.render();
            }
        }

        return (int) msg.wParam;
    }

    catch (...)
    {
        return 1;
    }
}

static ATOM MyRegisterClass(HINSTANCE hInstance, LPCTSTR szWindowClass)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style			= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc	= WndProc;
    wcex.cbWndExtra		= 0;
    wcex.cbClsExtra		= 0;
    wcex.hInstance		= hInstance;
    wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IRRADIANCE));
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_IRRADIANCE);
    wcex.lpszClassName	= szWindowClass;
    wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

static HWND InitInstance(HINSTANCE hInstance)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return 0;
   }

   return hWnd;
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;

    auto wnd = reinterpret_cast<window*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message)
    {
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_PAINT:
        if (wnd !=0 )
        {
            wnd->render();
            ValidateRect( hWnd, NULL );
        }
        break;

    case WM_SIZE:
        {
            if (wnd !=0 )
            {
                wnd->resize_window(LOWORD(lParam), HIWORD(lParam));
                wnd->render();
            }
        }
        break;

    case WM_DESTROY:
        {
            if (wnd)
            {
                wnd->destroy();
            }
            PostQuitMessage(0);
        }
        break;

    case WM_LBUTTONDOWN:
        {
            if (wnd)
            {
                ::SetCapture(hWnd);
                if ( MK_CONTROL & wParam)
                {
                    wnd->on_button_6_down();
                }
                else
                {
                    wnd->on_button_6_up();
                }

                wnd->on_mouse_down( MK_LBUTTON );
            }

            break;
        }

    case WM_LBUTTONUP:
        {
            if (wnd)
            {
                ::ReleaseCapture();
                if ( MK_CONTROL & wParam)
                {
                    wnd->on_button_6_down();
                }
                else
                {
                    wnd->on_button_6_up();
                }

                wnd->on_mouse_up(MK_LBUTTON);
            }

            break;
        }

    case WM_RBUTTONDOWN :
        {
            if (wnd)
            {
                ::SetCapture(hWnd);
                if ( MK_CONTROL & wParam)
                {
                    wnd->on_button_6_down();
                }
                else
                {
                    wnd->on_button_6_up();
                }

                wnd->on_mouse_down(MK_RBUTTON);
            }
            break;
        }
    case WM_RBUTTONUP :
        {
            if (wnd)
            {
                ::ReleaseCapture();
                if ( MK_CONTROL & wParam)
                {
                    wnd->on_button_6_down();
                }
                else
                {
                    wnd->on_button_6_up();
                }

                wnd->on_mouse_up(MK_RBUTTON);
            }
            break;
        }

    case WM_MBUTTONDOWN :
        {
            if (wnd)
            {
                ::SetCapture(hWnd);
                if ( MK_CONTROL & wParam)
                {
                    wnd->on_button_6_down();
                }
                else
                {
                    wnd->on_button_6_up();
                }

                wnd->on_mouse_down(MK_MBUTTON);
            }
            break;
        }

    case WM_MBUTTONUP :
        {
            if (wnd)
            {
                ::ReleaseCapture();
                if ( MK_CONTROL & wParam)
                {
                    wnd->on_button_6_down();
                }
                else
                {
                    wnd->on_button_6_up();
                }

                wnd->on_mouse_up(MK_MBUTTON);
            }
            break;
        }

    case WM_MOUSEMOVE :
        {
            if (wnd)
            {

                if ( MK_CONTROL & wParam)
                {
                    wnd->on_button_6_down();
                }
                else
                {
                    wnd->on_button_6_up();
                }

                WPARAM mouse_state = wParam;
                WORD xPos = GET_X_LPARAM(lParam); 
                WORD yPos = GET_Y_LPARAM(lParam); 
                wnd->on_mouse_move( static_cast<uint32_t> (mouse_state) , xPos, yPos );
            }

            break;
        }
    case WM_CAPTURECHANGED:
        {
            if( ( HWND )lParam != hWnd )
            {
                ReleaseCapture();
            }
            break;
        }
    case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case 'W':
                            if (wnd)
                            {
                                wnd->on_button_0_down();
                                break;
                            }
                case 'S':
                            if (wnd)
                            {
                                wnd->on_button_1_down();
                                break;
                            }
                case 'A':
                            if (wnd)
                            {
                                wnd->on_button_2_down();
                                break;
                            }
                case 'D':
                            if (wnd)
                            {
                                wnd->on_button_3_down();
                                break;
                            }
                case 'Q':
                            if (wnd)
                            {
                                wnd->on_button_4_down();
                                break;
                            }
                case 'E':
                            if (wnd)
                            {
                                wnd->on_button_5_down();
                                break;
                            }
            }

            break;
        }
    case WM_KEYUP:
        {
            switch (wParam)
            {
                case 'W':
                            if (wnd)
                            {
                                wnd->on_button_0_up();
                                break;
                            }
                case 'S':
                            if (wnd)
                            {
                                wnd->on_button_1_up();
                                break;
                            }
                case 'A':
                            if (wnd)
                            {
                                wnd->on_button_2_up();
                                break;
                            }
                case 'D':
                            if (wnd)
                            {
                                wnd->on_button_3_up();
                                break;
                            }
                case 'Q':
                            if (wnd)
                            {
                                wnd->on_button_4_up();
                                break;
                            }
                case 'E':
                            if (wnd)
                            {
                                wnd->on_button_5_up();
                                break;
                            }
            }

            break;
        }

    
    default:

        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

// Message handler for about box.
static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

