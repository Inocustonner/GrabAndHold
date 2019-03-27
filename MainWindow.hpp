#ifndef _MAIN_WINDOW_HPP_
#pragma once
#define _MAIN_WINDOW_HPP_
#include <Windows.h>
class MainWindow
{
public:
	MainWindow() = default;
	MainWindow(MainWindow&) = delete;
	MainWindow(MainWindow&&) = delete;

	bool InitializeWindow(HINSTANCE hInstance, INT width, INT height, BOOL fullscreen);
	void Run();
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	LPCSTR m_WindowName = "Grab and Hold";
	HWND m_hWnd;
	DWORD m_style;
};
#endif