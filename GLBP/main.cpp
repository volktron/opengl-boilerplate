#include "main.h"

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/


//init VSync func
void InitVSync()
{
   //get extensions of graphics card
   char* extensions = (char*)glGetString(GL_EXTENSIONS);
  
   //is WGL_EXT_swap_control in the string? VSync switch possible?
	if (strstr(extensions,"WGL_EXT_swap_control"))
	{
		//get address's of both functions and save them
		wglSwapIntervalEXT = (PFNWGLEXTSWAPCONTROLPROC)
			wglGetProcAddress("wglSwapIntervalEXT");
		wglGetSwapIntervalEXT = (PFNWGLEXTGETSWAPINTERVALPROC)
			wglGetProcAddress("wglGetSwapIntervalEXT");
	}
}
 
bool VSyncEnabled()
{
   //if interval is positif, it is not 0 so enabled ;)
   return (wglGetSwapIntervalEXT() > 0);
}
 
void SetVSyncState(bool enable)
{
    if (enable)
       wglSwapIntervalEXT(1); //set interval to 1 -&gt; enable
    else
       wglSwapIntervalEXT(0); //disable
}

// WinMain
int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	
	// Let's get a console going for debug purposes
	AllocConsole();
	FILE* stream;
	freopen_s(&stream,"conin$","r",stdin);
	freopen_s(&stream,"conout$","w",stdout);
	freopen_s(&stream,"conout$","w",stderr);
	
	// Ask The User Which Screen Mode They Prefer
	//if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
	//{
	//	fullscreen=FALSE;							// Windowed Mode
	//}
	
	ENGINE->initialize();
	INPUT->initialize();

	// Game setup
	//InitVSync();
	//SetVSyncState(false);

	RENDERER->initialize(	&hDC, 
							&hRC, 
							&hWnd,
							&hInstance,
							fullscreen,
							width,
							height);
	
	
	
	// Start main loop
	while(!ENGINE->done)							// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				ENGINE->done=TRUE;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if (active)								// Program Active?
			{
				if (ENGINE->keys[VK_ESCAPE])		// Was ESC Pressed?
				{
					ENGINE->done=TRUE;				// ESC Signalled A Quit
				}
			}

			if (ENGINE->keys[VK_F1])						// Is F1 Being Pressed?
			{
				ENGINE->keys[VK_F1]=FALSE;					// If So Make Key FALSE

				RENDERER->pending_fullscreen = true;
				while(!RENDERER->thread_waiting);

				wglMakeCurrent(hDC,hRC);
				RENDERER->kill_gl_window();
				RENDERER->create_gl_window();
				RENDERER->init_gl_window();
				wglMakeCurrent(0,0);
				RENDERER->pending_fullscreen = false;
				
			}
		}
		INPUT->update();
		ENGINE->update();
	}

	// Shutdown
	RENDERER->shutdown();
	ENGINE->~Engine();

	FreeConsole();

	return (msg.wParam);							// Exit The Program
}

LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
		case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			if (!HIWORD(wParam))					// Check Minimization State
			{
				active=TRUE;						// Program Is Active
			}
			else
			{
				active=FALSE;						// Program Is No Longer Active
			}

			return 0;								// Return To The Message Loop
		}

		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch (wParam)							// Check System Calls
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;							// Prevent From Happening
			}
			break;									// Exit
		}

		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}

		case WM_KEYDOWN:							// Is A Key Being Held Down?
		{
			ENGINE->keys[wParam] = TRUE;					// If So, Mark It As TRUE

			return 0;								// Jump Back
		}

		case WM_KEYUP:								// Has A Key Been Released?
		{
			ENGINE->keys[wParam] = FALSE;					// If So, Mark It As FALSE
			
			return 0;								// Jump Back
		}
		case WM_SIZE:								// Resize The OpenGL Window
		{
			RENDERER->pending_width		= LOWORD(lParam);
			RENDERER->pending_height	= HIWORD(lParam);
			RENDERER->pending_resize	= true;
			
			return 0;								// Jump Back
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}