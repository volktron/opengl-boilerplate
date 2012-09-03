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

			// Render!
			render();
		}

		return true;
	}

	DWORD Stop ( bool bForceKill = false )
	{
		wglMakeCurrent(NULL, NULL);
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

void Renderer::initialize(HDC* hDC, HGLRC* hRC)
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
	this->hDC = hDC;
	this->hRC = hRC;

	((Render_Thread*)render_thread)->hDC = hDC;
	((Render_Thread*)render_thread)->hRC = hRC;
	((Render_Thread*)render_thread)->Start();
}

Renderer::~Renderer()
{
	((Render_Thread*)render_thread)->Stop();
}

Renderer* Renderer::getInstance()
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
	glTranslatef(	-cam_x,
					-cam_y, 
					-cam_z	);
	
	int num_entities = ENGINE->scenes[0]->entities.size();
	for(int i = 0; i < num_entities; i++)
	{
		glPushMatrix();
		glRotatef(ENGINE->scenes[0]->entities[i]->body->rotation->x, 1.0f, 0.0f, 0.0f);
		glRotatef(ENGINE->scenes[0]->entities[i]->body->rotation->y, 0.0f, 1.0f, 0.0f);
		glRotatef(ENGINE->scenes[0]->entities[i]->body->rotation->z, 0.0f, 0.0f, 1.0f);
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

