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

// holds the defaults
std::vector<a_hotkey_shape_data> g_theHotkeyDefaultsDatas = {
	{ L"PreviousTrack",					VK_F1,	[]() { win32_abstraction::SendKeyStroke(VK_MEDIA_PREV_TRACK); } },
	{ L"NextTrack",						VK_F2,	[]() { win32_abstraction::SendKeyStroke(VK_MEDIA_NEXT_TRACK); } },
	{ L"ToggleSoundOutputDevice",		VK_F6,	[]() { ToggleSoundDeviceEndpoint(); } },
};

// holds the actuals
std::vector<a_hotkey_shape_data> g_theHotkeyDatas;

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

	fs::path GetConfigFileDefaultPath()
	{
		// AppData
		CComHeapPtr<WCHAR> appdataPath;
		THROW_IF_FAILED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, NULL, &appdataPath));

		auto dataDirectory = fs::path(static_cast<LPWSTR>(appdataPath)) / L"LeakyMediaControls";
		auto configFile = dataDirectory / L"config.ini";
		return configFile;
	}

	void WriteConfigFile(fs::path const & configFilePath, std::vector<a_hotkey_shape_data> const & hotkeyBindings)
	{
		std::wofstream configFile;
		configFile.open(configFilePath);
		configFile << L"[settings]" << std::endl;
		configFile << LR"(; See keycodes here, https://docs.microsoft.com/en-us/windows/desktop/inputdev/virtual-key-codes)" << std::endl;
		configFile << L"; To turn off a setting, set it to zero, e.g." << std::endl;
		configFile << L";	PreviousTrack = 0" << std::endl;
		configFile << L";" << std::endl;
		configFile << L"; 112 is the keycode for F1" << std::endl;
		configFile << L"; 113 is the keycode for F2" << std::endl;
		configFile << L"; 117 is the keycode for F6" << std::endl;
		configFile << L";" << std::endl;

		for (auto const & hotkeyBinding : hotkeyBindings)
		{
			configFile << hotkeyBinding.name << L" = " << hotkeyBinding.keycode << std::endl;
		}
	}

	void WriteConfigFile(std::vector<a_hotkey_shape_data> const & hotkeyBindings)
	{
		auto configFile = GetConfigFileDefaultPath();
		WriteConfigFile(GetConfigFileDefaultPath(), hotkeyBindings);
	}

	std::unique_ptr<INIReader> ReadOrMakeConfigFile()
	{
		auto configFile = GetConfigFileDefaultPath();
		if (!fs::exists(configFile))
		{
			WriteConfigFile(configFile, g_theHotkeyDefaultsDatas);
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

		for (auto const & aHotkeyShapeData : g_theHotkeyDefaultsDatas)
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
				userDefinedHotkey = aHotkeyShapeData.keycode;
			}
			
			// make hotkey binding helper struct
			auto hotkeyBinding = hotkey_binding(userDefinedHotkey, aHotkeyShapeData.action);
			
			// save it
			g_hotkeyBindings.emplace(aHotkeyShapeData.name, hotkeyBinding);

			// save it to the datas thiny too
			auto aHotkeyData = aHotkeyShapeData;
			aHotkeyData.keycode = userDefinedHotkey;
			g_theHotkeyDatas.push_back(aHotkeyData);
		}
	}

	void UserAssignsNewHotkeyAndWriteConfig(std::wstring name, UINT keycode)
	{
		auto hotkeyData = std::find_if(g_theHotkeyDatas.begin(), g_theHotkeyDatas.end(), [&](const auto & hotkeyData) {
			return hotkeyData.name == name;
		});

		if (hotkeyData == g_theHotkeyDatas.end())
		{
			hotkeyData = std::find_if(g_theHotkeyDefaultsDatas.begin(), g_theHotkeyDefaultsDatas.end(), [&](const auto & hotkeyData) {
				return hotkeyData.name == name;
			});

			if (hotkeyData == g_theHotkeyDatas.end())
			{
				throw win32_abstraction::exception(L"Assigning hotkey to bogus action");
			}

			g_theHotkeyDatas.push_back(*hotkeyData);
		}

		hotkeyData->keycode = keycode;

		WriteConfigFile(g_theHotkeyDatas);
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
