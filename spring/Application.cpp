
#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>

#include "Application.h"
#include "Extensions.h"
#include "GraphicObjManager.h"



void LogOutput(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
	std::string prefix;

	static bool alreadyWrited = false;
	std::ios::openmode mode;
	if( alreadyWrited )
		mode = std::ios::app;
	else {
		mode = std::ios::out;
		alreadyWrited = true;
	}
	std::ofstream logfile("log.txt", mode);

	switch( category )
	{
	case SDL_LOG_CATEGORY_APPLICATION:
		prefix = "INFO";
		break;
		
    case SDL_LOG_CATEGORY_ERROR:
		prefix = "ERROR";
    	break;
    	
    case SDL_LOG_CATEGORY_ASSERT:
		prefix = "ASSERT";
    	break;
    	
    case SDL_LOG_CATEGORY_SYSTEM:
		prefix = "SYSTEM";
    	break;
    	
    case SDL_LOG_CATEGORY_AUDIO:
		prefix = "AUDIO";
    	break;
    	
    case SDL_LOG_CATEGORY_VIDEO:
		prefix = "VIDEO";
    	break;
    	
    case SDL_LOG_CATEGORY_RENDER:
		prefix = "RENDER";
    	break;
    	
    case SDL_LOG_CATEGORY_INPUT:
		prefix = "INPUT";
    	break;
    	
    case SDL_LOG_CATEGORY_TEST:
		prefix = "TEST";
    	break;
	}
	std::cout /*<< prefix << ": "*/ << message << std::endl;
	logfile   /*<< prefix << ": "*/ << message << std::endl;
}


void EventHandler::OnEvent(SDL_Event* event)
{
	if( event->type == SDL_QUIT )
		return OnExit();

	if( event->type == SDL_WINDOWEVENT )
	{
		switch( event->window.event )
		{
        case SDL_WINDOWEVENT_EXPOSED:
			return OnExpose();

        case SDL_WINDOWEVENT_RESIZED:
			return OnResize(event->window.data1, event->window.data2);

        case SDL_WINDOWEVENT_MINIMIZED:
			return OnMinimize();

        case SDL_WINDOWEVENT_RESTORED:
			return OnRestore();

        case SDL_WINDOWEVENT_ENTER:
			return OnMouseEnter();

        case SDL_WINDOWEVENT_LEAVE:
			return OnMouseLeave();

        case SDL_WINDOWEVENT_FOCUS_GAINED:
			return OnFocusGained();

        case SDL_WINDOWEVENT_FOCUS_LOST:
			return OnFocusLost();
        }
	}
	if( event->type == SDL_KEYDOWN )
		return OnKeyDown(event->key.keysym.sym, event->key.keysym.scancode, event->key.keysym.mod);

	if( event->type == SDL_KEYUP )
		return OnKeyUp(event->key.keysym.sym, event->key.keysym.scancode, event->key.keysym.mod);

	if( event->type == SDL_MOUSEBUTTONDOWN )
	{
		switch( event->button.button )
		{
		case SDL_BUTTON_LEFT:
			return OnLButtonDown(event->button.x, event->button.y);

		case SDL_BUTTON_RIGHT:
			return OnRButtonDown(event->button.x, event->button.y);

		case SDL_BUTTON_MIDDLE:
			return OnMButtonDown(event->button.x, event->button.y);
		}
	}
	if( event->type == SDL_MOUSEBUTTONUP )
	{
		switch( event->button.button )
		{
		case SDL_BUTTON_LEFT:
			return OnLButtonUp(event->button.x, event->button.y);

		case SDL_BUTTON_RIGHT:
			return OnRButtonUp(event->button.x, event->button.y);

		case SDL_BUTTON_MIDDLE:
			return OnMButtonUp(event->button.x, event->button.y);
		}
	}
	if( event->type == SDL_MOUSEMOTION ) {
		SDL_MouseMotionEvent e = event->motion;
		return OnMouseMove(e.x, e.y, e.xrel, e.yrel,
			(e.state & SDL_BUTTON_LMASK) != 0,
			(e.state & SDL_BUTTON_RMASK) != 0,
			(e.state & SDL_BUTTON_MMASK) != 0);
	}
	if( event->type == SDL_MOUSEWHEEL )
		return OnMouseWheel(event->wheel.x, event->wheel.y);
}

Application::Application()
{
	m_quit = false;
	m_active = false;
}

int Application::SystemExecute()
{
	if( !SystemCreate() )
		return 1;

	SystemActivate(true);
	SDL_Event event;
	
	while( !m_quit )
	{
		while( SDL_PollEvent(&event) ) {
			OnEvent(&event);
		}
		if( m_active )
		{
			SystemFrame();
		}
		else SDL_Delay(1);
	}
	SystemCleanup();
	return 0;
}

void Application::SystemExit()
{
	SDL_Event event;
	event.type = SDL_QUIT;
	SDL_PushEvent( &event );
}

void Application::GfxBeginScene()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void Application::GfxEndScene()
{
	glFlush();
	SDL_GL_SwapWindow(m_window);
}

int Application::WindowWidth() const
{
	int w, h;
	SDL_GetWindowSize(m_window, &w, &h);
	return w;
}

int Application::WindowHeight() const
{
	int w, h;
	SDL_GetWindowSize(m_window, &w, &h);
	return h;
}

inline void CheckSDLError()
{
	SDL_Log("SDL: %s\n", SDL_GetError());
	SDL_ClearError();
}

bool Application::SystemCreate()
{
	atexit(SDL_Quit);
#ifdef NDEBUG
	SDL_LogSetOutputFunction( LogOutput, nullptr );
#endif	

	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		CheckSDLError();
		return false;
	}
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	m_window = SDL_CreateWindow("SDL Window",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		1152, 648,
#ifdef NDEBUG
		SDL_WINDOW_FULLSCREEN_DESKTOP |
#endif
		SDL_WINDOW_OPENGL);

	m_context = SDL_GL_CreateContext(m_window);
	SDL_GL_MakeCurrent(m_window, m_context);

    //
    // Initializing glew
    //
	GLenum glewError = glewInit();

	if( glewError != GLEW_OK ) {
		SDL_Log("Failed to init GLEW: %s", glewGetErrorString(glewError));
		return false;
	}
    
    //
    // Saving vendor/renderer info
    //
    vendorName = (const char*)glGetString(GL_VENDOR);
	rendererName = (const char*)glGetString(GL_RENDERER);

	SDL_Log("%s", vendorName.c_str());
	SDL_Log("%s\n", rendererName.c_str());

    //
    // Checking for Framebuffer support
    //
    /*if( glewIsSupported("GL_ARB_framebuffer_object") ) {
		FramebufferExt::InitializeExtension( FramebufferExt::ChooseARB );
		SDL_Log("GL_ARB_framebuffer_object: supported");
	}
	else if( glewIsSupported("GL_EXT_framebuffer_object") )	{
		FramebufferExt::InitializeExtension( FramebufferExt::ChooseEXT );
		SDL_Log("GL_EXT_framebuffer_object: supported");
	}
	else {
		SDL_Log("Application requies one of:\n  GL_ARB_framebuffer_object\n  GL_EXT_framebuffer_object");
		return false;
	}*/

    //
    // Checking for OpenGL 2.0 support
    //
    if( !glewIsSupported("GL_VERSION_2_0") )
	{
		SDL_Log("Application requies OpenGL version 2.0");
		return false;
	}
	SDL_Log("GL_VERSION_2_0: supported");
	
	return OnCreate();
}

void Application::SystemCleanup()
{
	OnDestroy();
    GraphicObjManager::Instance()->ReleaseGraphics();

	SDL_GL_DeleteContext(m_context);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}


void Application::SystemFrame()
{
	// Ensure we have at least 1ms time step
	// to not confuse user's code with 0
	do { m_dt = SDL_GetTicks() - m_frameStart; }
	while( m_dt < 1 );

	if( m_dt >= m_fixedDelta ) {
		m_frameStart = SDL_GetTicks();

		if( m_frameStart - m_fpsCounterStart <= 1000 )
			++m_fpsCounter;
		else {
			m_lastCalculatedFps = m_fpsCounter;
			m_fpsCounter = 0;
			m_fpsCounterStart = m_frameStart;
		}
		OnFrame();
		OnRender();
	}
	else
	{
		// If we have a fixed frame rate and the time
		// for the next frame isn't too close, sleep a bit
		if( m_fixedDelta > 0 && m_dt+3 < m_fixedDelta )
			SDL_Delay(1);
	}
}

void Application::SystemActivate(bool active)
{
	// activation
	if( !m_active && active )
	{
		m_lastCalculatedFps = m_fpsCounter = 0;
		m_fpsCounterStart = m_frameStart = SDL_GetTicks();
		m_fixedDelta = 10;
        OnActivate( true );
	}
    else if( m_active && !active )
    {
        OnActivate( false );
    }
	m_active = active;
}

void Application::OnEvent( SDL_Event* event )
{
    static bool dragEnabled = false;
    static Uint8 button = 0;
    static int lastMouseX = 0;
    static int lastMouseY = 0;

	// the application is about to quit
	if( event->type == SDL_QUIT )
	{
		m_quit = true;
		m_active = false;
	}
	// standart window events
	if( event->type == SDL_WINDOWEVENT )
	{
        switch (event->window.event)
		{
        case SDL_WINDOWEVENT_MINIMIZED:
			SystemActivate(false);
            break;

		case SDL_WINDOWEVENT_RESTORED:
			SystemActivate(true);
            break;

		case SDL_WINDOWEVENT_MOVED:
			SystemActivate(false);
			SystemActivate(true);
            break;
        }
    }
    if( event->type == SDL_MOUSEBUTTONDOWN )
    {
        if( !dragEnabled )
        {
            SDL_SetWindowGrab(m_window, SDL_TRUE);
            button = event->button.button;
            lastMouseX = event->button.x;
            lastMouseY = event->button.y;
            dragEnabled = true;
        }
    }
    else if( event->type == SDL_MOUSEBUTTONUP )
    {
        if( dragEnabled )
        {
            dragEnabled = false;
            SDL_SetWindowGrab(m_window, SDL_FALSE);
        }
    }
    else if( event->type == SDL_MOUSEMOTION )
    {
        if( dragEnabled && (event->motion.x != lastMouseX || event->motion.y != lastMouseY) )
        {
            OnMouseDrag(event->motion.x - lastMouseX, event->motion.y - lastMouseY, button);
            lastMouseX = event->motion.x;
            lastMouseY = event->motion.y;
        }
    }
	return EventHandler::OnEvent( event );
}
