// LeakyMediaControls.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <shellapi.h>
#include "LeakyMediaControls.h"

//INT_PTR CALLBACK	TrayWindow(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

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

void HandleHotkeySequencePress(UINT hotkey)
{
	WORD specialKey;
	
	if (hotkey == VK_F2)
	{
		specialKey = VK_MEDIA_NEXT_TRACK;
	}
	else if (hotkey == VK_F1)
	{
		specialKey = VK_MEDIA_PREV_TRACK;
	}
	else
	{
		throw std::runtime_error("Unknown hotkey");
	}

	//if (val == 0x42)
	//if (val == 0x56)

	INPUT input[1];
	memset(input, 0, sizeof(input));
	input[0].type = INPUT_KEYBOARD;

	input[0].ki.wVk = specialKey;

	input[0].ki.dwFlags = 0;
	input[0].ki.time = 0;
	input[0].ki.dwExtraInfo = 0;

	SendInput(1, input, sizeof(INPUT));
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
