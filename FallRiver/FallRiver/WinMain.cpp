#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


#include "resource.h"

#include <windows.h>	//	Needed for Windows Applications.
#include <tchar.h>

#include <dbghelp.h>	//	Used for Dump File
#pragma comment(lib, "dbghelp.lib")

#include "CGame.h"

const TCHAR* g_szWINDOW_CLASS_NAME	= _T("SGP");			//	Window Class Name.

const TCHAR* g_szWINDOW_TITLE		= _T("Fall River - I Play Dat");		//	Window Title.
const int	g_nWINDOW_WIDTH			= 800;							//	Window Width.
const int	g_nWINDOW_HEIGHT		= 600;							//	Window Height.


//	Windowed or Full screen depending on project setting
#ifdef _DEBUG	
	bool	g_bIS_WINDOWED			= true;						
#else
	bool	g_bIS_WINDOWED			= true;
#endif

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//	This is the main message handler of the system.
	PAINTSTRUCT	ps;			//	Used in WM_PAINT.
	HDC			hdc;		//	Handle to a device context.

	//	What is the message 
	switch(msg)
	{
		//	To skip ALT pop up menu (system menu)
		case WM_SYSKEYUP:
		case WM_SYSCHAR:
			return(0);
		break;
		
		//	Handle ALT+F4
		case WM_CLOSE:
		{
			// Sends us a WM_DESTROY
			DestroyWindow(hWnd);			
		}
		break;

		//	and lose/gain focus
		case WM_ACTIVATE:
		{
			//	gaining focus
			if (LOWORD(wParam) != WA_INACTIVE)
			{
				g_bIS_WINDOWED = !g_bIS_WINDOWED;
				PostMessage( hWnd, WM_USER, NULL, NULL);
				// unpause game code here
			}
			else // losing focus
			{
				// pause game code here
			}
		}
		break;

		case WM_CREATE: 
		{
			//	Do initialization here
			return(0);
		}
		break;

		case WM_PAINT:
		{
			//	Start painting
			hdc = BeginPaint(hWnd,&ps);

			//	End painting
			EndPaint(hWnd,&ps);
			return(0);
		}
		break;

		case WM_USER:
			{

				g_bIS_WINDOWED = !g_bIS_WINDOWED;
				DWORD windowStyle = WS_VISIBLE;
				if( g_bIS_WINDOWED )
				{
					windowStyle |= WS_OVERLAPPEDWINDOW;
					ShowCursor(true);
					SetWindowLong( hWnd, GWL_STYLE, windowStyle);
					SetWindowPos(hWnd, HWND_TOP, 0, 0, g_nWINDOW_WIDTH, g_nWINDOW_HEIGHT, SWP_SHOWWINDOW);
				}
				else if( !g_bIS_WINDOWED )
				{
					DEVMODE* screenRes = new DEVMODE();
					screenRes->dmSize = sizeof(DEVMODE);
					screenRes->dmDriverExtra = sizeof(DEVMODE);
					EnumDisplaySettings( 0, ENUM_CURRENT_SETTINGS, screenRes);

					windowStyle |= WS_POPUP;
					ShowCursor(false);
					SetWindowLong( hWnd, GWL_STYLE, windowStyle);
					SetWindowPos(hWnd, HWND_TOP, 0, 0, screenRes->dmPelsWidth, screenRes->dmPelsHeight, SWP_SHOWWINDOW);
					delete screenRes;
				}
				
				UpdateWindow(hWnd);
			}
			break;

		case WM_DESTROY: 
		{
			//	Kill the application			
			PostQuitMessage(0);
			return(0);
		}
		break;

		default:
		break;
	}

	//	Process any messages that we didn't take care of 
	return (DefWindowProc(hWnd, msg, wParam, lParam));
}

//	Checks to see if the game was already running in another window.
//
//	NOTE:	Don't call this function if your game needs to have more
//			than one instance running on the same computer (i.e. client/server)
BOOL CheckIfAlreadyRunning(void)
{
	//	Find a window of the same window class name and window title
	HWND hWnd = FindWindow(g_szWINDOW_CLASS_NAME, g_szWINDOW_TITLE);

	//	If one was found
	if (hWnd)
	{
		//	If it was minimized
		if (IsIconic(hWnd))
			//	restore it
			ShowWindow(hWnd, SW_RESTORE);

		//	Bring it to the front
		SetForegroundWindow(hWnd);

		return TRUE;
	}

	//	No other copies found running
	return FALSE;
}


BOOL RegisterWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEX	winClassEx;	//	This will describe the window class we will create.

	//	First fill in the window class structure
	winClassEx.cbSize			= sizeof(winClassEx);
	winClassEx.style			= CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winClassEx.lpfnWndProc		= WindowProc;
	winClassEx.cbClsExtra		= 0;
	winClassEx.cbWndExtra		= 0;
	winClassEx.hInstance		= hInstance;
	winClassEx.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	//winClassEx.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	winClassEx.hIconSm			= NULL;
	winClassEx.hCursor			= LoadCursor(NULL, IDC_ARROW);
	winClassEx.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	winClassEx.lpszMenuName		= NULL; 
	winClassEx.lpszClassName	= g_szWINDOW_CLASS_NAME;

	_CrtDumpMemoryLeaks();

	//	Register the window class
	return RegisterClassEx(&winClassEx);
}

//	Creates and sizes the window appropriately depending on if 
//	the application is windowed or full screen.
HWND MakeWindow(HINSTANCE hInstance)
{
	// Setup window style flags
	DWORD dwWindowStyleFlags = WS_VISIBLE;

	if (g_bIS_WINDOWED)
	{
		dwWindowStyleFlags |= WS_OVERLAPPEDWINDOW;
	}
	else
	{
		dwWindowStyleFlags |= WS_POPUP;
		ShowCursor(TRUE);	// Stop showing the mouse cursor
	}

	// Setup the desired client area size
	RECT rWindow;
	rWindow.left	= 0;
	rWindow.top		= 0;
	rWindow.right	= g_nWINDOW_WIDTH;
	rWindow.bottom	= g_nWINDOW_HEIGHT;

	// Get the dimensions of a window that will have a client rect that
	// will really be the resolution we're looking for.
	AdjustWindowRectEx(&rWindow, 
						dwWindowStyleFlags,
						FALSE, 
						WS_EX_APPWINDOW);
	
	// Calculate the width/height of that window's dimensions
	int nWindowWidth	= rWindow.right - rWindow.left;
	int nWindowHeight	= rWindow.bottom - rWindow.top;

	//	Create the window
	return CreateWindowEx(WS_EX_APPWINDOW,											//	Extended Style flags.
						  g_szWINDOW_CLASS_NAME,									//	Window Class Name.
						  g_szWINDOW_TITLE,											//	Title of the Window.
						  dwWindowStyleFlags,										//	Window Style Flags.
						  (GetSystemMetrics(SM_CXSCREEN)/2) - (nWindowWidth/2),		//	Window Start Point (x, y). 
						  (GetSystemMetrics(SM_CYSCREEN)/2) - (nWindowHeight/2),	//		-Does the math to center the window over the desktop.
						  nWindowWidth,												//	Width of Window.
						  nWindowHeight,											//	Height of Window.
						  NULL,														//	Handle to parent window.
						  NULL,														//	Handle to menu.
						  hInstance,												//	Application Instance.
						  NULL);													//	Creation parameters.
}

//////////////////////////
//		WinMain			//
//////////////////////////

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetBreakAlloc(-1L);

	lpCmdLine;
	MSG		msg;	//	Generic message.
	HWND	hWnd;	//	Main Window Handle.


	//	if in release mode set the exception filter to write out a dump file
#ifndef _DEBUG
	//SetUnhandledExceptionFilter(Handler);
#endif

	// Don't let more than one instance of the application exist
	//
	// NOTE:	Comment out the following section of code if your game needs to have more
	//			than one instance running on the same computer (i.e. client/server)
	////////////////////////////////////////////////////////////////////////
	if (!hPrevInstance)
	{
		if (CheckIfAlreadyRunning())
			return FALSE;
	}
	////////////////////////////////////////////////////////////////////////

	//	Register the window class
	if (!RegisterWindowClass(hInstance))
		return 0;

	//	Create the window
	hWnd = MakeWindow(hInstance);

	if (!hWnd)
		return 0;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//////////////////////////////////////////
	//	Initialize Game here
	//////////////////////////////////////////
	//g_bIS_WINDOWED = !g_bIS_WINDOWED;
	CGame* pGame = CGame::GetInstance();
	pGame->Initialize( hWnd, hInstance,
					   g_nWINDOW_WIDTH, g_nWINDOW_HEIGHT,
					   !g_bIS_WINDOWED);


	//////////////////////////////////////////

	//	Enter main event loop
	bool test = true;
	while (test)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{ 
			//	Test if this is a quit
			if (msg.message == WM_QUIT)
				break;
		
			//	Translate any accelerator keys
			TranslateMessage(&msg);

			//	Send the message to the window proc
			DispatchMessage(&msg);
		}
		
		//////////////////////////////////
		//	Put Game Logic Here
		//////////////////////////////////

		if( pGame->Main() == false )
			PostQuitMessage( 0 );
		
		//////////////////////////////////
	}

	/////////////////////////////////////////
	//	Shutdown Game Here
	/////////////////////////////////////////
	
	pGame->ShutDown();
	pGame = nullptr;

	/////////////////////////////////////////
	
	
	//	Unregister the window class
	UnregisterClass(g_szWINDOW_CLASS_NAME, hInstance);
	
	//	Return to Windows like this.
	return (int)(msg.wParam);
}
