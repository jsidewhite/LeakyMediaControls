#pragma once

#include <windows.h>
#include "LeakyMediaControls.h"

namespace leakymediacontrols
{
	HWND MakeWindow(HINSTANCE hinstance, LPCWSTR szTitle, LPCWSTR szWindowClass);
}
