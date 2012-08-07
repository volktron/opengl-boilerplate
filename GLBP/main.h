#ifndef MAIN_H
#define MAIN_H

// Includes
#include <iostream>

// Game logic
#include "engine.h"
#include "renderer.h"

// Graphics
#include "glew.h"
#include <gl\gl.h>		// Header File For The OpenGL32 Library
#include <gl\glu.h>		
#include "wglext.h"

#include <string>
#include <sstream>

// Config Variables
int width = 1280;
int height = 720;
char* title_string = "GL BoilerPlate";

// Windows Stuff
HDC			hDC=NULL;		// Private GDI Device Context
HGLRC		hRC=NULL;		// Permanent Rendering Context
HWND		hWnd=NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

bool	active=TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen=FALSE;	// Fullscreen Flag Set To Fullscreen Mode By Default

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

// Function pointer typdefs
typedef void (APIENTRY *PFNWGLEXTSWAPCONTROLPROC) (int);
typedef int (*PFNWGLEXTGETSWAPINTERVALPROC) (void);

// Declare functions
PFNWGLEXTSWAPCONTROLPROC wglSwapIntervalEXT = NULL;
PFNWGLEXTGETSWAPINTERVALPROC wglGetSwapIntervalEXT = NULL;

void InitVSync();
void SetVSyncState(bool enable);

// Function declarations
GLvoid KillGLWindow(GLvoid);
void handleResize(int w, int h);

// Temporary functions to get stuff on screen
void render();


#endif

