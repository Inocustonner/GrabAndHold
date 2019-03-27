#ifndef _MAIN_WINDOW_HPP_
#pragma once
#define _MAIN_WINDOW_HPP_
#include <Windows.h>
#include "Object.hpp"
using NObject = ObjectSpace::Object; // non-static object

class MainWindow
{
public:
	MainWindow() = default;
	~MainWindow();
	MainWindow(MainWindow&) = delete;
	MainWindow(MainWindow&&) = delete;
	bool Initialize(HINSTANCE hInstance, INT width, INT height, BOOL fullscreen);
	bool CreateObjects(SHORT count, LPCSTR* path, LPCSTR* maskPaths, COORD* positions, COORD* grabPoints);
	void Run();
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
private:
	bool InitializeWindow(HINSTANCE hInstance, INT width, INT height, BOOL fullscreen);
	LPCSTR m_WindowName = "Grab and Hold";
	HWND m_hWnd;
	DWORD m_style;
	NObject** m_ppobjs;
	SHORT m_objsCnt;
};
#endif