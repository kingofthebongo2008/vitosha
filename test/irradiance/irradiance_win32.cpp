#include "precompiled.h"

#include <win32/irradiance.h>

#include <windowsx.h>

#include <sys/sys_base.h>

#include <dx11/dx11_system.h>
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

#define MAX_LOADSTRING 100

// Global Variables:
static HINSTANCE hInst;								// current instance
static TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
static TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
static ATOM				MyRegisterClass(HINSTANCE hInstance, LPCTSTR szWindowClass);
static HWND				InitInstance(HINSTANCE);
static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
static INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

using namespace wnd;

double PCFreq = 0.0;
__int64 CounterStart = 0;

void StartCounter()
{
    LARGE_INTEGER li;
    if(!QueryPerformanceFrequency(&li))
        std::cout << "QueryPerformanceFrequency failed!\n";

    PCFreq = double(li.QuadPart) / 1000.0;

    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
}

double GetCounter()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return double(li.QuadPart-CounterStart) / PCFreq;
}

void myInvalidParameterHandler(const wchar_t* expression,
   const wchar_t* function, 
   const wchar_t* file, 
   unsigned int line, 
   uintptr_t pReserved)
{
   wprintf(L"Invalid parameter detected in function %s."
            L" File: %s Line: %d\n", function, file, line);
   wprintf(L"Expression: %s\n", expression);
   abort();
}

math::float4x4 m_g;
math::float4x4 m_g_1;
math::float4   m_q;
math::float4   m_q_1;
math::float4   m_q_2;

extern std::shared_ptr<gx::scene> universe_bootstrap(  gx::render_context* render_context, dx11::system_context context, std::shared_ptr<fnd::universe> universe );


/// Create a Timer, which will immediately begin counting
/// up from 0.0 seconds.
/// You can call reset() to make it start over.
class Timer {
  public:
    Timer() {
      reset();
    }
    /// reset() makes the timer start over counting from 0.0 seconds.
    void reset() {
      unsigned __int64 pf;
      QueryPerformanceFrequency( (LARGE_INTEGER *)&pf );
      freq_ = 1.0 / (double)pf;
      QueryPerformanceCounter( (LARGE_INTEGER *)&baseTime_ );
    }
    /// seconds() returns the number of seconds (to very high resolution)
    /// elapsed since the timer was last created or reset().
    double seconds() {
      unsigned __int64 val;
      QueryPerformanceCounter( (LARGE_INTEGER *)&val );
      return (val - baseTime_) * freq_;
    }
    /// seconds() returns the number of milliseconds (to very high resolution)
    /// elapsed since the timer was last created or reset().
    double milliseconds() {
      return seconds() * 1000.0;
    }
  private:
    double freq_;
    unsigned __int64 baseTime_;
};

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    
    math::float4 axis = math::set(0.2f, -1.0f, 0.0f, 0.0f);
    float angle         = - 3.1415f / 20.0f;

    math::float4 q1 = math::quaternion_axis_angle(  axis, angle );

    q1 = math::quaternion_normalize(q1);

    /*
    m_g = math::quaternion_2_matrix_ref(q1);
    m_g_1 = math::quaternion_2_matrix(q1);

    math::float4 s = math::normalize3( math::vector3(-1.0f, 1.0f, 1.0f) ) ;
    math::float4 t = math::normalize3( math::vector3(0.0f, -0.5f, 1.0f) ) ;

    math::float4 rotate = math::quaternion_rotate_vector3( s, t);
    math::float4 rotated_v = math::rotate_vector3(s, rotate);

    const math::float4 v = {0.0000001f, 0.0000001f, 0.0000001f, 0.0000001f};

    math::float4x4 m = math::matrix_rotate_vector3(s, t);
    math::float4x4 m_1 = math::matrix_rotate_vector3_ref(s, t);
    math::float4  rotated_v_1 = math::mul( s, m );


    rotated_v_1 = math::add(rotated_v_1,v);
    rotated_v_1 = math::sub(rotated_v_1, v);

    rotated_v = math::add(rotated_v, v);
    rotated_v = math::sub(rotated_v, v);

    */

    //math::float4x4 m = math::quaternion_2_matrix( q1 );

    math::float4x4 m;

    m.r[0] = math::set(-1.0f, -1.0f, 1.5f, 0.0f);
    m.r[1] = math::set(0.0f, -1.5f, 1.5f, 0.0f);
    m.r[2] = math::set(0.2f, -1.1f, 0.0f, 0.0f);
    m.r[3] = math::set(0.0f, 0.0f, 0.0f, 1.0f);

    XMMATRIX m2;
    m2.r[0] = m.r[0];
    m2.r[1] = m.r[1];
    m2.r[2] = m.r[2];
    m2.r[3] = m.r[3];


    math::float4  q2 = math::matrix_2_quaternion_ref( m );
    math::float4  q4 = math::matrix_2_quaternion_ref2( m );
    math::float4  q5 = math::matrix_2_quaternion( m );

    math::float4  q3 = XMQuaternionRotationMatrix(m2);


    std::ofstream f("result.txt");
    /*
    f<<"ref:"<< result<<std::endl;
    f<<"ref2:"<<result_1<<std::endl;
    f<<"simd:"<<result_2<<std::endl;
    f<<"dx:"<<result_21<<std::endl;
    

    f<<"q2m_ref:"<<result_3<<std::endl;
    f<<"q2m:"<<result_4<<std::endl;
    */
    f.close();



   _invalid_parameter_handler oldHandler, newHandler;
   newHandler = myInvalidParameterHandler;
   oldHandler = _set_invalid_parameter_handler(newHandler);

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
	dx11::system_context	context = dx11::create_system_context(hwnd);
	//4. Create render contexts
	gx::render_context		render_context(context, 3, view_port);
	//5. Create application with a window and worlds with data
	application				application;

    std::shared_ptr<fnd::universe> universe = std::make_shared<fnd::universe>();
    std::shared_ptr<gx::scene> scene = universe_bootstrap( &render_context, context, universe );

    application.set_universe(universe);
    application.set_scene(scene);

	window* wnd = new window( application, context.m_swap_chain, &render_context );
	application.new_window(wnd);
	wnd->set_scene(scene);
	::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG>(wnd) );

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
				wnd->on_mouse_down(MK_LBUTTON);
			}

			break;
		}

	case WM_LBUTTONUP:
		{
			if (wnd)
			{
				wnd->on_mouse_up(MK_LBUTTON);
			}

			break;
		}

	case WM_RBUTTONDOWN :
		{
			if (wnd)
			{
				wnd->on_mouse_down(MK_RBUTTON);
			}
			break;
		}
	case WM_RBUTTONUP :
		{
			if (wnd)
			{
				wnd->on_mouse_up(MK_RBUTTON);
			}
			break;
		}

	case WM_MBUTTONDOWN :
		{
			if (wnd)
			{
				wnd->on_mouse_down(MK_MBUTTON);
			}
			break;
		}

	case WM_MBUTTONUP :
		{
			if (wnd)
			{
				wnd->on_mouse_up(MK_MBUTTON);
			}
			break;
		}

	case WM_MOUSEMOVE :
		{
			if (wnd)
			{
				WPARAM mouse_state = wParam;
				WORD xPos = GET_X_LPARAM(lParam); 
				WORD yPos = GET_Y_LPARAM(lParam); 
				wnd->on_mouse_move( static_cast<uint32_t> (mouse_state) , xPos, yPos );
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


