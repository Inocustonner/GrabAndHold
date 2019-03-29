#include "MainWindow.hpp"
#include <iostream>
BOOL RegisterWindow(LPCSTR wndName, HINSTANCE hInstance, LRESULT(CALLBACK *WndProc)(HWND, UINT, WPARAM, LPARAM))
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
		return FALSE;
	}
	return TRUE;
}

MainWindow::~MainWindow()
{
	/* free all objects */
	for (SHORT i = 0; i < m_objsCnt; ++i)
	{
		if (m_ppobjs[i])
			delete m_ppobjs[i];
	}
	if (m_ppobjs)
		delete[] m_ppobjs;
	/* release backbuffer */
	if (m_backBuffer)
		DeleteObject(m_backBuffer);

	if (m_backBufferDC)
		DeleteDC(m_backBufferDC);
	/* delete bg */
	if (m_backGround)
		DeleteObject(m_backGround);
}

BOOL MainWindow::InitBackBuffer()
{
	HDC hdc = GetDC(m_hWnd);

	m_backBufferDC = CreateCompatibleDC(hdc);

	RECT rc;
	GetClientRect(m_hWnd, &rc);/*                 width                height        */
	m_backBuffer = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
	SelectObject(m_backBufferDC, m_backBuffer);

	ReleaseDC(m_hWnd, hdc);
	return TRUE;
}

inline BOOL LoadBackGround(HBITMAP* bg, LPCSTR path)
{
	*bg = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (NULL == *bg)
	{
		return FALSE;
	}
	return TRUE;
}
inline BOOL LoadBackGround(HBITMAP* bg, HBITMAP src)
{
	*bg = src;
	if (NULL == *bg)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL MainWindow::Initialize(HINSTANCE hInstance, INT width, INT height, BOOL fullscreen)
{
	if (!InitializeWindow(hInstance, width, height, fullscreen))
		return FALSE;

	const int count = 2;
	LPCSTR paths[count] = { "./test.bmp", "./test2.bmp" };
	LPCSTR maskPaths[count] = { "./testMask.bmp", "./testMask.bmp" };
	COORD poses[count] = { {50, 50}, {200, 100} };
	COORD grabs[count] = { {30, 0}, {30, 0} };
	if (!CreateObjects(count, paths, maskPaths, poses, grabs))
		return FALSE;

	InitBackBuffer();
	if (NULL == LoadBackGround(&m_backGround, "./bgtest.bmp"))
		return FALSE;
	return TRUE;
}

BOOL MainWindow::CreateObjects(SHORT count, LPCSTR* path, LPCSTR* maskPaths, COORD* positions, COORD* grabPoints)
{
	m_objsCnt = count;
	m_ppobjs = new NObject*[m_objsCnt];
	if (!m_ppobjs)
	{
		MessageBox(NULL, "Failed to allocate memory", "Error", MB_OK);
		return FALSE;
	}
	for (SHORT i = 0; i < m_objsCnt; ++i)
	{
		m_ppobjs[i] = new NObject;
		if (!m_ppobjs[i]->Initialize(path[i], maskPaths[i], positions[i], grabPoints[i]))
		{
			return FALSE;
		}
	}
	return TRUE;
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

void DrawBackGround(HWND hWnd, HDC* hdc, HBITMAP backGround, RECT* lprect, COORD srcOff)
{
	HDC srcDC = CreateCompatibleDC(*hdc);
	HGDIOBJ oldBmp = nullptr;
	if (nullptr == lprect)
	{
		lprect = new RECT;
		GetClientRect(hWnd, lprect);
	}
	oldBmp = SelectObject(srcDC, backGround);
	BitBlt(*hdc, lprect->left, lprect->top, lprect->right - lprect->left, lprect->bottom - lprect->top, srcDC, srcOff.X, srcOff.Y, SRCCOPY);
	SelectObject(srcDC, oldBmp);
	DeleteObject(oldBmp);
	DeleteDC(srcDC);
}

/* i've made this function because i don't know how to hook RECT from InvalidateRect function */
void MainWindow::Draw(HWND hWnd, LPRECT pRect, BOOL bRepaintBG)
{
	/* if pRect is nullptr then redraw the entire client area */
	BOOL isRectNew = FALSE;
	if (bRepaintBG)
	{
		DrawBackGround(hWnd, &m_backBufferDC, m_backGround, pRect, { 0, 0 });
	}
	if (nullptr == pRect)
	{
		isRectNew = TRUE;
		ObjectSpace::Render(&m_backBufferDC, m_ppobjs, m_objsCnt);/* render the entire area*/
		pRect = new RECT;
		GetClientRect(hWnd, pRect);
	}
	else
	{
		ObjectSpace::RenderRect(&m_backBufferDC, m_ppobjs, m_objsCnt, pRect);/* render only rect area */
	}
	HDC hdc = GetDC(m_hWnd);
	BitBlt(hdc, pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top, m_backBufferDC, pRect->left, pRect->top, SRCCOPY);
	ReleaseDC(m_hWnd, hdc);
	if (TRUE == isRectNew) delete pRect;
	return;
}

void MainWindow::Run()
{
	MSG msg = { 0 };
	BOOL ret;
	/*while (ret = GetMessage(&msg, NULL, NULL, NULL))
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
	}*/
	/* time at the start of a frame */
	DOUBLE fps = 1.0 / 30.0;
	std::chrono::time_point<std::chrono::system_clock> start_time;
	/* to translate in double */
	std::chrono::duration<DOUBLE> delta;
	Draw(m_hWnd, nullptr, TRUE);
	while (TRUE)
	{
		start_time = std::chrono::system_clock::now();
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&msg);
			TranslateMessage(&msg);
			if (msg.message == WM_DESTROY || msg.message == WM_QUIT)
				break;
		}
		for (SHORT i = 0; i < m_objsCnt; ++i)
		{
			ObjectSpace::ComputatePos(m_ppobjs[i]);
		}
		Draw(m_hWnd, nullptr, TRUE);
		delta = std::chrono::system_clock::now() - start_time;
		if (delta.count() < fps)
		{
			Sleep((fps - delta.count()) * 500);
		}
	}
}

LRESULT CALLBACK MainWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_PAINT:/* have the Draw function instead */
	{
		return 0L;
	}
	case WM_ERASEBKGND:/* have the Draw function instead */
	{
		return 1L;
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
				mw->m_chosen->UpdateTime();
				//mw->Draw(hWnd, nullptr, TRUE);
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
		PostQuitMessage(0);
		return 0L;
	}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}