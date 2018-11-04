#pragma once

#include <algorithm>
#include <vector>
#include <windows.h>

namespace win32_abstraction
{
	class virtual_key_code
	{
	public:
		int keycode;
		std::wstring name;
	};

	inline std::vector<virtual_key_code> const & GetVirtualKeycodes()
	{
		static std::vector<virtual_key_code> vkcs = {
			{ VK_F1, L"F1" },
			{ VK_F2, L"F2" },
			{ VK_F3, L"F3" },
			{ VK_F4, L"F4" },
			{ VK_F5, L"F5" },
			{ VK_F6, L"F6" },
			{ VK_F7, L"F7" },
			{ VK_F8, L"F8" },
			{ VK_F9, L"F9" },
			{ VK_F10, L"F10" },
			{ VK_F11, L"F11" },
			{ VK_F12, L"F12" },
			{ VK_F13, L"F13" },
			{ VK_F14, L"F14" },
			{ VK_F15, L"F15" },
			{ VK_F16, L"F16" },
			{ VK_F17, L"F17" },
			{ VK_F18, L"F18" },
			{ VK_F19, L"F19" },
			{ VK_F20, L"F20" },
			{ VK_F21, L"F21" },
			{ VK_F22, L"F22" },
			{ VK_F23, L"F23" },
			{ VK_F24, L"F24" },
		};

		return vkcs;
	}

	class exception : public std::exception
	{
	private:
		std::wstring errMsg;
	public:
		exception(const std::wstring msg)
			: std::exception(std::string(msg.begin(), msg.end()).c_str())
		{
			errMsg = std::wstring(msg.begin(), msg.end());
		}

		std::wstring const & wide_what() const
		{
			return errMsg;
		}
	};

	inline void RegisterHotkey(HWND hwnd, UINT hotkey)
	{
		if (!RegisterHotKey(hwnd, 1, MOD_NOREPEAT, hotkey))
		{
			throw exception(L"Couldn't register hotkey");
		}
	}

	inline void SendKeyStroke(UINT keystroke)
	{
		INPUT input[1];
		memset(input, 0, sizeof(input));
		input[0].type = INPUT_KEYBOARD;

		input[0].ki.wVk = keystroke;

		input[0].ki.dwFlags = 0;
		input[0].ki.time = 0;
		input[0].ki.dwExtraInfo = 0;

		if (0 == SendInput(1, input, sizeof(INPUT)))
		{
			throw exception(L"Couldn't send keystroke");
		}
	}


	inline std::wstring GetVirtualKeycodeName(UINT keycode)
	{
		auto vkcs = GetVirtualKeycodes();

		auto hit = std::find_if(vkcs.begin(), vkcs.end(), [&keycode](auto const & vkc) {
			return vkc.keycode == keycode;
		});

		if (hit != vkcs.end())
			return (*hit).name;

		return L"<n/a>";
	}

	inline UINT GetVirtualKeycode(std::wstring const & name)
	{
		auto vkcs = GetVirtualKeycodes();

		auto hit = std::find_if(vkcs.begin(), vkcs.end(), [&name](auto const & vkc) {
			return vkc.name == name;
		});

		if (hit != vkcs.end())
			return (*hit).keycode;

		return NULL;
	}

	

	/*std::vector<virtual_key_code> GetVirtualKeycodes()
	{

	}*/
}
