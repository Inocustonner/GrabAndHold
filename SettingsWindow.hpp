#ifndef _SETTINGS_WINDOW_HPP
#pragma once
#define _SETTINGS_WINDOW_HPP
#define CHECKBOX_CLIENTRESOLUTION 0xc
#define TEXTFIELD_WIDTH 0x33
#define TEXTFIELD_HEIGHT 0x41
#define CHECKBOX_FULLSCREEN 0xf
#define CHECKBOX_BLUE 0x9
#define BUTTON_STARTGAME 0x2
#include <Windows.h>
/* Start window that will ask you for the settings */
class SettWnd
{
public:
	struct Settings
	{
		INT width, height;
		BOOL fullscreen;
		BOOL gay;
	};

	SettWnd() = default;
	SettWnd(SettWnd& wnd) = default;
	bool Initialize(HINSTANCE hInstance, INT width, INT height);
	UINT Run();
	void FreezeWindow();
	Settings GetSettings();
	~SettWnd();
private:
	static LRESULT CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool RegisterWindow(LPCSTR classname, HINSTANCE hInstance);
	HWND m_hWnd;
	Settings m_settings;
};
#endif