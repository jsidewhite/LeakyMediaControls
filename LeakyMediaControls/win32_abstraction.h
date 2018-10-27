#pragma once

#include <windows.h>

namespace win32_abstraction
{
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

	class virtual_key_code
	{
		int keycode;
		std::wstring name;
	};
}
