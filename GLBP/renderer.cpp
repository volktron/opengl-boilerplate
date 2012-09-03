#include "renderer.h"
#include "engine.h"

// OpenGL includes
#include "glew.h"
#include <gl\gl.h>		// Header File For The OpenGL32 Library
#include <gl\glu.h>		
#include "wglext.h"

#include "thread.h"

class Render_Thread : public Thread
{
public:
	bool done;
	HDC* hDC;
	HGLRC* hRC;

	void render()
	{
		RENDERER->render();
		SwapBuffers(*hDC);				// Swap Buffers (Double Buffering)
	}

	virtual DWORD Run( LPVOID /* arg */ )
	{
		wglMakeCurrent(*hDC,*hRC);
		while(!done)
		{
			// Check for events
			if(RENDERER->pending_resize)
			{
				RENDERER->pending_resize = false;
				RENDERER->handle_resize();
			}
			if(RENDERER->pending_kill_gl)
			{
				RENDERER->pending_kill_gl = false;
				RENDERER->kill_gl_window();
			}

			// Render!
			render();
		}

		return true;
	}

	DWORD Stop ( bool bForceKill = false )
	{
		// Shutdown the gl context
		RENDERER->pending_kill_gl = true;
		while(RENDERER->pending_kill_gl);

		return Thread::Stop(bForceKill);
	}
};

Renderer* Renderer::renderer = 0;
bool Renderer::instanceflag = false;

Renderer::Renderer()
{
	render_thread = new Render_Thread();
	((Render_Thread*)render_thread)->done = false;
}

void Renderer::initialize(	HDC*		hDC, 
							HGLRC*		hRC, 
							HWND*		hWnd, 
							HINSTANCE*	hInstance,
							bool		fullscreen)
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.4f, 0.3f, 0.2f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	wglMakeCurrent(0,0);
	this->hDC	= hDC;
	this->hRC	= hRC;
	this->hWnd	= hWnd;

	this->pending_fullscreen	= false;
	this->pending_kill_gl		= false;
	((Render_Thread*)render_thread)->hDC = hDC;
	((Render_Thread*)render_thread)->hRC = hRC;
	((Render_Thread*)render_thread)->Start();
}

Renderer::~Renderer()
{
	((Render_Thread*)render_thread)->Stop();
}

Renderer* Renderer::get_instance()
{
	if (instanceflag)
		return renderer;

	instanceflag = true;
	renderer = new Renderer();

	return renderer;
}

void Renderer::handle_resize()
{
	g_width = pending_width;
	g_height = pending_height;

	if (g_height == 0)										// Prevent A Divide By Zero By
	{
		g_height = 1;										// Making Height Equal One
	}

	glViewport(0, 0, g_width, g_height);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluPerspective(45.0,
					(double)g_width / (double)g_height,
					0.01,
					10000000.0);
	glMatrixMode(GL_MODELVIEW);

	//InitVSync();
	//SetVSyncState(false);
}

void Renderer::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear The Screen And The Depth Buffer
    glLoadIdentity();
	
	double cam_x = ENGINE->scenes[0]->cameras[0]->body->position->x;
	double cam_y = ENGINE->scenes[0]->cameras[0]->body->position->y;
	double cam_z = ENGINE->scenes[0]->cameras[0]->body->position->z;
	glTranslated(	-cam_x,
					-cam_y, 
					-cam_z	);
	
	int num_entities = ENGINE->scenes[0]->entities.size();
	for(int i = 0; i < num_entities; i++)
	{
		glPushMatrix();
		glRotated(ENGINE->scenes[0]->entities[i]->body->rotation->x, 1.0, 0.0, 0.0);
		glRotated(ENGINE->scenes[0]->entities[i]->body->rotation->y, 0.0, 1.0, 0.0);
		glRotated(ENGINE->scenes[0]->entities[i]->body->rotation->z, 0.0, 0.0, 1.0);
		glBegin(GL_TRIANGLES);                      // Drawing Using Triangles
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f( 0.0f, 1.0f, 0.0f);              // Top
		glColor3f(0.0f,1.0f,0.0f);
		glVertex3f(-1.0f,-1.0f, 0.0f);              // Bottom Left
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f( 1.0f,-1.0f, 0.0f);              // Bottom Right
		glEnd();
		glPopMatrix();
		glTranslatef(1.5f,0.0f,0.0f);   
	}

	   
}

// Kill the window
void Renderer::kill_gl_window()								// Properly Kill The Window
{
	if (this->fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (*this->hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(*this->hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		*this->hRC=NULL;										// Set RC To NULL
	}

	if (*this->hDC && !ReleaseDC(*this->hWnd, *this->hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		*this->hDC=NULL;										// Set DC To NULL
	}

	if (*this->hWnd && !DestroyWindow(*this->hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		*this->hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",*this->hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		*this->hInstance=NULL;									// Set hInstance To NULL
	}
}
