#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <vector>
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <atlbase.h>
#include "inc.h"
#include "LeakyMediaControls.h"
#include "SoundDeviceEndpoint.h"
#include "win32_abstraction.h"
#include "resource.h"
#include "inih.h"

UINT g_prevSongHotkey = VK_F1;
UINT g_nextSongHotkey = VK_F2;
UINT g_toggleDefaultSoundOutputDeviceHotkey = VK_F6;

#include <iostream>
#include <sstream>

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
		namespace fs = std::experimental::filesystem;

		// AppData
		CComHeapPtr<WCHAR> appdataPath;
		THROW_IF_FAILED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, NULL, &appdataPath));

		fs::path dataDirectory = fs::path(static_cast<LPWSTR>(appdataPath)) / L"LeakyMediaControls";
		fs::create_directories(dataDirectory);

		auto configFile = dataDirectory / L"config.ini";

		INIReader reader(configFile.string());

		if (reader.ParseError() < 0) 
		{
			throw win32_abstraction::exception(L"Can't parse config.ini");
		}

		auto g_prevSongHotkey = reader.GetInteger("settings", "prevTrackKeycode", -1);
		if (g_prevSongHotkey == -1)
		{
			g_prevSongHotkey = VK_F1;
		}
		

		auto g_nextSongHotkey = reader.GetInteger("settings", "nextTrackKeycode", -1);
		if (g_nextSongHotkey == -1)
		{
			g_nextSongHotkey = VK_F2;
		}

		auto g_toggleDefaultSoundOutputDeviceHotkey = reader.GetInteger("settings", "toggleSoundOutput", -1);
		if (g_toggleDefaultSoundOutputDeviceHotkey == -1)
		{
			g_toggleDefaultSoundOutputDeviceHotkey = VK_F6;
		}

		std::wstringstream ss;
		ss << L" "<< g_toggleDefaultSoundOutputDeviceHotkey << L" " << VK_F6;
		//MessageBox(hwnd, ss.str().c_str(), L"LeakyMediaControls failure", MB_OK);
		//throw win32_abstraction::exception(ss.str().c_str());


		// Assign bindings
		if (g_prevSongHotkey > 0)
		{
			g_hotkeyBindings.push_back(hotkey_binding(g_prevSongHotkey, []() {
				win32_abstraction::SendKeyStroke(VK_MEDIA_PREV_TRACK);
			}));
		}

		if (g_nextSongHotkey != 0)
		{
			g_hotkeyBindings.push_back(hotkey_binding(g_nextSongHotkey, []() {
				win32_abstraction::SendKeyStroke(VK_MEDIA_NEXT_TRACK);
			}));
		}

		if (g_toggleDefaultSoundOutputDeviceHotkey != 0)
		{
			g_hotkeyBindings.push_back(hotkey_binding(g_toggleDefaultSoundOutputDeviceHotkey, []() {
				ToggleSoundDeviceEndpoint();
			}));
		}
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
