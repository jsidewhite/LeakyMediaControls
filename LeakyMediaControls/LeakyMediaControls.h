#pragma once

#include "resource.h"

#define WM_MYMESSAGE (WM_USER + 1)

extern HWND g_hwnd;
void DoExitSequence();

void RegisterHotkeys(HWND hWnd);
void CreateSystemTrayIcon(HWND hWnd);
void HandleHotkeySequencePress(UINT hotkey);
