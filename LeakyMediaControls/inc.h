#pragma once

#include <sstream>
#include "win32_abstraction.h"

namespace details
{
	inline void throw_if_failed(HRESULT hr, wchar_t const * file, size_t line, wchar_t const * message = nullptr)
	{
		if (SUCCEEDED(hr))
		{
			return;
		}

		std::wstringstream ss;
		if (message)
		{
			ss << message << L" - ";
		}
		ss << L"Errorcode 0x" << std::hex << hr;
		ss << L" (" << file << L":" << line << L")" << std::endl;
		throw win32_abstraction::exception(ss.str());
	}
}

#define THROW_IF_FAILED(x) ::details::throw_if_failed((x), __FILEW__, __LINE__)

#define THROW_IF_FAILED2(x) do { \
	HRESULT hr = (x); \
	if (FAILED(hr)) \
	{ \
		throw exception(L"Couldn't register hotkey"); \
	} \
} while(false)
