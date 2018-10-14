#include <windows.h>
#include "LeakyMediaControls.h"

namespace leakymediacontrols
{
	HWND MakeWindow(HINSTANCE hinstance, LPCWSTR szTitle, LPCWSTR szWindowClass)
	{
		HWND hwnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hinstance, nullptr);

		std::wstring prevKey(L"n/a");
		if (g_prevSongHotkey == VK_F1)
		{
			prevKey = std::wstring(L"F1");
		}

		CreateWindow(TEXT("STATIC"), TEXT("Previous Song Hotkey"), WS_VISIBLE | WS_CHILD | BS_TEXT, 10, 10, 160, 25, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

		CreateWindowEx(WS_EX_CLIENTEDGE, L"Edit", prevKey.c_str(), WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 10, 40, 30, 21, hwnd, NULL, NULL, NULL);

		CreateWindow(TEXT("button"), TEXT("Set Key"), WS_VISIBLE | WS_CHILD | BS_TEXT, 10, 70, 80, 25, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

		std::wstring nextKey(L"n/a");
		if (g_nextSongHotkey == VK_F2)
		{
			nextKey = std::wstring(L"F2");
		}

		CreateWindow(TEXT("STATIC"), TEXT("Next Song Hotkey"), WS_VISIBLE | WS_CHILD | BS_TEXT, 200, 10, 160, 25, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

		CreateWindowEx(WS_EX_CLIENTEDGE, L"Edit", nextKey.c_str(), WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 200, 40, 30, 21, hwnd, NULL, NULL, NULL);

		CreateWindow(TEXT("button"), TEXT("Set Key"), WS_VISIBLE | WS_CHILD | BS_TEXT, 200, 70, 80, 25, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

		std::wstring toggleDefaultSoundOutputDevice(L"n/a");
		if (g_toggleDefaultSoundOutputDeviceHotkey == VK_F12)
		{
			toggleDefaultSoundOutputDevice = std::wstring(L"F12");
		}

		CreateWindow(TEXT("STATIC"), TEXT("Toggle Default Sound Output Device Hotkey"), WS_VISIBLE | WS_CHILD | BS_TEXT, 400, 10, 320, 25, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

		CreateWindowEx(WS_EX_CLIENTEDGE, L"Edit", toggleDefaultSoundOutputDevice.c_str(), WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 400, 40, 30, 21, hwnd, NULL, NULL, NULL);

		CreateWindow(TEXT("button"), TEXT("Set Key"), WS_VISIBLE | WS_CHILD | BS_TEXT, 400, 70, 80, 25, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

		CheckDlgButton(hwnd, 1, BST_UNCHECKED);

		catch_and_show(hwnd, [&]() {
			RegisterHotkeys(hwnd);
			CreateSystemTrayIcon(hwnd);
		});

		return hwnd;
	}
}
