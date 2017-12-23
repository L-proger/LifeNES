#pragma once

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "IRenderWindow.h"

class RenderWindow : public IRenderWindow
{
public:
	/*virtual ~RenderWindow(void) {}
	virtual void Update() = 0;
	virtual void SetCaption(const String& caption) = 0;
	virtual int GetWindowHandle() = 0;
	virtual int GetWidth() = 0;
	virtual int GetHeight() = 0;
	virtual const String& GetCaption() = 0;*/



	RenderWindow();
	~RenderWindow(void);
	void Update();
	void SetCaption(const String& caption);
	int GetWindowHandle(){return (int)g_hwnd;}
	void SetSize(int w, int h);
	bool IsFullScreen();
	void SetFullScreen(bool state);
	bool SwitchMode(bool fullscreen);
	void SetFullscreenMode(DisplayMode mode);


	void OnActivate();
	void OnDeactivate();
	void OnResize();
	bool IsActive();
	bool IsModeSwitching();
	void SetCursorVisible(bool state);
	bool IsCursorVisible();
	void CenterWindow();

	int GetWidth()
	{
		return width;
	}
	virtual int GetHeight()
	{
		return height;
	}
	const String& GetCaption()
	{
		return caption;
	}
	void SetAltEnterLocked(bool state)
	{
		bAltEnterLocked = state;
	}
	bool IsAltEnterLocked()
	{
		return bAltEnterLocked;
	}
protected:
	bool bAltEnterLocked;
	HINSTANCE g_hInst;
	HWND g_hwnd;
	DEVMODE mode;
};
