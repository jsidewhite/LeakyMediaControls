#include <sstream>
#include <windows.h>
#include "LeakyMediaControls.window.h"
#include "LeakyMediaControls.h"
#include "resource.h"
#include "inc.h"

class EditBox
{
	
public:
	EditBox(std::wstring const & _name) : name(_name)
	{
		static UINT magicIdSeed = 10000;
		magicId = magicIdSeed++;
	}

	std::wstring name;
	UINT magicId;
	HWND hwnd;
};

EditBox previousTrackEditbox = { L"PreviousTrack" };
EditBox nextTrackEditbox = { L"NextTrack" };
EditBox toggleDefaultSoundOutputDeviceHotkeyEditbox = { L"PreviousTrack" };

std::vector<EditBox*> editboxes = {
	&previousTrackEditbox,
	&nextTrackEditbox,
	&toggleDefaultSoundOutputDeviceHotkeyEditbox
};

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
		auto prevKey = win32_abstraction::GetVirtualKeycodeName(g_hotkeyBindings.find(L"PreviousTrack")->second.m_hotkey);

		CreateWindow(TEXT("STATIC"), TEXT("Previous Song Hotkey"), WS_VISIBLE | WS_CHILD | BS_TEXT, 10, 10, 160, 25, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

		previousTrackEditbox.hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, L"Edit", prevKey.c_str(), WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 10, 40, 30, 21, hwnd, NULL, NULL, NULL);

		CreateWindow(TEXT("button"), TEXT("Set Key"), WS_VISIBLE | WS_CHILD | BS_TEXT, 10, 70, 80, 25, hwnd, (HMENU)previousTrackEditbox.magicId, GetModuleHandle(NULL), NULL);


		auto nextKey = win32_abstraction::GetVirtualKeycodeName(g_hotkeyBindings.find(L"NextTrack")->second.m_hotkey);

		CreateWindow(TEXT("STATIC"), TEXT("Next Song Hotkey"), WS_VISIBLE | WS_CHILD | BS_TEXT, 200, 10, 160, 25, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

		nextTrackEditbox.hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, L"Edit", nextKey.c_str(), WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 200, 40, 30, 21, hwnd, NULL, NULL, NULL);

		CreateWindow(TEXT("button"), TEXT("Set Key"), WS_VISIBLE | WS_CHILD | BS_TEXT, 200, 70, 80, 25, hwnd, (HMENU)nextTrackEditbox.magicId, GetModuleHandle(NULL), NULL);


		auto toggleDefaultSoundOutputDevice = win32_abstraction::GetVirtualKeycodeName(g_hotkeyBindings.find(L"ToggleSoundOutputDevice")->second.m_hotkey);

		CreateWindow(TEXT("STATIC"), TEXT("Toggle Default Sound Output Device Hotkey"), WS_VISIBLE | WS_CHILD | BS_TEXT, 400, 10, 320, 25, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

		toggleDefaultSoundOutputDeviceHotkeyEditbox.hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, L"Edit", toggleDefaultSoundOutputDevice.c_str(), WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 400, 40, 30, 21, hwnd, NULL, NULL, NULL);

		CreateWindow(TEXT("button"), TEXT("Set Key"), WS_VISIBLE | WS_CHILD | BS_TEXT, 400, 70, 80, 25, hwnd, (HMENU)toggleDefaultSoundOutputDeviceHotkeyEditbox.magicId, GetModuleHandle(NULL), NULL);

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

	void DestroySystemTrayIcon(HWND hwnd)
	{
		NOTIFYICONDATA nid;
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hwnd;
		nid.uID = 100;
		if (FALSE == Shell_NotifyIcon(NIM_DELETE, &nid))
		{
			MessageBox(hwnd, TEXT("Shell_NotifyIcon failed"), TEXT(""), 0);
		}
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


	bool WndProc(HWND hwnd, UINT id)
	{
		for (auto & editbox : editboxes)
		{
			if (editbox->magicId == id)
			{
				wchar_t keycodeName[100];
				THROW_IF_FAILED(SendMessage(editbox->hwnd, WM_GETTEXT, 100, (LPARAM)keycodeName));

				UINT keycode = win32_abstraction::GetVirtualKeycode(keycodeName);

				//MessageBox(hwnd, TEXT("Button Pressed"), TEXT(""), 0);

				UserAssignsNewHotkeyAndWriteConfig(editbox->name, keycode);
				return true;
			}
		}
		return false;
	}

}
