// LeakyMediaControls.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <shellapi.h>
#include "LeakyMediaControls.h"

#define MAX_LOADSTRING 100

#define WM_MYMESSAGE (WM_USER + 1)

HWND g_hwnd;
void DoExitSequence();

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

void RegisterHotkeys(HWND hWnd)
{
	//0x42 is 'b'
	//if (!RegisterHotKey(hWnd, 1, MOD_ALT | MOD_NOREPEAT, 0x42))
	//if (!RegisterHotKey(hWnd, 1, MOD_ALT | MOD_NOREPEAT, VK_F2))
	if (!RegisterHotKey(hWnd, 1, MOD_NOREPEAT, VK_F2))
	{
		throw std::runtime_error("Couldn't register hotkey");
	}

	//0x56 is 'v'
	//if (!RegisterHotKey(hWnd, 1, MOD_ALT | MOD_NOREPEAT, 0x56))
	//if (!RegisterHotKey(hWnd, 1, MOD_ALT | MOD_NOREPEAT, VK_F1))
	if (!RegisterHotKey(hWnd, 1, MOD_NOREPEAT, VK_F1))
	{
		throw std::runtime_error("Couldn't register hotkey");
	}
}

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
    if (!InitInstance (hInstance, nCmdShow))
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

    return (int) msg.wParam;
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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LEAKYMEDIACONTROLS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LEAKYMEDIACONTROLS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

   if (!hWnd)
   {
      return FALSE;
   }

   g_hwnd = hWnd;
   
   RegisterHotkeys(hWnd);

   NOTIFYICONDATA nid;

   nid.cbSize = sizeof(NOTIFYICONDATA);
   nid.hWnd = hWnd;
   nid.uID = 100;
   nid.uVersion = NOTIFYICON_VERSION;
   nid.uCallbackMessage = WM_MYMESSAGE;
   //nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
   //nid.hIcon = LoadIconW(GetModuleHandle(NULL), L"small.ico");
   //nid.hIcon = LoadIconW(GetModuleHandle(NULL), IDI_SMALL);
   HICON icon = LoadIconW(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_KIRBY));
   //HICON icon = LoadIconW(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON));
   //HICON icon = LoadIconW(NULL, MAKEINTRESOURCE(IDI_LEAKYMEDIACONTROLS));
   //HICON icon = LoadIconW(NULL, MAKEINTRESOURCE(IDC_LEAKYMEDIACONTROLS));
   
   nid.hIcon = icon;
   //nid.hIcon = LoadIconW(GetModuleHandle(NULL), L"LeakyMediaControls.ico");
   wcscpy_s(nid.szTip, L"Leaky Media Controls");
   //nid.uFlags = NIF_MESSAGE  NIF_ICON NIF_TIP;
   nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

   Shell_NotifyIcon(NIM_ADD, &nid);



   //ShowWindow(hWnd, nCmdShow);
   //UpdateWindow(hWnd);

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
			UINT val = HIWORD(lParam);

			WORD specialKey;
			//if (val == 0x42)
			if (val == VK_F2)
			{
				_tprintf(_T("next track hotkey pressed.\n"));
				specialKey = VK_MEDIA_NEXT_TRACK;
			}
			//if (val == 0x56)
			if (val == VK_F1)
			{
				_tprintf(_T("prev track hotkey pressed.\n"));
				specialKey = VK_MEDIA_PREV_TRACK;
			}

			INPUT input[1];
			memset(input, 0, sizeof(input));
			input[0].type = INPUT_KEYBOARD;

			input[0].ki.wVk = specialKey;

			input[0].ki.dwFlags = 0;
			input[0].ki.time = 0;
			input[0].ki.dwExtraInfo = 0;

			SendInput(1, input, sizeof(INPUT));
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

		NOTIFYICONDATA nid;
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hWnd;
		nid.uID = 100;
		Shell_NotifyIcon(NIM_DELETE, &nid);

        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

void DoExitSequence()
{
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = g_hwnd;
	nid.uID = 100;
	Shell_NotifyIcon(NIM_DELETE, &nid);
	PostMessage(g_hwnd, WM_CLOSE, 0, 0);
}

/*
INT_PTR CALLBACK TrayWindow(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDYES)
		{
			EndDialog(hDlg, LOWORD(wParam));
			DoExitSequence(hDlg);
			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDNO)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
*/
