#include "MainWindow.hpp"
bool RegisterWindow(LPCSTR wndName, HINSTANCE hInstance, LRESULT(CALLBACK *WndProc)(HWND, UINT, WPARAM, LPARAM))
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

MainWindow::~MainWindow()
{
	for (SHORT i = 0; i < m_objsCnt; ++i)
	{
		delete m_ppobjs[i];
	}
	delete[] m_ppobjs;
}

bool MainWindow::Initialize(HINSTANCE hInstance, INT width, INT height, BOOL fullscreen)
{
	if (!InitializeWindow(hInstance, width, height, fullscreen))
		return false;
	const int count = 2;
	LPCSTR paths[count] = { "./test.bmp", "./test2.bmp" };
	LPCSTR maskPaths[count] = { "./testMask.bmp", "./testMask.bmp" };
	COORD poses[count] = { {50, 50}, {200, 100} };
	COORD grabs[count] = { {30, 0}, {30, 0} };
	if (!CreateObjects(count, paths, maskPaths, poses, grabs))
		return false;
	return true;
}

bool MainWindow::CreateObjects(SHORT count, LPCSTR* path, LPCSTR* maskPaths, COORD* positions, COORD* grabPoints)
{
	m_objsCnt = count;
	m_ppobjs = new NObject*[m_objsCnt];
	if (!m_ppobjs)
	{
		MessageBox(NULL, "Failed to allocate memory", "Error", MB_OK);
		return false;
	}
	for (SHORT i = 0; i < m_objsCnt; ++i)
	{
		m_ppobjs[i] = new NObject;
		if (!m_ppobjs[i]->Initialize(path[i], maskPaths[i], positions[i], grabPoints[i]))
		{
			return false;
		}
	}
	return true;
}

bool MainWindow::InitializeWindow(HINSTANCE hInstance, INT width, INT height, BOOL fullscreen)
{
	if (!RegisterWindow(m_WindowName, hInstance, this->WndProc))
		return false;
	INT x, y;// window positnion coordinates
	m_style = fullscreen ? WS_POPUP : WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;

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
		this);
	if (!m_hWnd)
	{
		MessageBox(NULL, "Failed to create window", "Error", MB_OK);
		return false;
	}
	SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);/* set data for the lParam related to this window */
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
	static HDC backbuffer;/* back buffer dc */
	static HBITMAP backbufferBitmap;/* back buffer */

	static ObjectSpace::SIZE areaSize; /* area size = Client area size */
	switch (msg)
	{
	case WM_CREATE:
	{
		HDC hdc = GetDC(hWnd);
		backbuffer = CreateCompatibleDC(hdc);

		RECT rc;
		GetClientRect(hWnd, &rc);
		areaSize.width = rc.right - rc.left;
		areaSize.height = rc.bottom - rc.top;
		backbufferBitmap = CreateCompatibleBitmap(hdc, areaSize.width, areaSize.height);
		SelectObject(backbuffer, backbufferBitmap);
		ReleaseDC(hWnd, hdc);
		return 0L;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		MainWindow* mw = (MainWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

		ObjectSpace::Render(&backbuffer, mw->m_ppobjs, mw->m_objsCnt);

		HDC hdc = BeginPaint(hWnd, &ps);
		BitBlt(hdc, 0, 0, areaSize.width, areaSize.height, backbuffer, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		return 0L;
	}
	case WM_ERASEBKGND:
	{
		return 0L;
	}
	/* if user clicked then look was it click on an object and if he did set the object as chosen */
	case WM_LBUTTONDOWN:
	{
		COORD p;// mouse position on the window
		p.Y = HIWORD(lParam);
		p.X = LOWORD(lParam);
		MainWindow* mw = (MainWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		INT nearest = ~(1 << 31); /* squared distance between points */
		SHORT chosen = -1; /* position chosen object in the array of objects*/
		COORD pos;
		COORD grabPoint;
		ObjectSpace::SIZE sz;
		for (SHORT i = 0; i < mw->m_objsCnt; ++i)
		{
			pos = mw->m_ppobjs[i]->GetPos();
			sz = mw->m_ppobjs[i]->GetSize();
			if (((USHORT)(p.X - pos.X) <= sz.width) & ((USHORT)(p.Y - pos.Y) <= sz.height))
			{
				grabPoint = mw->m_ppobjs[i]->GetGrabPoint();
				/* calculate distance between grab point and click pos */
				if (nearest > (p.X - grabPoint.X)*(p.X - grabPoint.X) + (p.Y - grabPoint.Y)*(p.X - grabPoint.X))
				{
					chosen = i;
					nearest = (p.X - grabPoint.X)*(p.X - grabPoint.X) + (p.Y - grabPoint.Y)*(p.X - grabPoint.X);
				}
			}
		}
		if (-1 == chosen)
		{
			mw->m_chosen = nullptr;
			return 0L;
		}
		mw->m_chosen = mw->m_ppobjs[chosen];
		return 0L;
	}
	case WM_MOUSEMOVE:
	{
		if ((MK_LBUTTON == wParam))
		{
			MainWindow* mw = (MainWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if (nullptr != mw->m_chosen)
			{
				COORD p;/* mouse position on the window */
				p.Y = HIWORD(lParam);
				p.X = LOWORD(lParam);
				mw->m_chosen->MoveTo(p);
				InvalidateRect(hWnd, nullptr, FALSE);
			}
		}
		return 0L;
	}
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_ESCAPE:
		{
			PostQuitMessage(0);
			break;
		}
		}
		return 0L;
	}
	case WM_DESTROY:
	case WM_QUIT:
	{
		DeleteObject(backbufferBitmap);
		DeleteDC(backbuffer);
		PostQuitMessage(0);
		return 0L;
	}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}