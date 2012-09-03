#ifndef RENDERER_H
#define RENDERER_H

#include <Windows.h>

#define RENDERER Renderer::getInstance()

class Renderer
{
private:
	Renderer();
	static Renderer* renderer;
	static bool instanceflag;

	void* render_thread;
	HDC* hDC;
	HGLRC* hRC;

public:
	bool pending_resize;
	int pending_width;
	int pending_height;

	int g_width;
	int g_height;

public:
	~Renderer();
	static Renderer* getInstance();
	void initialize(HDC* hDC, HGLRC* hglrc);
	void handle_resize();

	void render();
};

#endif