﻿#include "precompiled.h"

#include <sys/sys_base.h>
#include <win32/irradiance.h>

#include <fnd/fnd_universe.h>
#include <win32/application.h>
#include <win32/window.h>


#define MAX_LOADSTRING 100

// Global Variables:
static HINSTANCE hInst;								// current instance
static TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
static TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
static ATOM				MyRegisterClass(HINSTANCE hInstance);
static HWND				InitInstance(HINSTANCE, int);
static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
static INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

using namespace wnd;


extern std::shared_ptr<gx::scene> universe_bootstrap( std::shared_ptr<fnd::universe> universe );

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    UNUSED_PARAMETER(hInstance);
	UNUSED_PARAMETER(nCmdShow);

	UNUSED_PARAMETER(hPrevInstance);
	UNUSED_PARAMETER(lpCmdLine);

	
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

    application application;

    std::shared_ptr<fnd::universe> universe = std::make_shared<fnd::universe>();
    std::shared_ptr<gx::scene> scene = universe_bootstrap( universe );

    application.set_universe(universe);
    application.set_scene(scene);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IRRADIANCE));
	
	window* wnd = new window(application, context);
	application.new_window(wnd);

	::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG>(wnd) );

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

	window* wnd = reinterpret_cast<window*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

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
			}
		}
		break;

	case WM_DESTROY:
		if (wnd)
		{
			wnd->destroy();
		}
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

