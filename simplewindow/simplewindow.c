/**
 * SimpleWindow v1.0
 * @file
 *
 * 09-24-2008:	JPH - Created.
 *
 * @author Jacob Hammack
 */
 
#include <windows.h>

/**
 * Forces the compiler to link these libraries
 *
 */
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "kernel32.lib")

/**
 * WindowsMessageLoop processes 
 *
 * @author Jacob Hammack
 */
LRESULT CALLBACK WindowsMessageLoop(HWND WindowHandle, UINT Message, 
                          WPARAM WindowParameters, LPARAM MoreWindowsParameters)
{
	switch(Message)
	{
		case WM_CREATE:
			return 0;

		case WM_PAINT:
			return 0;
		
		case WM_SIZE:
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(WindowHandle, Message, WindowParameters, MoreWindowsParameters);
}

/**
 * WinMain is the main() equivilent for a windows program, execution starts here.
 *
 * @author Jacob Hammack
 */
int WINAPI WinMain (HINSTANCE CurrentInstance, HINSTANCE PreviousInstance, 
                                            PSTR CommandLine, int CommandShow)
{
	static TCHAR ApplicationName[] = TEXT("SimpleWindow");
	HWND WindowHandle;
	MSG Message;
	WNDCLASS WindowsClass;
	
	WindowsClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowsClass.lpfnWndProc = WindowsMessageLoop;
	WindowsClass.cbClsExtra = 0;
	WindowsClass.cbWndExtra = 0;
	WindowsClass.hInstance = CurrentInstance;
	WindowsClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WindowsClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WindowsClass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	WindowsClass.lpszMenuName = NULL;
	WindowsClass.lpszClassName = ApplicationName;
	
	if(!RegisterClass(&WindowsClass))
	{
		MessageBox(NULL, TEXT("Unable to create a window."), ApplicationName, MB_ICONERROR);
		
		return 0;
	}
	
    WindowHandle = CreateWindow(ApplicationName,        /* Window Class Name*/
                                TEXT("Simple Window"),  /* Window Caption */
                                WS_OVERLAPPEDWINDOW,    /* Window Style*/
                                CW_USEDEFAULT,          /* Initial X position*/
                                CW_USEDEFAULT,          /* Initial Y position */
                                300,                    /* Initial Width of the window*/
                                100,                    /* Initial Height of the window */
                                NULL,                   /* Parent Window Handle */
                                NULL,                   /* Window Menu Handle */
                                CurrentInstance,        /* Instance of the Program Handle*/
                                NULL);                  /* Window Creation Parameters */
	
	ShowWindow(WindowHandle, CommandShow);
	UpdateWindow(WindowHandle);

	while(GetMessage(&Message, NULL, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	return Message.wParam;
}
