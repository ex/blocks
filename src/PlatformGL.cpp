/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Copyright (c) 2011 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */
#include "PlatformGL.hpp"

#include <stdlib.h>
#include <ctime>
#include <iostream>
#include <windows.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "wgl/wglext.h"

typedef HGLRC (APIENTRYP PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);

PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;

namespace Stc {

PlatformGL::PlatformGL(HINSTANCE hInstance) {
    mHandleInstance = hInstance;
    mGame = NULL;
    mTexture = NULL;
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

LRESULT PlatformGL::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: // Window creation
        {
            mHandleDeviceContext = GetDC(hWnd);

            // Setup pixel format
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
            int pixelFormat = ChoosePixelFormat(mHandleDeviceContext, &pfd);
            SetPixelFormat(mHandleDeviceContext, pixelFormat, &pfd);

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
        }
        break;
    
    case WM_DESTROY: // window destroy
    case WM_CLOSE:   // windows is closing
        wglMakeCurrent(mHandleDeviceContext, NULL);
        wglDeleteContext(mHandleRenderContext);
        PostQuitMessage(0); // Send a WM_QUIT message

        // Signal game end.
        mGame->onEventStart(Game::EVENT_QUIT);
        return 0;

    case WM_SIZE:
        {
            int height = HIWORD(lParam); // retrieve width and height
            int width = LOWORD(lParam);
            glViewport(0, 0, width, height);
        }
        break;

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) { // If the escape key was pressed
            DestroyWindow(mHandleWindow); // Send a WM_DESTROY message
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

int PlatformGL::init(Game *game) {
    // Create the window.
    // -------------------------------------------------------------------------
    DWORD dwExStyle;       // Window Extended Style
    DWORD dwStyle;         // Window Style

    mIsFullscreen = false; // Fullscreen mode fails for screen size of 480x272

    // Set initial position and size
    mWindowRect.left = long(0);        
    mWindowRect.right = long(SCREEN_WIDTH);
    mWindowRect.top = long(0);
    mWindowRect.bottom = long(SCREEN_HEIGHT);

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
        return Game::ERROR_PLATFORM;
    }

    // If we are fullscreen, we need to change the display mode
    if (mIsFullscreen) {
        DEVMODE dmScreenSettings; // device mode
        
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize = sizeof(dmScreenSettings); 

        dmScreenSettings.dmPelsWidth = SCREEN_WIDTH;      // screen width
        dmScreenSettings.dmPelsHeight = SCREEN_HEIGHT;    // screen height
        dmScreenSettings.dmBitsPerPel = SCREEN_BIT_DEPTH; // bits per pixel
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
            // Setting display mode failed, switch to windowed
            MessageBox(NULL, "Full screen display mode failed", NULL, MB_OK);
            mIsFullscreen = false; 
        }
    }

    // Are we still in fullscreen mode?
    if (mIsFullscreen) {
        dwExStyle = WS_EX_APPWINDOW; // Window extended style
        dwStyle = WS_POPUP;          // Windows style
        ShowCursor(false);           // Hide mouse pointer
    }
    else {
        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; // Window extended style
        dwStyle = WS_OVERLAPPEDWINDOW;                  // Windows style
    }

    // Adjust window to true requested size
    AdjustWindowRectEx(&mWindowRect, dwStyle, false, dwExStyle);     

    // Class registered, so now create our window
    mHandleWindow = CreateWindowEx(NULL,           // extended style
                                   "GLClass",      // class name
                                   "stc - OpenGL", // app name
                                   dwStyle | WS_CLIPCHILDREN |WS_CLIPSIBLINGS,
                                   0, 0,   // x,y coordinate
                                   mWindowRect.right - mWindowRect.left,
                                   mWindowRect.bottom - mWindowRect.top, // width, height
                                   NULL,   // handle to parent
                                   NULL,   // handle to menu
                                   mHandleInstance, // application instance
                                   this);  // we pass a pointer to the PlatformGL here

    // Check if window creation failed (handle equal NULL)
    if (mHandleWindow == NULL) {
        MessageBox(NULL, "Unable to create the OpenGL Window", "An error occurred", MB_ICONERROR | MB_OK);
        end(); // Reset the display and exit
        return Game::ERROR_PLATFORM;
    }

    mHandleDeviceContext = GetDC(mHandleWindow);
    ShowWindow(mHandleWindow, SW_SHOW); // display the window
    UpdateWindow(mHandleWindow);        // update the window

    // Setup OpenGL 2D view.
    // -------------------------------------------------------------------------
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0.375f, 0.375f, 0.f);

    // Make sure depth testing and lighting are disabled for 2D rendering
    glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT);
	glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Setup image texture
    // -------------------------------------------------------------------------
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mTexture = new TargaImage();
    if (!mTexture->load("back.tga")) {
        MessageBox(NULL, "Unable to load texture", "An error occurred", MB_ICONERROR | MB_OK);
        end(); // Reset the display and exit
        return Game::ERROR_NO_IMAGES;
    }

    glGenTextures(1, &mTextureId);
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int bmpWidth = mTexture->getWidth();
    int bmpHeight = mTexture->getHeight();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmpWidth, bmpHeight,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, mTexture->getImageData());

    mGame = game;
    return Game::ERROR_NONE;
}

void PlatformGL::end() {
    // Destroy the program window.
    if (mIsFullscreen) {
        ChangeDisplaySettings(NULL, 0); // if so switch back to the desktop
        ShowCursor(true);               // show mouse pointer
    }

    delete mTexture;
    mTexture = NULL;
}

void PlatformGL::renderGame() {
    // Render the background.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    GLfloat vertices[] = {0.f, 272.f, 0.f,
                          480.f, 272.f, 0.f,
                          0.f, 0.f, 0.f,
                          480, 0.f, 0.f};

    GLfloat texcoord[] = {0.f, 0.f,
                          480/512.f, 0.f,
                          0.f, 272/512.f,
                          480/512.f, 272/512.f};

    GLubyte indices[] = {0, 1, 2, 3};

    glBindTexture(GL_TEXTURE_2D, mTextureId);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, texcoord);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, indices);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    SwapBuffers(mHandleDeviceContext);
}

long PlatformGL::getSystemTime() {
    return GetTickCount();
}

void PlatformGL::seedRandom(long seed) {
    srand(seed);
}

int PlatformGL::random() {
    return rand();
}
}

