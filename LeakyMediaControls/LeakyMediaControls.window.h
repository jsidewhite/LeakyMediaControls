#pragma once

#include <string>
#include <windows.h>

namespace leakymediacontrols
{
	bool WndProc(HWND hwnd, UINT id);

	HWND MakeWindow(HINSTANCE hinstance, std::wstring const & title, std::wstring const & windowClass);
	void FillWindow(HWND hwnd);
	void DoExitSequence();
	void CreateSystemTrayIcon(HWND hWnd);
	void DestroySystemTrayIcon(HWND hWnd);
	INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	bool WndProc(HWND hwnd, UINT id);
}
