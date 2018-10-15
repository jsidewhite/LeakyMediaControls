#pragma once

#include <windows.h>

namespace leakymediacontrols
{
	HWND MakeWindow(HINSTANCE hinstance, LPCWSTR szTitle, LPCWSTR szWindowClass);
}
