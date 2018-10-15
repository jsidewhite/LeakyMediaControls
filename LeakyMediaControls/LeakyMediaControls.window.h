#pragma once

#include <windows.h>

namespace leakymediacontrols
{
	void Initialize();
	HWND MakeWindow(HINSTANCE hinstance, LPCWSTR szTitle, LPCWSTR szWindowClass);
	void DoExitSequence();
	void CreateSystemTrayIcon(HWND hWnd);
	void DestroySystemTrayIcon(HWND hWnd);
	INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
}
