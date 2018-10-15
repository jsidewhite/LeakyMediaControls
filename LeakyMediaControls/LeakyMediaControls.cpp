#include <vector>
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

	struct hotkey_binding
	{
		UINT hotkey;
		UINT action;
	};

	std::vector<hotkey_binding> hotkeyBindings;

	void RegisterHotkeys(HWND hwnd)
	{
		win32_abstraction::RegisterHotkey(hwnd, g_nextSongHotkey);
		win32_abstraction::RegisterHotkey(hwnd, g_prevSongHotkey);
		win32_abstraction::RegisterHotkey(hwnd, g_toggleDefaultSoundOutputDeviceHotkey);
	}

	void HandleHotkeyPress(UINT hotkey)
	{
		WORD specialKey;

		if (hotkey == g_nextSongHotkey)
		{
			specialKey = VK_MEDIA_NEXT_TRACK;
		}
		else if (hotkey == g_prevSongHotkey)
		{
			specialKey = VK_MEDIA_PREV_TRACK;
		}
		else if (hotkey == g_toggleDefaultSoundOutputDeviceHotkey)
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

}
