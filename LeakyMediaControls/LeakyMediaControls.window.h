#pragma once

#include <string>
#include <windows.h>

namespace leakymediacontrols
{
	void Initialize();
	HWND MakeWindow(HINSTANCE hinstance, std::wstring const & title, std::wstring const & windowClass);
	void DoExitSequence();
	void CreateSystemTrayIcon(HWND hWnd);
	void DestroySystemTrayIcon(HWND hWnd);
	INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
}
