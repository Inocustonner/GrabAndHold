#include <Windows.h>
#include "SettingsWindow.hpp"
#include "MainWindow.hpp"
INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR cmdline, INT cmdshow)
{
	SettWnd *stWnd = (SettWnd*)malloc(sizeof SettWnd);// window for that check client settings
	if (!stWnd->Initialize(hInstance, 400, 300))
	{
		free(stWnd);
		return 0L;
	}
	if (WM_QUIT == stWnd->Run())
	{
		return 0;
	}
	SettWnd::Settings* settings = (SettWnd::Settings*)malloc(sizeof SettWnd::Settings);
	*settings = stWnd->GetSettings();
	stWnd->FreezeWindow();/* hide and disable window */

	MainWindow mainWindow;
	if (!mainWindow.InitializeWindow(hInstance, settings->width, settings->height, settings->fullscreen))
	{
		free(settings);
		return 0;
	}
	free(settings); /* release settings */
	mainWindow.Run();/* run window message loop */

	stWnd->~SettWnd();
	free(stWnd);/* destroy window and release memory*/
	return 0;
}

int main()
{
	return WinMain(GetModuleHandle(NULL), NULL, NULL, SW_SHOW);
}