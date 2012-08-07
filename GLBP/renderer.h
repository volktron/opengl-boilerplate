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
	public:
	~Renderer();
	static Renderer* getInstance();
	void initialize(HDC* hDC, HGLRC* hglrc);

	void render();
};

#endif