#pragma once

namespace DeCef {

	WNDPROC SetWndProcPtr(HWND hWnd, WNDPROC wndProc);

	int GetCefMouseModifiers(WPARAM wparam);
	int GetCefKeyboardModifiers(WPARAM wparam, LPARAM lparam);
	bool IsKeyDown(WPARAM wparam);

}
