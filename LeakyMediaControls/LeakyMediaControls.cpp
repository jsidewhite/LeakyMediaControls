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

std::vector<a_hotkey_shape_data> g_theHotkeyShapeDatas = {
	{ L"PreviousTrack",					VK_F1,	[]() { win32_abstraction::SendKeyStroke(VK_MEDIA_PREV_TRACK); } },
	{ L"NextTrack",						VK_F2,	[]() { win32_abstraction::SendKeyStroke(VK_MEDIA_NEXT_TRACK); } },
	{ L"ToggleSoundOutputDevice",		VK_F6,	[]() { ToggleSoundDeviceEndpoint(); } },
};

namespace leakymediacontrols
{
	namespace details
	{
		static std::string to_string(std::wstring const & wstr)
		{
			// Super dirty and lame conversion
			std::vector<char> chars(wstr.size());
			std::transform(wstr.begin(), wstr.end(), chars.begin(), [](auto const & ch) {
				return static_cast<char>(ch);
			});
			return { chars.data() };
		}
	}

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

		for (auto const & aHotkeyShapeData : g_theHotkeyShapeDatas)
		{
			auto name = details::to_string(aHotkeyShapeData.name);

			auto userDefinedHotkey = reader->GetInteger("settings", name, -1);
			if (userDefinedHotkey == 0)
			{
				// skip registering hotkey (user has set the hotkey to zero, explicitly)
				continue;
			}
			
			if (userDefinedHotkey == -1)
			{
				// hotkey wasn't specified in config file at all, set it to the default value
				userDefinedHotkey = aHotkeyShapeData.default_keycode;
			}
			
			// make hotkey binding helper struct
			auto hotkeyBinding = hotkey_binding(userDefinedHotkey, aHotkeyShapeData.action);
			
			// save it
			g_hotkeyBindings.emplace(aHotkeyShapeData.name, hotkeyBinding);
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
