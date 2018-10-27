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

namespace fs = std::experimental::filesystem;

namespace leakymediacontrols
{

	std::map<std::wstring, hotkey_binding> g_hotkeyBindings;

	void WriteDefaultConfigFile(fs::path const & configFilePath)
	{
		std::ofstream configFile;
		configFile.open(configFilePath);
		configFile << "[settings]" << std::endl;
		configFile << R"(; See keycodes here, https://docs.microsoft.com/en-us/windows/desktop/inputdev/virtual-key-codes)" << std::endl;
		configFile << "; To turn off a setting, set it to zero, e.g." << std::endl;
		configFile << ";	prevTrackKeycode = 0" << std::endl;
		configFile << ";" << std::endl;
		configFile << "; 112 is the keycode for F1" << std::endl;
		configFile << "; 113 is the keycode for F2" << std::endl;
		configFile << "; 117 is the keycode for F6" << std::endl;
		configFile << ";" << std::endl;
		configFile << "PreviousTrack = 112" << std::endl;
		configFile << "NextTrack = 113" << std::endl;
		configFile << "ToggleSoundOutputDevice = 117" << std::endl;
	}

	std::unique_ptr<INIReader> ReadOrMakeConfigFile()
	{
		// AppData
		CComHeapPtr<WCHAR> appdataPath;
		THROW_IF_FAILED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, NULL, &appdataPath));

		fs::path dataDirectory = fs::path(static_cast<LPWSTR>(appdataPath)) / L"LeakyMediaControls";
		fs::create_directories(dataDirectory);

		auto configFile = dataDirectory / L"config.ini";
		if (!fs::exists(configFile))
		{
			WriteDefaultConfigFile(configFile);
		}

		auto reader = std::make_unique<INIReader>(configFile.string());
		if (reader->ParseError() < 0)
		{
			std::wstringstream ss;
			ss << L"Can't parse config.ini.  Delete it and relaunch.  Location: " << configFile.string().c_str();
			throw win32_abstraction::exception(ss.str());
		}
		return reader;
	}

	void Initialize()
	{
		auto reader = ReadOrMakeConfigFile();

		auto g_prevSongHotkey = reader->GetInteger("settings", "PreviousTrack", -1);
		if (g_prevSongHotkey == -1)
		{
			g_prevSongHotkey = VK_F1;
		}
		
		auto g_nextSongHotkey = reader->GetInteger("settings", "NextTrack", -1);
		if (g_nextSongHotkey == -1)
		{
			g_nextSongHotkey = VK_F2;
		}

		auto g_toggleDefaultSoundOutputDeviceHotkey = reader->GetInteger("settings", "ToggleSoundOutputDevice", -1);
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
			g_hotkeyBindings.emplace(L"PreviousTrack", hotkey_binding(g_prevSongHotkey, []() {
				win32_abstraction::SendKeyStroke(VK_MEDIA_PREV_TRACK);
			}));
		}

		if (g_nextSongHotkey != 0)
		{
			g_hotkeyBindings.emplace(L"NextTrack", hotkey_binding(g_nextSongHotkey, []() {
				win32_abstraction::SendKeyStroke(VK_MEDIA_NEXT_TRACK);
			}));
		}

		if (g_toggleDefaultSoundOutputDeviceHotkey != 0)
		{
			g_hotkeyBindings.emplace(L"ToggleSoundOutputDevice", hotkey_binding(g_toggleDefaultSoundOutputDeviceHotkey, []() {
				ToggleSoundDeviceEndpoint();
			}));
		}
	}

	void RegisterHotkeys(HWND hwnd)
	{
		for (auto& hotkeyBinding : g_hotkeyBindings)
		{
			win32_abstraction::RegisterHotkey(hwnd, hotkeyBinding.second.m_hotkey);
		}
	}

	void HandleHotkeyPress(UINT hotkey)
	{
		auto hotkeyBinding = std::find_if(g_hotkeyBindings.begin(), g_hotkeyBindings.end(), [hotkey](auto const & hotkeyBinding) {
			return hotkeyBinding.second.m_hotkey == hotkey;
		});

		if (hotkeyBinding == g_hotkeyBindings.end())
		{
			throw win32_abstraction::exception(L"Unknown hotkey");
		}

		hotkeyBinding->second.m_action();
	}

}
