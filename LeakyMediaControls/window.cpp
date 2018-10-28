#include <iostream>
#include <memory>
#include <windows.h>
#include <shellapi.h>
#include "LeakyMediaControls.h"
#include "LeakyMediaControls.window.h"
#include "resource.h"

HWND g_hwnd;
HINSTANCE g_hinstance;

bool InitInstance(HINSTANCE hinstance, int nCmdShow, std::wstring const & title, std::wstring const & windowClass)
{
	g_hinstance = hinstance;

	HWND hwnd = leakymediacontrols::MakeWindow(hinstance, title, windowClass);
	
	catch_and_show(hwnd, [&](HWND hwnd) {
		leakymediacontrols::Initialize();
		leakymediacontrols::FillWindow(hwnd);
	});

	if (!hwnd)
	{
		return false;
	}

	g_hwnd = hwnd;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	return true;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_HOTKEY:
	{
		UINT hotkey = HIWORD(lParam);
		catch_and_show(hwnd, [&](HWND hwnd) {
			leakymediacontrols::HandleHotkeyPress(hotkey);
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
				leakymediacontrols::DoExitSequence();
			}
			//DialogBox(hInst, MAKEINTRESOURCE(IDM_EXIT), hWnd, TrayWindow);
			break;
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
		};
	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);

		if (leakymediacontrols::WndProc(hwnd, wmId))
		{
			return 0;
		}

		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(g_hinstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, leakymediacontrols::About);
			break;
		case IDM_EXIT:
			DestroyWindow(hwnd);
			break;
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hwnd, &ps);
	}
	break;
	case WM_CLOSE:
		leakymediacontrols::DestroySystemTrayIcon(hwnd);
		PostQuitMessage(0);
		break;
	case WM_DESTROY:
		leakymediacontrols::DestroySystemTrayIcon(hwnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance, std::wstring const & windowClass)
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
	wcex.lpszClassName = windowClass.c_str();
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	return RegisterClassExW(&wcex);
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	(void)hPrevInstance;
	(void)lpCmdLine;

	wchar_t szTitle[100];
	wchar_t szWindowClass[100];

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, 100);
	LoadStringW(hInstance, IDC_LEAKYMEDIACONTROLS, szWindowClass, 100);

	auto title = std::wstring(szTitle);
	auto windowClass = std::wstring(szWindowClass);

	MyRegisterClass(hInstance, windowClass);

	if (!InitInstance(hInstance, nCmdShow, title, windowClass))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LEAKYMEDIACONTROLS));

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return static_cast<int>(msg.wParam);
}
