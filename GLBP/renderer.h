#ifndef RENDERER_H
#define RENDERER_H

#include <Windows.h>

#define RENDERER Renderer::get_instance()

class Renderer
{
private:
	Renderer();
	static Renderer* renderer;
	static bool instanceflag;

	void*		render_thread;
	HDC*		hDC;
	HGLRC*		hRC;
	HWND*		hWnd;
	HINSTANCE*	hInstance;

public:
	bool pending_resize;
	int pending_width;
	int pending_height;

	int g_width;
	int g_height;

	bool pending_fullscreen;
	bool fullscreen;

	bool pending_kill_gl;

public:
	~Renderer();
	static Renderer* get_instance();
	
	// Event functions
	void initialize(HDC*		hDC, 
					HGLRC*		hglrc, 
					HWND*		hWnd,
					HINSTANCE*	hInstance,
					bool		fullscreen);
	void handle_resize();
	void handle_fullscreen_toggle();

	void kill_gl_window();

	// Rendering functions
	void render();
};

#endif