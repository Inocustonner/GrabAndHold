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
	BOOL InitBackBuffer();
	BOOL Initialize(HINSTANCE hInstance, INT width, INT height, BOOL fullscreen);
	BOOL CreateObjects(SHORT count, LPCSTR* path, LPCSTR* maskPaths, COORD* positions, COORD* grabPoints);
	void Draw(HWND hWnd, LPRECT lr, BOOL repaintBG);
	void Run();
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
private:
	bool InitializeWindow(HINSTANCE hInstance, INT width, INT height, BOOL fullscreen);
	LPCSTR m_WindowName = "Grab and Hold";
	HWND m_hWnd;
	DWORD m_style;
	NObject** m_ppobjs;
	NObject* m_chosen = nullptr;
	SHORT m_objsCnt;

	HBITMAP m_backGround;
	HDC m_backBufferDC;
	HBITMAP m_backBuffer;
};
#endif