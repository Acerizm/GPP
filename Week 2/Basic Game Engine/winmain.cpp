// Programming 2D Games
// Copyright (c) 2011 by:
// Charles Kelly
// Chapter 2 "Hello World" Windows Style v1.0
// winmain.cpp

#define WIN32_LEAN_AND_MEAN
#define _CRTDBG_MAP_ALLOC       // for detecting memory leaks

#include <windows.h>
#include <stdlib.h>             // for detecting memory leaks
#include <crtdbg.h>             // for detecting memory leaks
#include "graphics.h"
#include "spacewar.h"

// Function prototypes
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int); 
bool CreateMainWindow(HWND &,HINSTANCE, int);
LRESULT WINAPI WinProc(HWND, UINT, WPARAM, LPARAM); 


HDC hdc; //handle to device context
TCHAR ch = ' '; //character entered
RECT rect; //rectangle structure
PAINTSTRUCT ps; // used in VM_Paint
bool vkKeys[256]; //state of virtual keys
DWORD backColor;



// Game pointer
Spacewar *game = NULL;
HWND hwnd = NULL;



//=============================================================================
// Starting point for a Windows application
// Parameters are:
//   hInstance - handle to the current instance of the application
//   hPrevInstance - always NULL, obsolete parameter, maintained for backwards compatibilty
//   lpCmdLine - pointer to null-terminated string of command line arguments
//   nCmdShow - specifies how the window is to be shown
//=============================================================================

bool AnotherInstance()
{
	HANDLE ourMutex;

	// Attempt to create a mutex using our unique string
	ourMutex = CreateMutex(NULL, true,
		"Use_a_different_string_here_for_each_program_48161-XYZZY");

	if (GetLastError() == ERROR_ALREADY_EXISTS)
		return true;            // another instance was found

	return false;               // we are the only instance
}

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	// Check for memory leak if debug build
	#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif


	MSG	 msg;

	// Create the game, sets up message handler
	game = new Spacewar;


	// Prevent Multiple Instances
	if (AnotherInstance())
		return false;

	// Create the window
	if (!CreateMainWindow(hwnd, hInstance, nCmdShow))
		return false;

	try {
		
		game->initialize(hwnd);     // throws GameError

		// main message loop
		int done = 0;
		while (!done)
		{
			// PeekMessage,non-blocking method for checking for Windows messages.
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				// look for quit message
				if (msg.message == WM_QUIT)
					done = 1;

				// decode and pass messages on to WinProc
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else {
				game->run(hwnd);    // run the game loop
			}
		}
		SAFE_DELETE(game);     // free memory before exit
		return msg.wParam;
	}
	catch (const GameError &err)
	{
		game->deleteAll();
		DestroyWindow(hwnd);
		MessageBox(NULL, err.getMessage(), "Error", MB_OK);

	}
	catch (...)
	{
		game->deleteAll();
		DestroyWindow(hwnd);
		MessageBox(NULL, "Unknown error occured in game.", "Error", MB_OK);
	}
	SAFE_DELETE(game);     // free memory before exit
	return 0;
}



//=============================================================================
// window event callback function
//=============================================================================
LRESULT WINAPI WinProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	return (game->messageHandler(hwnd, msg, wParam, lParam));
}

//=============================================================================
// Create the window
// Returns: false on error
//=============================================================================
bool CreateMainWindow(HWND &hwnd,HINSTANCE hInstance, int nCmdShow)
{ 
    WNDCLASSEX wcx; 
 
    // Fill in the window class structure with parameters 
    // that describe the main window. 
    wcx.cbSize = sizeof(wcx);           // size of structure 
    wcx.style = CS_HREDRAW | CS_VREDRAW;    // redraw if size changes 
    wcx.lpfnWndProc = WinProc;          // points to window procedure 
    wcx.cbClsExtra = 0;                 // no extra class memory 
    wcx.cbWndExtra = 0;                 // no extra window memory 
    wcx.hInstance = hInstance;          // handle to instance 
    wcx.hIcon = NULL; 
    wcx.hCursor = LoadCursor(NULL,IDC_ARROW);   // predefined arrow 
    wcx.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);    // black background brush 
    wcx.lpszMenuName =  NULL;           // name of menu resource 
    wcx.lpszClassName = CLASS_NAME;     // name of window class 
    wcx.hIconSm = NULL;                 // small class icon 
 
    // Register the window class. 
    // RegisterClassEx returns 0 on error.
    if (RegisterClassEx(&wcx) == 0)    // if error
        return false;

    // Create window
	// Create window
	hwnd = CreateWindow(
		CLASS_NAME,             // name of the window class
		GAME_TITLE,              // title bar text
		WS_OVERLAPPEDWINDOW,    // window style
		CW_USEDEFAULT,          // default horizontal position of window
		CW_USEDEFAULT,          // default vertical position of window
		GAME_WIDTH,           // width of window
		GAME_HEIGHT,          // height of the window
		(HWND)NULL,            // no parent window
		(HMENU)NULL,           // no menu
		hInstance,              // handle to application instance
		(LPVOID)NULL);         // no window parameters


    // if there was an error creating the window
    if (!hwnd)
        return false;

    // Show the window
    ShowWindow(hwnd, nCmdShow);

    return true;
}
