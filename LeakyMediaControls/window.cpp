#include <iostream>
#include <windows.h>
#include <shellapi.h>
#include "LeakyMediaControls.h"

#define MAX_LOADSTRING 100

HWND g_hwnd;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	TrayWindow(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_LEAKYMEDIACONTROLS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LEAKYMEDIACONTROLS));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LEAKYMEDIACONTROLS));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LEAKYMEDIACONTROLS);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	/*
	CreateWindow(TEXT("button"), TEXT("Show Title"),
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		10, 20, 185, 35,
		//hWnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hWnd, (HMENU)1, GetModuleHandle(NULL), NULL);

	CreateWindow(TEXT("edit"), TEXT("Show Title"),
		WS_VISIBLE | WS_CHILD | BS_TEXT,
		10, 50, 185, 35,
		//hWnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hWnd, (HMENU)1, GetModuleHandle(NULL), NULL);
		*/


	std::wstring prevKey(L"n/a");
	if (g_prevSongHotkey == VK_F1)
	{
		prevKey = std::wstring(L"F1");
	}

	CreateWindow(TEXT("STATIC"), TEXT("Previous Song Hotkey"),
		WS_VISIBLE | WS_CHILD | BS_TEXT,
		10, 10, 160, 25,
		//hWnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hWnd, (HMENU)1, GetModuleHandle(NULL), NULL);

	CreateWindowEx(WS_EX_CLIENTEDGE, L"Edit", prevKey.c_str(), WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		10, 40, 30, 21, hWnd, NULL, NULL, NULL);

	CreateWindow(TEXT("button"), TEXT("Set Key"),
		WS_VISIBLE | WS_CHILD | BS_TEXT,
		10, 70, 80, 25,
		//hWnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hWnd, (HMENU)1, GetModuleHandle(NULL), NULL);




	std::wstring nextKey(L"n/a");
	if (g_nextSongHotkey == VK_F2)
	{
		nextKey = std::wstring(L"F2");
	}

	CreateWindow(TEXT("STATIC"), TEXT("Next Song Hotkey"),
		WS_VISIBLE | WS_CHILD | BS_TEXT,
		200, 10, 160, 25,
		//hWnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hWnd, (HMENU)1, GetModuleHandle(NULL), NULL);

	CreateWindowEx(WS_EX_CLIENTEDGE, L"Edit", nextKey.c_str(), WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		200, 40, 30, 21, hWnd, NULL, NULL, NULL);

	CreateWindow(TEXT("button"), TEXT("Set Key"),
		WS_VISIBLE | WS_CHILD | BS_TEXT,
		200, 70, 80, 25,
		//hWnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hWnd, (HMENU)1, GetModuleHandle(NULL), NULL);





	std::wstring toggleDefaultSoundOutputDevice(L"n/a");
	if (g_toggleDefaultSoundOutputDeviceHotkey == VK_F12)
	{
		toggleDefaultSoundOutputDevice = std::wstring(L"F12");
	}

	CreateWindow(TEXT("STATIC"), TEXT("Toggle Default Sound Output Device Hotkey"),
		WS_VISIBLE | WS_CHILD | BS_TEXT,
		400, 10, 320, 25,
		//hWnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hWnd, (HMENU)1, GetModuleHandle(NULL), NULL);

	CreateWindowEx(WS_EX_CLIENTEDGE, L"Edit", toggleDefaultSoundOutputDevice.c_str(), WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		400, 40, 30, 21, hWnd, NULL, NULL, NULL);

	CreateWindow(TEXT("button"), TEXT("Set Key"),
		WS_VISIBLE | WS_CHILD | BS_TEXT,
		400, 70, 80, 25,
		//hWnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hWnd, (HMENU)1, GetModuleHandle(NULL), NULL);




	


	CheckDlgButton(hWnd, 1, BST_UNCHECKED);

	if (!hWnd)
	{
		return FALSE;
	}

	g_hwnd = hWnd;

	catch_and_show(hWnd, [&]() {
		RegisterHotkeys(hWnd);
		CreateSystemTrayIcon(hWnd);
	});

	// Don't show window
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_HOTKEY:
	{
		UINT hotkey = HIWORD(lParam);
		catch_and_show(hWnd, [&]() {
			HandleHotkeySequencePress(hotkey);
		});
	}
	break;

	case WM_MYMESSAGE:
	{
		switch (lParam)
		{
		case WM_LBUTTONDOWN:
			//case WM_LBUTTONDBLCLK:
			if (IDYES == MessageBoxW(NULL, L"Exit the program?", L"LeakyMediaControls", MB_YESNO))
			{
				DoExitSequence();
			}
			//DialogBox(hInst, MAKEINTRESOURCE(IDM_EXIT), hWnd, TrayWindow);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		};
	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
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
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:

		DestroySystemTrayIcon(hWnd);

		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
