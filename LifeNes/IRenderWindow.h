#ifndef IRenderWindow_h__
#define IRenderWindow_h__

#include "Delegate.h"
#include "LString.h"
#include "DisplayMode.h"


class IRenderWindow
{
public:
	//events
	EVENT(ExitEvent);
	ExitEvent Event_Quit;

	EVENT1(WindowEvent, IRenderWindow*);
	WindowEvent Event_Resize;
	WindowEvent Event_SwitchFullscreen;
	WindowEvent Event_Deactivate;
	WindowEvent Event_Activate;


	virtual ~IRenderWindow(void) {}
	virtual void Update() = 0;
	virtual void SetCaption(const String& caption) = 0;
	virtual int GetWindowHandle() = 0;
	virtual int GetWidth() = 0;
	virtual int GetHeight() = 0;
	virtual void SetSize(int w, int h) = 0;
	virtual const String& GetCaption() = 0;

	virtual bool IsFullScreen() = 0;
	virtual void SetFullScreen(bool state) = 0;
	virtual bool SwitchMode(bool fullscreen) = 0;
	virtual void SetFullscreenMode(DisplayMode mode) = 0;

	virtual const DisplayMode& GetFullscreenMode()
	{
		return fsMode;
	}

	virtual void OnActivate() = 0;
	virtual void OnDeactivate() = 0;
	virtual void OnResize() = 0;
	virtual bool IsActive() = 0;
	virtual bool IsModeSwitching() = 0;
	virtual void SetCursorVisible(bool state) = 0;
	virtual bool IsCursorVisible() = 0;
	virtual void CenterWindow() = 0;
protected:
	bool bCursorVisible;
	bool bModeSwitching;
	bool bFullscreen;
	bool bActive;
	int width;
	int height;
	String caption;
	DisplayMode fsMode;
};
#endif // IRenderWindow_h__
