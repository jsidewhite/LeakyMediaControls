#include <algorithm>
#include <vector>
#include <windows.h>
#include <shellapi.h>
#include "LeakyMediaControls.h"
#include "SoundDeviceEndpoint.h"
#include "win32_abstraction.h"
#include "functional"
#include "resource.h"

UINT g_prevSongHotkey = VK_F1;
UINT g_nextSongHotkey = VK_F2;
UINT g_toggleDefaultSoundOutputDeviceHotkey = VK_F6;

namespace leakymediacontrols
{

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

	std::vector<hotkey_binding> g_hotkeyBindings;

	void Initialize()
	{
		g_hotkeyBindings.push_back(hotkey_binding(VK_F1, []() {
			win32_abstraction::SendKeyStroke(VK_MEDIA_PREV_TRACK);
		}));

		g_hotkeyBindings.push_back(hotkey_binding(VK_F2, []() {
			win32_abstraction::SendKeyStroke(VK_MEDIA_NEXT_TRACK);
		}));

		g_hotkeyBindings.push_back(hotkey_binding(VK_F6, []() {
			ToggleSoundDeviceEndpoint();
		}));
	}

	void RegisterHotkeys(HWND hwnd)
	{
		for (auto& hotkeyBinding : g_hotkeyBindings)
		{
			win32_abstraction::RegisterHotkey(hwnd, hotkeyBinding.m_hotkey);
		}
	}

	void HandleHotkeyPress(UINT hotkey)
	{
		auto hotkeyBinding = std::find_if(g_hotkeyBindings.begin(), g_hotkeyBindings.end(), [hotkey](auto const & hotkeyBinding) {
			return hotkeyBinding.m_hotkey == hotkey;
		});

		if (hotkeyBinding == g_hotkeyBindings.end())
		{
			throw win32_abstraction::exception(L"Unknown hotkey");
		}

		hotkeyBinding->m_action();
	}

}
