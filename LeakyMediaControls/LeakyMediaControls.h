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
	void RegisterHotkeys(HWND hWnd);
	void HandleHotkeyPress(UINT hotkey);
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
