#include <windows.h>
#include <shellapi.h>
#include "LeakyMediaControls.h"
#include "SoundDeviceEndpoint.h"
#include "win32_abstraction.h"
#include "resource.h"

UINT g_prevSongHotkey = VK_F1;
UINT g_nextSongHotkey = VK_F2;
UINT g_toggleDefaultSoundOutputDeviceHotkey = VK_F6;

namespace leakymediacontrols
{
	//INT_PTR CALLBACK	TrayWindow(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	void RegisterHotkeys(HWND hwnd)
	{
		win32_abstraction::RegisterHotkey(hwnd, g_nextSongHotkey);
		win32_abstraction::RegisterHotkey(hwnd, g_prevSongHotkey);
		win32_abstraction::RegisterHotkey(hwnd, g_toggleDefaultSoundOutputDeviceHotkey);
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
		else if (hotkey == VK_F6)
		{
			ToggleSoundDeviceEndpoint();
			return;
		}
		else
		{
			throw win32_abstraction::exception(L"Unknown hotkey");
		}

		win32_abstraction::SendKeyStroke(specialKey);
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
