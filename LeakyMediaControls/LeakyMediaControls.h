#pragma once

#include "resource.h"
#include <string>


#define WM_MYMESSAGE (WM_USER + 1)

extern HWND g_hwnd;
extern UINT g_prevSongHotkey;
extern UINT g_nextSongHotkey;
extern UINT g_toggleDefaultSoundOutputDeviceHotkey;

void DoExitSequence();
void RegisterHotkeys(HWND hWnd);
void CreateSystemTrayIcon(HWND hWnd);
void DestroySystemTrayIcon(HWND hWnd);
void HandleHotkeySequencePress(UINT hotkey);

class my_exception : public std::exception
{
private:
	std::wstring errMsg;
public:
	my_exception(const std::wstring msg)
		: std::exception(std::string(msg.begin(), msg.end()).c_str())
	{
		errMsg = std::wstring(msg.begin(), msg.end());
	}

	std::wstring const & wide_what() const
	{
		return errMsg;
	}
};

template <typename F>
void catch_and_show(HWND hWnd, F&& func)
{
	try
	{
		func();
	}
	catch (my_exception const & e)
	{
		MessageBox(hWnd, e.wide_what().c_str(), L"LeakyMediaControls failure", MB_OK);
	}
}
