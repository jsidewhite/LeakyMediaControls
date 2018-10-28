#pragma once

#include <functional>
#include <map>
#include <string>
#include "win32_abstraction.h"


#define WM_MYMESSAGE (WM_USER + 1)

extern HWND g_hwnd;
extern UINT g_prevSongHotkey;
extern UINT g_nextSongHotkey;
extern UINT g_toggleDefaultSoundOutputDeviceHotkey;

struct a_hotkey_shape_data
{
	std::wstring name;
	UINT keycode;
	std::function<void()> action;
};

class hotkey_binding
{
public:
	UINT m_hotkey;
	std::function<void()> m_action;

	hotkey_binding(UINT hotkey, std::function<void()> const & action) : m_hotkey(hotkey), m_action(std::move(action))
	{
	}

	void operator() ()
	{
		m_action();
	}
};

namespace leakymediacontrols
{
	extern std::map<std::wstring, hotkey_binding> g_hotkeyBindings;

	void Initialize();
	void UserAssignsNewHotkeyAndWriteConfig(std::wstring name, UINT keycode);
	void RegisterHotkeys(HWND hWnd);
	void HandleHotkeyPress(UINT hotkey);
}

template <typename F>
void catch_and_show(HWND hwnd, F&& func)
{
	try
	{
		func(hwnd);
	}
	catch (win32_abstraction::exception const & e)
	{
		MessageBox(hwnd, e.wide_what().c_str(), L"LeakyMediaControls failure", MB_OK);
	}
}
