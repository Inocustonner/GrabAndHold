#include "MainWindow.hpp"
bool RegisterWindow(LPCSTR wndName, HINSTANCE hInstance, LRESULT (CALLBACK *WndProc)(HWND, UINT, WPARAM, LPARAM))
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof WNDCLASSEX);
	wc.cbSize = sizeof WNDCLASSEX;
	wc.hbrBackground = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = wc.hIcon;
	wc.hInstance = hInstance;
	wc.lpszClassName = wndName;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = WndProc;
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Failed to register window class", "Error", MB_OK);
		return false;
	}
	return true;
}

bool MainWindow::InitializeWindow(HINSTANCE hInstance, INT width, INT height, BOOL fullscreen)
{
	if (!RegisterWindow(m_WindowName, hInstance, this->WndProc))
		return false;
	INT x, y;// window positnion coordinates
	m_style = fullscreen ? WS_POPUP : WS_OVERLAPPEDWINDOW;

	if (1 == fullscreen)
	{
		DEVMODE screenSettings;
		ZeroMemory(&screenSettings, sizeof DEVMODE);
		screenSettings.dmSize = sizeof screenSettings;
		screenSettings.dmPelsWidth = (ULONG)width;
		screenSettings.dmPelsHeight = (ULONG)height;
		screenSettings.dmBitsPerPel = 32;
		screenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//change display settings to full screen
		ChangeDisplaySettings(&screenSettings, CDS_FULLSCREEN);
		x = 0;
		y = 0;
	}
	else
	{
		INT monitorsW = GetSystemMetrics(SM_CXSCREEN);
		INT monitorsH = GetSystemMetrics(SM_CYSCREEN);
		/* adjust window size to client size be as specified width and height */
		RECT adjustWnd = { 0 , 0, width, height };
		AdjustWindowRect(&adjustWnd, m_style, FALSE);

		width = adjustWnd.right - adjustWnd.left;
		height = adjustWnd.bottom - adjustWnd.top;

		x = monitorsW / 2 - width / 2;
		y = monitorsH / 2 - height / 2;
	}

	m_hWnd = CreateWindow(m_WindowName, m_WindowName, m_style,
		x, y,
		width, height,
		NULL,
		NULL,
		hInstance,
		NULL);
	if (!m_hWnd)
	{
		MessageBox(NULL, "Failed to create window", "Error", MB_OK);
		return false;
	}
	UpdateWindow(m_hWnd);
	ShowWindow(m_hWnd, SW_SHOW);
	return true;
}

void MainWindow::Run()
{
	MSG msg = { 0 };
	BOOL ret;
	while (ret = GetMessage(&msg, NULL, NULL, NULL))
	{
		if (-1 == ret)
		{
			break;
		}
		else
		{
			DispatchMessage(&msg);
			TranslateMessage(&msg);
		}
	}
}

LRESULT CALLBACK MainWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0L;
	}
	case WM_ERASEBKGND:
	{
		HDC hdc = GetDC(hWnd);
		RECT rc;
		GetClientRect(hWnd, &rc);
		FillRect(hdc, &rc, CreateSolidBrush(RGB(0xFF, 0xFF, 0xFF)));
		ReleaseDC(hWnd, hdc);
		return 1L;
	}
	case WM_KEYDOWN:
	{
		if (VK_ESCAPE == wParam)
		{
			PostQuitMessage(0);
		}
		return 0L;
	}
	case WM_DESTROY:
	case WM_QUIT:
	{
		PostQuitMessage(0);
		return 0L;
	}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
