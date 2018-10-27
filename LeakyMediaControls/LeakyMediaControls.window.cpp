#include <windows.h>
#include "LeakyMediaControls.window.h"
#include "LeakyMediaControls.h"
#include "resource.h"

namespace leakymediacontrols
{
	//INT_PTR CALLBACK	TrayWindow(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);c

	HWND MakeWindow(HINSTANCE hinstance, std::wstring const & title, std::wstring const & windowClass)
	{
		HWND hwnd = CreateWindowW(windowClass.c_str(), title.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hinstance, nullptr);
		return hwnd;
	}

	void FillWindow(HWND hwnd)
	{
		//std::wstring prevKey(L"n/a");
		//if (g_prevSongHotkey == VK_F1)
		//{
			//prevKey = std::wstring(L"F1");
		//}

		auto prevTrackBinding = g_hotkeyBindings.find(L"PreviousTrack");
		auto pk = std::to_wstring(prevTrackBinding->second.m_hotkey);

		CreateWindow(TEXT("STATIC"), TEXT("Previous Song Hotkey"), WS_VISIBLE | WS_CHILD | BS_TEXT, 10, 10, 160, 25, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

		CreateWindowEx(WS_EX_CLIENTEDGE, L"Edit", pk.c_str(), WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 10, 40, 30, 21, hwnd, NULL, NULL, NULL);

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
		//if (g_toggleDefaultSoundOutputDeviceHotkey == VK_F12)
		//{
			//toggleDefaultSoundOutputDevice = std::wstring(L"F12");
			toggleDefaultSoundOutputDevice = std::wstring(std::to_wstring(g_toggleDefaultSoundOutputDeviceHotkey));
		//}

		CreateWindow(TEXT("STATIC"), TEXT("Toggle Default Sound Output Device Hotkey"), WS_VISIBLE | WS_CHILD | BS_TEXT, 400, 10, 320, 25, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

		CreateWindowEx(WS_EX_CLIENTEDGE, L"Edit", toggleDefaultSoundOutputDevice.c_str(), WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 400, 40, 30, 21, hwnd, NULL, NULL, NULL);

		CreateWindow(TEXT("button"), TEXT("Set Key"), WS_VISIBLE | WS_CHILD | BS_TEXT, 400, 70, 80, 25, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

		CheckDlgButton(hwnd, 1, BST_UNCHECKED);

		RegisterHotkeys(hwnd);
		CreateSystemTrayIcon(hwnd);
	}

	void CreateSystemTrayIcon(HWND hWnd)
	{
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
	}

	void DestroySystemTrayIcon(HWND hWnd)
	{
		NOTIFYICONDATA nid;
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hWnd;
		nid.uID = 100;
		Shell_NotifyIcon(NIM_DELETE, &nid);
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

}
