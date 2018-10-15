#pragma once

#include <string>
#include "win32_abstraction.h"


#define WM_MYMESSAGE (WM_USER + 1)

extern HWND g_hwnd;
extern UINT g_prevSongHotkey;
extern UINT g_nextSongHotkey;
extern UINT g_toggleDefaultSoundOutputDeviceHotkey;


namespace leakymediacontrols
{
	void DoExitSequence();
	void RegisterHotkeys(HWND hWnd);
	void CreateSystemTrayIcon(HWND hWnd);
	void DestroySystemTrayIcon(HWND hWnd);
	void HandleHotkeySequencePress(UINT hotkey);
	INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
}

template <typename F>
void catch_and_show(HWND hWnd, F&& func)
{
	try
	{
		func();
	}
	catch (win32_abstraction::exception const & e)
	{
		MessageBox(hWnd, e.wide_what().c_str(), L"LeakyMediaControls failure", MB_OK);
	}
}
