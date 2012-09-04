#include "renderer.h"
#include "engine.h"
#include "wndproc.h"

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
			if(RENDERER->pending_fullscreen)
			{
				RENDERER->fullscreen = !RENDERER->fullscreen;
				wglMakeCurrent(0,0);
				RENDERER->thread_waiting = true;
				
				while (RENDERER->pending_fullscreen);
				
				RENDERER->thread_waiting = false;
				wglMakeCurrent(*hDC,*hRC);
			}

			// Render!
			RENDERER->render();
			SwapBuffers(*hDC);				// Swap Buffers (Double Buffering)
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
							bool		fullscreen,
							int			width,
							int			height)
{
	this->hDC	= hDC;
	this->hRC	= hRC;
	this->hWnd	= hWnd;
	this->hInstance = hInstance;

	this->pending_fullscreen	= false;
	this->pending_kill_gl		= false;
	this->thread_waiting		= false;
	this->fullscreen			= fullscreen;

	this->g_width = width;
	this->g_height = height;
	
	this->create_gl_window();
	this->init_gl_window();
	wglMakeCurrent(0,0);
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

BOOL Renderer::create_gl_window()
{
	GLuint		PixelFormat;					// Holds The Results After Searching For A Match
	WNDCLASS	wc;								// Windows Class Structure
	DWORD		dwExStyle;						// Window Extended Style
	DWORD		dwStyle;						// Window Style
	RECT		WindowRect;						// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;					// Set Left Value To 0
	WindowRect.right=(long)this->g_width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;						// Set Top Value To 0
	WindowRect.bottom=(long)g_height;				// Set Bottom Value To Requested Height

	
	
	*this->hInstance	= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= *this->hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	
	if (this->fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= this->g_width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= this->g_height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= 32;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (this->fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(*this->hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
										"OpenGL",							// Class Name
										"GL Boilerplate",					// Window Title
										dwStyle |							// Defined Window Style
										WS_CLIPSIBLINGS |					// Required Window Style
										WS_CLIPCHILDREN,					// Required Window Style
										0, 0,								// Window Position
										WindowRect.right-WindowRect.left,	// Calculate Window Width
										WindowRect.bottom-WindowRect.top,	// Calculate Window Height
										NULL,								// No Parent Window
										NULL,								// No Menu
										*this->hInstance,					// Instance
										NULL)))								// Dont Pass Anything To WM_CREATE
	{
		kill_gl_window();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		32,											// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// Main Drawing Layer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(*this->hDC = GetDC(*this->hWnd)))					// Did We Get A Device Context?
	{
		kill_gl_window();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(*this->hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		kill_gl_window();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(*this->hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		kill_gl_window();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(*this->hRC = wglCreateContext(*this->hDC)))				// Are We Able To Get A Rendering Context?
	{
		kill_gl_window();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(*this->hDC,*this->hRC))					// Try To Activate The Rendering Context
	{
		kill_gl_window();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(*this->hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(*this->hWnd);						// Slightly Higher Priority
	SetFocus(*this->hWnd);									// Sets Keyboard Focus To The Window
	RENDERER->pending_width		= this->g_width;
	RENDERER->pending_height	= this->g_height;
	RENDERER->pending_resize	= true;
	RENDERER->handle_resize();					// Set Up Our Perspective GL Screen

	return TRUE;									// Success
}

void Renderer::init_gl_window()
{
	glewInit();
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
}