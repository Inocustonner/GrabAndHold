#include "SettingsWindow.hpp"
#include <iostream>
bool SettWnd::RegisterWindow(LPCSTR classname, HINSTANCE hInstance)
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof WNDCLASSEX);
	wc.cbSize = sizeof WNDCLASSEX;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DDKSHADOW);//(HBRUSH)GetStockObject(GRAY_BRUSH);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = wc.hIcon;
	wc.hInstance = hInstance;
	wc.lpfnWndProc = this->DlgProc;
	wc.lpszClassName = classname;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	if (!RegisterClassEx(&wc))
		return false;
	return true;
}
bool SettWnd::Initialize(HINSTANCE hInstance, INT width, INT height)
{
	/* title for the window */
	LPCSTR name = "Settings";
	/* register window class if the operation is failed then return false */
	if (!RegisterWindow(name, hInstance))
	{
		MessageBox(NULL, "Failed to create windows class", "Error", MB_OK);
		return false;
	}
	/* get clients monitor sizes */
	INT screenWidth = GetSystemMetrics(SM_CXSCREEN);
	INT screenHeight = GetSystemMetrics(SM_CYSCREEN);

	m_hWnd = CreateWindowEx(WS_EX_DLGMODALFRAME, name, name,
		WS_VISIBLE | WS_SYSMENU,
		screenWidth / 2 - width / 2, screenHeight / 2 - height / 2,
		width, height,
		(HWND)NULL,
		(HMENU)NULL,
		hInstance,
		this);// put a pointer on our class to save the settings input
	/* if window creation failed */
	if (!m_hWnd)
	{
		MessageBox(NULL, "Failed to create Window", "Error", MB_OK);
		return false;
	}

	UpdateWindow(m_hWnd);
	ShowWindow(m_hWnd, SW_SHOW);
	return true;
}

UINT SettWnd::Run()
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
	return msg.message;
}

void SettWnd::FreezeWindow()
{
	ShowWindow(m_hWnd, SW_HIDE);
	EnableWindow(m_hWnd, FALSE);
	SendMessage(m_hWnd, WM_SETREDRAW, FALSE, 0);
}

SettWnd::Settings SettWnd::GetSettings()
{
	return m_settings;
}


LRESULT CALLBACK SettWnd::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP image;
	switch (msg)
	{
	case WM_CREATE:
	{
		/* Create Clients Resolution button */
		CreateWindowEx(NULL, "button", "", BS_CHECKBOX | WS_VISIBLE | WS_CHILD,
			10, 175,
			13, 13,
			hWnd,
			(HMENU)CHECKBOX_CLIENTRESOLUTION,
			NULL,
			NULL);

		/* Client Resolutions input boxes */
		CreateWindowEx(NULL, "edit", "", ES_LEFT | ES_NUMBER | WS_VISIBLE | WS_CHILD,
			230, 175,
			40, 13,
			hWnd,
			(HMENU)TEXTFIELD_WIDTH,
			NULL,
			NULL);

		CreateWindowEx(NULL, "edit", "", ES_LEFT | ES_NUMBER | WS_VISIBLE | WS_CHILD,
			310, 175,
			40, 13,
			hWnd,
			(HMENU)TEXTFIELD_HEIGHT,
			NULL,
			NULL);
		
		/* Set char limit equals 4 for each text field */
		SendMessage(GetDlgItem(hWnd, TEXTFIELD_WIDTH), EM_SETLIMITTEXT, (WPARAM)4, NULL);
		SendMessage(GetDlgItem(hWnd, TEXTFIELD_HEIGHT), EM_SETLIMITTEXT, (WPARAM)4, NULL);

		/* Create Check Box fullscreen */
		CreateWindowEx(NULL, "button", "", BS_CHECKBOX | WS_VISIBLE | WS_CHILD,
			10, 200,
			13, 13,
			hWnd,
			(HMENU)CHECKBOX_FULLSCREEN,
			NULL,
			NULL);
		/* Pre-check fullscreen button */
		CheckDlgButton(hWnd, CHECKBOX_FULLSCREEN, BST_CHECKED);

		/* Create Check Box blue */
		CreateWindowEx(NULL, "button", "", BS_CHECKBOX | WS_VISIBLE | WS_CHILD,
			150, 200,
			13, 13,
			hWnd,
			(HMENU)CHECKBOX_BLUE,
			NULL,
			NULL);

		/* Create start button */
		CreateWindowEx(NULL, "button", "Start game", WS_VISIBLE | WS_CHILD,
			250, 200,
			100, 50,
			hWnd,
			(HMENU)BUTTON_STARTGAME,
			NULL,
			NULL);

		/* Load image */
		image = (HBITMAP)LoadImage(GetModuleHandle(NULL), ".\\menu.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (!image)
			MessageBox(NULL, "Failed to load image", "Error", MB_OK);

		/* Set long param */
		CREATESTRUCT* lparamStruct = (CREATESTRUCT*)lParam;
		SettWnd* pState = (SettWnd*)lparamStruct->lpCreateParams;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pState);

		return 0L;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case CHECKBOX_CLIENTRESOLUTION:
		{
			/* if choosed client resolution uncheck fullscreen */
			CheckDlgButton(hWnd, CHECKBOX_CLIENTRESOLUTION, BST_CHECKED);

			CheckDlgButton(hWnd, CHECKBOX_FULLSCREEN, BST_UNCHECKED);
			break;
		}
		case CHECKBOX_FULLSCREEN:
		{
			/* if choosed fullscreen uncheck client resolution */
			CheckDlgButton(hWnd, CHECKBOX_FULLSCREEN, BST_CHECKED);

			CheckDlgButton(hWnd, CHECKBOX_CLIENTRESOLUTION, BST_UNCHECKED);
			break;
		}
		case CHECKBOX_BLUE:
		{
			UINT checked = IsDlgButtonChecked(hWnd, CHECKBOX_BLUE);
			if (checked)
			{
				image = (HBITMAP)LoadImage(GetModuleHandle(NULL), ".\\menu.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
				CheckDlgButton(hWnd, CHECKBOX_BLUE, BST_UNCHECKED);
				InvalidateRect(hWnd, NULL, TRUE);
			}
			else
			{
				image = (HBITMAP)LoadImage(GetModuleHandle(NULL), ".\\gaymenu.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
				CheckDlgButton(hWnd, CHECKBOX_BLUE, BST_CHECKED);
				InvalidateRect(hWnd, NULL, TRUE);
			}
			break;
		}
		case BUTTON_STARTGAME:
		{
			/* Get the class pointer from lParam */
			LONG_PTR lPtr = GetWindowLongPtr(hWnd, GWLP_USERDATA);
			SettWnd* wnd = (SettWnd*)lPtr;
			INT monitorW = GetSystemMetrics(SM_CXSCREEN);//client monitor's width
			INT monitorH = GetSystemMetrics(SM_CYSCREEN);//client monitor's height

			/* if fullscreen choosed then width and height equals monitors sizes */
			if (IsDlgButtonChecked(hWnd, CHECKBOX_FULLSCREEN))
			{
				wnd->m_settings.fullscreen = 1;
				wnd->m_settings.gay = IsDlgButtonChecked(hWnd, CHECKBOX_BLUE);
				wnd->m_settings.width = monitorW;
				wnd->m_settings.height = monitorH;
			}
			else
			{
				wnd->m_settings.fullscreen = 0;
				wnd->m_settings.gay = IsDlgButtonChecked(hWnd, CHECKBOX_BLUE);
				CHAR buffer[5]; /* buffer for reading text fields info */

				/* Get width */
				GetWindowText(GetDlgItem(hWnd, TEXTFIELD_WIDTH), buffer, -1);
				wnd->m_settings.width = atoi(buffer);
				if (0 == wnd->m_settings.width)
				{
					MessageBox(NULL, "Enter width", "Empty field", MB_OK);
					break;
				}/* if user entered a width that greater than monitor's set the width to monitor's */
				if (wnd->m_settings.width > monitorW)
					wnd->m_settings.width = monitorW;

				/* Get height */
				GetWindowText(GetDlgItem(hWnd, TEXTFIELD_HEIGHT), buffer, -1);
				wnd->m_settings.height = atoi(buffer);
				if (0 == wnd->m_settings.height)
				{
					MessageBox(NULL, "Enter width", "Empty field", MB_OK);
					break;
				}/* if user entered a height that greater than monitor's set the height to monitor's */
				if (wnd->m_settings.height > monitorH)
					wnd->m_settings.height = monitorH;
			}
			/* close the window */
			PostQuitMessage(0);
		}
		}
		return 0L;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BITMAP bitmap;
		HGDIOBJ oldBitmap;
		HDC hdcmem;
		HDC hdc = BeginPaint(hWnd, &ps);
		/* Draw Image */

		hdcmem = CreateCompatibleDC(hdc);

		oldBitmap = SelectObject(hdcmem, image);

		GetObject(image, sizeof bitmap, &bitmap);

		BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcmem, 0, 0, SRCCOPY);
		//StretchBlt(hdc, 0, 0, 400, 150, hdcmem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
		SelectObject(hdcmem, oldBitmap);
		DeleteDC(hdcmem);

		/* Draw text*/
		const short TEXTSZ = 20;
		/* set text font*/
		HFONT texFont = CreateFont(TEXTSZ, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
			OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, "Courier New");
		SetBkMode(hdc, TRANSPARENT);
		HGDIOBJ oldFont = SelectObject(hdc, texFont);

		/* clients check box text pos */
		RECT clientResolution = { 25, 175 - 3, 0, 0 };
		/* width text field pos */
		RECT clientWidth = { 210, 175 - 3, 0, 0 };
		/* height text field pos */
		RECT clientHeight = { 290, 175 - 3, 0, 0 };
		/* fullscreen check box text pos */
		RECT fullscreen = { 25, 200 - 3, 0, 0 };
		/* gay check boxt text pos */
		RECT gay = { 165, 200 - 3, 0, 0 };

		DrawText(hdc, "client resolution", -1, &clientResolution, DT_NOCLIP | DT_TOP | DT_LEFT);
		DrawText(hdc, "w:", -1, &clientWidth, DT_NOCLIP | DT_TOP | DT_LEFT);
		DrawText(hdc, "h:", -1, &clientHeight, DT_NOCLIP | DT_TOP | DT_LEFT);
		DrawText(hdc, "fullscreen", -1, &fullscreen, DT_NOCLIP | DT_TOP | DT_LEFT);
		DrawText(hdc, "blue", -1, &gay, DT_NOCLIP | DT_TOP | DT_LEFT);
		SelectObject(hdc, oldFont);
		EndPaint(hWnd, &ps);
		return 0L;
	}
	case WM_DESTROY:
	case WM_QUIT:
	{
		DeleteObject(image);
		PostQuitMessage(0);
		return 0L;
	}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

SettWnd::~SettWnd()
{
	DestroyWindow(m_hWnd);
}