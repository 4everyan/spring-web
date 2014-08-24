
#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <SDL2/SDL.h>
#include <string>


class EventHandler
{
public:
	virtual ~EventHandler() {}

	virtual void OnEvent(SDL_Event* event);
	//
	// keyboard events
	virtual void OnKeyDown(SDL_Keycode keycode, SDL_Scancode scancode, Uint16 mod) {}
	virtual void OnKeyUp(SDL_Keycode keycode, SDL_Scancode scancode, Uint16 mod) {}
	//
	// mouse events
	virtual void OnLButtonDown(int mx, int my) {}
	virtual void OnLButtonUp(int mx, int my) {}
	virtual void OnRButtonDown(int mx, int my) {}
	virtual void OnRButtonUp(int mx, int my) {}
	virtual void OnMButtonDown(int mx, int my) {}
	virtual void OnMButtonUp(int mx, int my) {}
	virtual void OnMouseMove(int mx, int my, int relx, int rely, bool left, bool right, bool middle) {}
	virtual void OnMouseWheel(int dx, int dy) {}
	//
	// window events
	virtual void OnFocusGained() {}
	virtual void OnFocusLost() {}
	virtual void OnMouseEnter() {}
	virtual void OnMouseLeave() {}
	virtual void OnMinimize() {}
	virtual void OnRestore() {}
	virtual void OnResize(int width, int height) {}
	virtual void OnExpose() {}
	virtual void OnExit() {}
};


class Application: public EventHandler
{
public:
	Application();

	// starts the system
	int SystemExecute();

	// stops the system breaking out main loop
	void SystemExit();

	// graphics methods
	void GfxBeginScene();
	void GfxEndScene();

	// simulation methods
	Uint32 TimerGetDelta() const { return m_dt; }
	Uint32 TimerGetFps() const { return m_lastCalculatedFps; }

	// window
	int WindowWidth() const;
	int WindowHeight() const;

    std::string VendorName() const { return vendorName; }
    std::string RendererName() const { return rendererName; }

	// application specific events
	virtual bool OnCreate() { return true; }
	virtual void OnDestroy() {}
	virtual void OnFrame() {}
    virtual void OnRender() {}
    virtual void OnActivate(bool active) {}
    virtual void OnMouseDrag(int dx, int dy, Uint8 button) {}

private:
	bool SystemCreate();
	void SystemCleanup();
	void SystemFrame();
	void SystemActivate(bool active = true);

	// EventHandler overrides
    void OnEvent(SDL_Event* event) override;

private:
	bool m_quit;
	bool m_active;
	SDL_Window* m_window;
	SDL_GLContext m_context;

	// time manage
	Uint32 m_lastCalculatedFps, m_fpsCounter;
	Uint32 m_frameStart, m_fpsCounterStart;
	Uint32 m_dt, m_fixedDelta;

	// draw debug info variables
	std::string vendorName;
	std::string rendererName;
};

#endif //__APPLICATION_H__
