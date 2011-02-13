/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Copyright (c) 2011 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */
#include "PlatformGL.hpp"

#include <ctime>
#include <iostream>
#include <windows.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "wgl/wglext.h"

typedef HGLRC (APIENTRYP PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);

PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;

PlatformGL::PlatformGL(HINSTANCE hInstance) {
    mIsRunning = false;
    mHandleInstance = hInstance;
    mLastTime = 0;
    mRotationAngle = 0.0f;
}

bool PlatformGL::create(int width, int height, int bpp, bool fullscreen) {
    DWORD      dwExStyle;       // Window Extended Style
    DWORD      dwStyle;         // Window Style

    mIsFullscreen = fullscreen; // Store the fullscreen flag

    mWindowRect.left = (long)0;        // Set Left Value To 0
    mWindowRect.right = (long)width;   // Set Right Value To Requested Width
    mWindowRect.top = (long)0;         // Set Top Value To 0
    mWindowRect.bottom = (long)height; // Set Bottom Value To Requested Height

    // Fill out the window class structure
    mWindowClass.cbSize          = sizeof(WNDCLASSEX);
    mWindowClass.style           = CS_HREDRAW | CS_VREDRAW;
    mWindowClass.lpfnWndProc     = PlatformGL::StaticWndProc; // We set our static method as the event handler
    mWindowClass.cbClsExtra      = 0;
    mWindowClass.cbWndExtra      = 0;
    mWindowClass.hInstance       = mHandleInstance;
    mWindowClass.hIcon           = LoadIcon(NULL, IDI_APPLICATION);  // default icon
    mWindowClass.hCursor         = LoadCursor(NULL, IDC_ARROW);      // default arrow
    mWindowClass.hbrBackground   = NULL;                             // don't need background
    mWindowClass.lpszMenuName    = NULL;                             // no menu
    mWindowClass.lpszClassName   = "GLClass";
    mWindowClass.hIconSm         = LoadIcon(NULL, IDI_WINLOGO);      // windows logo small icon

    // Register the windows class
    if (!RegisterClassEx(&mWindowClass)) {
        return false;
    }

    // If we are fullscreen, we need to change the display mode
    if (mIsFullscreen) {
        DEVMODE dmScreenSettings; // device mode
        
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize = sizeof(dmScreenSettings); 

        dmScreenSettings.dmPelsWidth = width;   // screen width
        dmScreenSettings.dmPelsHeight = height; // screen height
        dmScreenSettings.dmBitsPerPel = bpp;    // bits per pixel
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
            // setting display mode failed, switch to windowed
            MessageBox(NULL, "Display mode failed", NULL, MB_OK);
            mIsFullscreen = false; 
        }
    }

    // Are We Still In Fullscreen Mode?
    if (mIsFullscreen) {
        dwExStyle = WS_EX_APPWINDOW; // Window Extended Style
        dwStyle = WS_POPUP;          // Windows Style
        ShowCursor(false);           // Hide Mouse Pointer
    }
    else {
        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; // Window Extended Style
        dwStyle = WS_OVERLAPPEDWINDOW;                  // Windows Style
    }

    // Adjust Window To True Requested Size
    AdjustWindowRectEx(&mWindowRect, dwStyle, false, dwExStyle);     

    // Class registered, so now create our window
    mHandleWindow = CreateWindowEx(NULL,       // extended style
                                   "GLClass",  // class name
                                   "stc - OpenGL", // app name
                                   dwStyle | WS_CLIPCHILDREN |WS_CLIPSIBLINGS,
                                   0, 0,   // x,y coordinate
                                   mWindowRect.right - mWindowRect.left,
                                   mWindowRect.bottom - mWindowRect.top, // width, height
                                   NULL,   // handle to parent
                                   NULL,   // handle to menu
                                   mHandleInstance, // application instance
                                   this);  // we pass a pointer to the PlatformGL here

    // Check if window creation failed (hwnd would equal NULL)
    if (!mHandleWindow) {
        return 0;
    }

    mHandleDeviceContext = GetDC(mHandleWindow);
    ShowWindow(mHandleWindow, SW_SHOW); // display the window
    UpdateWindow(mHandleWindow);        // update the window

    mLastTime = GetTickCount() / 1000.0f; // Initialize the time
    return true;
}

void PlatformGL::destroy() {
    if (mIsFullscreen) {
        ChangeDisplaySettings(NULL, 0); // If So Switch Back To The Desktop
        ShowCursor(true);               // Show Mouse Pointer
    }
}

bool PlatformGL::isRunning() {
    return mIsRunning;
}

void PlatformGL::processEvents() {
    MSG msg;

    // While there are messages in the queue, store them in msg
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        // Process the messages one-by-one
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void PlatformGL::setupPixelFormat(void) {
    int pixelFormat;

    PIXELFORMATDESCRIPTOR pfd = {   
        sizeof(PIXELFORMATDESCRIPTOR),  // size
        1,                          // version
        PFD_SUPPORT_OPENGL |        // OpenGL window
        PFD_DRAW_TO_WINDOW |        // render to window
        PFD_DOUBLEBUFFER,           // support double-buffering
        PFD_TYPE_RGBA,              // color type
        32,                         // prefered color depth
        0, 0, 0, 0, 0, 0,           // color bits (ignored)
        0,                          // no alpha buffer
        0,                          // alpha bits (ignored)
        0,                          // no accumulation buffer
        0, 0, 0, 0,                 // accum bits (ignored)
        16,                         // depth buffer
        0,                          // no stencil buffer
        0,                          // no auxiliary buffers
        PFD_MAIN_PLANE,             // main layer
        0,                          // reserved
        0, 0, 0,                    // no layer, visible, damage masks
    };

    pixelFormat = ChoosePixelFormat(mHandleDeviceContext, &pfd);
    SetPixelFormat(mHandleDeviceContext, pixelFormat, &pfd);
}

LRESULT PlatformGL::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch(uMsg) {
    case WM_CREATE: // Window creation
        {
            mHandleDeviceContext = GetDC(hWnd);
            setupPixelFormat();

            // Set the version that we want, in this case 3.0
            int attribs[] = {
	            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
	            WGL_CONTEXT_MINOR_VERSION_ARB, 0,
                0   //zero indicates the end of the array
            }; 

            // Create temporary context so we can get a pointer to the function
            HGLRC tmpContext = wglCreateContext(mHandleDeviceContext);
            // Make it current
            wglMakeCurrent(mHandleDeviceContext, tmpContext);

            // Get the function pointer
            wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");

            // If this is NULL then OpenGL 3.0 is not supported
            if (!wglCreateContextAttribsARB) {
			    std::cerr << "OpenGL 3.0 is not supported, falling back to GL 2.1" << std::endl;
                mHandleRenderContext = tmpContext;
            } 
		    else {
			    // Create an OpenGL 3.0 context using the new function
			    mHandleRenderContext = wglCreateContextAttribsARB(mHandleDeviceContext, 0, attribs);
			    // Delete the temporary context
			    wglDeleteContext(tmpContext);
		    }

            // Make the GL3 context current
            wglMakeCurrent(mHandleDeviceContext, mHandleRenderContext);

            mIsRunning = true; // Mark our window as running
        }
        break;

    case WM_DESTROY: // window destroy
    case WM_CLOSE:   // windows is closing
        wglMakeCurrent(mHandleDeviceContext, NULL);
        wglDeleteContext(mHandleRenderContext);
        mIsRunning = false; // Stop the main loop
        PostQuitMessage(0);  // Send a WM_QUIT message
        return 0;

    case WM_SIZE:
        {
            int height = HIWORD(lParam); // retrieve width and height
            int width = LOWORD(lParam);
			onResize(width, height);
        }
        break;

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) { //If the escape key was pressed
            DestroyWindow(mHandleWindow); //Send a WM_DESTROY message
        }
        break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK PlatformGL::StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    PlatformGL* window = NULL;

    // If this is the create message
    if (uMsg == WM_CREATE) {
        // Get the pointer we stored during create
        window = (PlatformGL*)((LPCREATESTRUCT)lParam)->lpCreateParams;

        // Associate the window pointer with the hwnd for the other events to access
        SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)window);
    }
    else {
        // If this is not a creation event, then we should have stored a pointer to the window
        window = (PlatformGL*)GetWindowLongPtr(hWnd, GWL_USERDATA);

        if (!window) {
            return DefWindowProc(hWnd, uMsg, wParam, lParam);    
        }
    }

    // Call our window's member WndProc (allows us to access member variables)
    return window->WndProc(hWnd, uMsg, wParam, lParam);
}

float PlatformGL::getElapsedSeconds() {
    float currentTime = float(GetTickCount()) / 1000.0f;
    float seconds = float(currentTime - mLastTime);
    mLastTime = currentTime;
    return seconds;
}

bool PlatformGL::init() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
    // Return success
    return true;
}

void PlatformGL::prepare(float dt) {
    const float SPEED = 15.0f;
    mRotationAngle += SPEED * dt;
    if (mRotationAngle > 360.0f) {
        mRotationAngle -= 360.0f;
    }
}

void PlatformGL::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glRotatef(mRotationAngle, 0, 0, 1);

	glBegin(GL_TRIANGLES);
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	    glVertex3f(-1.0f, -0.5f, -4.0f);
		glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, -0.5f, -4.0f);
		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		glVertex3f(0.0f,  0.5f, -4.0f);
	glEnd();
}

void PlatformGL::shutdown() {
}

void PlatformGL::onResize(int width, int height) {
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f, float(width) / float(height), 1.0f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

