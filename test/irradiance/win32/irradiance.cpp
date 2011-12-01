// irradiance.cpp : Defines the entry point for the application.
//

#include "precompiled.h"

#include <boost/noncopyable.hpp>

#include "win32/irradiance.h"
#include <intrin.h>
#include <math.h>
#include <cstdint>



#include "dx11/dx11_error.h"
#include "dx11/dx11_system.h"

#include <D3DX10math.h>


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
static ATOM				MyRegisterClass(HINSTANCE hInstance);
static HWND				InitInstance(HINSTANCE, int);
static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
static INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);




class window;

class application : public boost::noncopyable
{
	public:
	explicit application()
	{

	}

	void	update()
	{

	}

	void	render()
	{

	}

	private:
	std::vector< std::unique_ptr<window> > m_windows;
};


class window
{
	public:
	explicit window(application& application, dx11::system_context system_context) : m_application(application), m_system_context(system_context)
	{

	}

	void resize_window(std::uint32_t width, std::uint32_t height)
	{
		using namespace dx11;
		DXGI_SWAP_CHAIN_DESC desc;
		
		check_result<d3d11_exception>(m_system_context.m_swap_chain->GetDesc(&desc));
		check_result<d3d11_exception>(m_system_context.m_swap_chain->ResizeBuffers(desc.BufferCount, width, height,  desc.BufferDesc.Format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
	}

	void render()
	{

	}

	private:
	application&				m_application;
	dx11::system_context		m_system_context;
};


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IRRADIANCE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	HWND hwnd = InitInstance (hInstance, nCmdShow);

	if (hwnd == 0)
	{
		return FALSE;
	}

	dx11::system_context context = dx11::create_system_context(hwnd);
	swap_chain_handler	swap_chain(context.m_swap_chain, context.m_hwnd);

	application application(context);

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IRRADIANCE));

	window_handler window_handler(swap_chain, &application);

	LONG_PTR ptr = ::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG>(&window_handler) );

	MSG msg = {};

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
				application.update();
				application.render();
			}
		}
		else
		{
				application.update();
				application.render();
		}
	}

	return (int) msg.wParam;
}

static ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IRRADIANCE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_IRRADIANCE);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

static HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return 0;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return hWnd;
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	window_handler* handler = reinterpret_cast<window_handler*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

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
		if (handler !=0 )
		{
			handler->render();
			ValidateRect( hWnd, NULL );
		}
		break;

	case WM_SIZE:
		{
			if (handler !=0 )
			{
				handler->m_swap_chain_handler.resize_window(LOWORD(lParam), HIWORD(lParam));
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
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

typedef enum 
{
	Up,		//(0,0,1)
	Down,	//(0,0,-1)
	Left,	//(1,0,0)
	Right,	//(-1,0,0)
	Front,	//(0,1,0)
	Back,	//(0,-1,0)
} Direction;

void InitializeCube(float r[6], float g[6], float b[6])
{
	r[Up] = 1.0f;		//red
	r[Down] = 0.0f;		//blue
	r[Left] = 1.0f;		//yellow
	r[Right] = 0.0f;	//green
	r[Front] = 1.0f;	//white
	r[Back] = 0.0f;		//black

	g[Up] = 0.0f;
	g[Down] = 0.0f;
	g[Left] = 1.0f;
	g[Right] = 1.0f;
	g[Front] = 1.0f;
	g[Back] = 0.0f;

	b[Up] = 0.0f;
	b[Down] = 1.0f;
	b[Left] = 0.0f;
	b[Right] = 0.0f;
	b[Front] = 1.0f;
	b[Back] = 0.0f;

}

inline float sh0(float x, float y, float z)
{
	return 0.282095f;
}

inline float sh1(float x, float y, float z)
{
	float  c = 0.488603f; //z
	return c * z;
}

inline float sh2(float x, float y, float z)
{
	float  c = -0.488603f; //y
	return c * y;
}

inline float sh3(float x, float y, float z)
{
	float  c = -0.488603f; //x
	return c * x;
}

void Test()
{
	float test[25];
	float r1[25];
	float g1[25];
	float b1[25];

	::memset( &test[0],0, sizeof(test) );

	::memset( &r1[0],0, sizeof(test) );
	::memset( &g1[0],0, sizeof(test) );
	::memset( &b1[0],0, sizeof(test) );

	D3DXVECTOR3 dir(-1,0,0);

	D3DXSHEvalDirection(&test[0], 2, &dir);

	D3DXSHEvalDirectionalLight(2, &dir, 1.0f, 0.0f, 0.0f, &r1[0], &g1[0], &b1[0]);

	float r[6];
	float g[6];
	float b[6];

	float fr[4];

	InitializeCube(r,g,b);

	const float pi = 3.14f;
	const float fTmp = 3.0f;//3.0f;
	const float wt = 4 / (sqrtf(fTmp)*fTmp);
	const float wtSum = 1;// * wt;

	fr[0] = r[Up] * sh0 ( 0, 0, 1 );
	fr[1] = r[Up] * sh1 ( 0, 0, 1 );
	fr[2] = r[Up] * sh2 ( 0, 0, 1 );
	fr[3] = r[Up] * sh3 ( 0, 0, 1 );

	/*

	fr[0] += r[Down] * sh0 ( 0, 0, -1 );
	fr[1] += r[Down] * sh1 ( 0, 0, -1 );
	fr[2] += r[Down] * sh2 ( 0, 0, -1 );
	fr[3] += r[Down] * sh3 ( 0, 0, -1 );

	fr[0] += r[Left] * sh0 ( -1, 0, 0 );
	fr[1] += r[Left] * sh1 ( -1, 0, 0 );
	fr[2] += r[Left] * sh2 ( -1, 0, 0 );
	fr[3] += r[Left] * sh3 ( -1, 0, 0 );

	fr[0] += r[Right] * sh0 ( 1, 0, 0 );
	fr[1] += r[Right] * sh1 ( 1, 0, 0 );
	fr[2] += r[Right] * sh2 ( 1, 0, 0 );
	fr[3] += r[Right] * sh3 ( 1, 0, 0 );

	fr[0] += r[Front] * sh0 ( 0, 1, 0 );
	fr[1] += r[Front] * sh1 ( 0, 1, 0 );
	fr[2] += r[Front] * sh2 ( 0, 1, 0 );
	fr[3] += r[Front] * sh3 ( 0, 1, 0 );

	fr[0] += r[Back] * sh0 ( 0, -1, 0 );
	fr[1] += r[Back] * sh1 ( 0, -1, 0 );
	fr[2] += r[Back] * sh2 ( 0, -1, 0 );
	fr[3] += r[Back] * sh3 ( 0, -1, 0 );
	*/

	
	fr[0] *= (4 * pi / wtSum);
	fr[1] *= (4 * pi / wtSum);
	fr[2] *= (4 * pi / wtSum);
	fr[3] *= (4 * pi / wtSum);
	

	float n[4] = {1.0f, 0.7f, 0.2f, -0.2f};

	float norm = sqrtf(n[1] * n[1] + n[2] * n[2] + n[3] * n[3]);

	//n[0] =norm;
	n[1] /=norm;
	n[2] /=norm;
	n[3] /=norm;

	float t = fr[0] * n[0] + fr[1] * n[1] + fr[2] * n[2] + fr[3] * n[3];
	int k;
	k++;
}

static void UpdateApplication(dx11::system_context context)
{

}

static void RenderApplication(dx11::system_context* context)
{

}
