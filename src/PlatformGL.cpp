/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Copyright (c) 2011 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */
#include "PlatformGL.hpp"

#include <assert.h>
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
			    // OpenGL 3.0 is not supported, falling back to GL 2.1
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
        switch (wParam) {
        case VK_ESCAPE: // If the escape key was pressed
            DestroyWindow(mHandleWindow); // Send a WM_DESTROY message
            break;

        case 'S':
        case VK_DOWN:
            mGame->onEventStart(Game::EVENT_MOVE_DOWN);
            break;
        case 'W':
        case VK_UP:
            mGame->onEventStart(Game::EVENT_ROTATE_CW);
            break;
        case 'A':
        case VK_LEFT:
            mGame->onEventStart(Game::EVENT_MOVE_LEFT);
            break;
        case 'D':
        case VK_RIGHT:
            mGame->onEventStart(Game::EVENT_MOVE_RIGHT);
            break;
        case VK_SPACE:
            mGame->onEventStart(Game::EVENT_DROP);
            break;
        case VK_F5:
            mGame->onEventStart(Game::EVENT_RESTART);
            break;
        case VK_F1:
            mGame->onEventStart(Game::EVENT_PAUSE);
            break;
        case VK_F2:
            mGame->onEventStart(Game::EVENT_SHOW_NEXT);
            break;
#ifdef STC_SHOW_GHOST_PIECE
        case VK_F3:
            mGame->onEventStart(Game::EVENT_SHOW_SHADOW);
            break;
#endif /* STC_SHOW_GHOST_PIECE */
        }
        break;

    case WM_KEYUP:
        switch (wParam) {
        case 'S':
        case VK_DOWN:
            mGame->onEventEnd(Game::EVENT_MOVE_DOWN);
            break;
        case 'A':
        case VK_LEFT:
            mGame->onEventEnd(Game::EVENT_MOVE_LEFT);
            break;
        case 'D':
        case VK_RIGHT:
            mGame->onEventEnd(Game::EVENT_MOVE_RIGHT);
            break;
#ifdef STC_AUTO_ROTATION
        case 'W':
        case VK_UP:
            mGame->onEventEnd(Game::EVENT_ROTATE_CW);
            break;
#endif /* STC_AUTO_ROTATION */
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

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

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

    assert(mTexture->getWidth() == TEXTURE_SIZE);
    assert(mTexture->getHeight() == TEXTURE_SIZE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXTURE_SIZE, TEXTURE_SIZE,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, mTexture->getImageData());

    // Initialize background texture coordinates
    mBackgroundTexCoord = new GLfloat[8];
    mBackgroundTexCoord[0] = 0.f;
    mBackgroundTexCoord[1] = 0.f;
    mBackgroundTexCoord[2] = SCREEN_WIDTH / GLfloat(TEXTURE_SIZE);
    mBackgroundTexCoord[3] = 0.f;
    mBackgroundTexCoord[4] = 0.f;
    mBackgroundTexCoord[5] = SCREEN_HEIGHT / GLfloat(TEXTURE_SIZE);
    mBackgroundTexCoord[6] = SCREEN_WIDTH / GLfloat(TEXTURE_SIZE);
    mBackgroundTexCoord[7] = SCREEN_HEIGHT / GLfloat(TEXTURE_SIZE);

    mBackgroundVertices = new GLfloat[12];
    mBackgroundVertices[0] = 0.f;                    // down-left
    mBackgroundVertices[1] = GLfloat(SCREEN_HEIGHT);
    mBackgroundVertices[2] = 0.f;
    mBackgroundVertices[3] = GLfloat(SCREEN_WIDTH);  // down-right
    mBackgroundVertices[4] = GLfloat(SCREEN_HEIGHT);
    mBackgroundVertices[5] = 0.f;
    mBackgroundVertices[6] = 0.f;                    // up-left
    mBackgroundVertices[7] = 0.f;
    mBackgroundVertices[8] = 0.f;
    mBackgroundVertices[9] = GLfloat(SCREEN_WIDTH);  // up-right
    mBackgroundVertices[10] = 0.f;
    mBackgroundVertices[11] = 0.f;

    // Initialize indexes used for drawing sprites
    mTextureIndexes = new GLubyte[4];
    for (int k = 0; k < 4; mTextureIndexes[k++] = k);

    // Create arrays for drawing the sprites
    mSpriteTexCoord = new GLfloat[8];
    mSpriteVertices = new GLfloat[12];

    // We don't change textures or 2D mode, so do this only once.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // Save reference to game and return that everything is OK
    mGame = game;
    return Game::ERROR_NONE;
}

void PlatformGL::end() {
    // Destroy the program window
    if (mIsFullscreen) {
        ChangeDisplaySettings(NULL, 0); // if so switch back to the desktop
        ShowCursor(true);               // show mouse pointer
    }

    delete[] mSpriteTexCoord;
    delete[] mSpriteVertices;
    delete[] mBackgroundVertices;
    delete[] mBackgroundTexCoord;
    delete[] mTextureIndexes;

    delete mTexture;
    mTexture = NULL;
}

// Set the texture coordinates used for rendering a sprite.
void PlatformGL::setSpriteTextureCoord(GLfloat *coords, int x, int y, int w, int h) {
    // down-left
    coords[0] = x / GLfloat(TEXTURE_SIZE);   
    coords[1] = (TEXTURE_SIZE - y - h) / GLfloat(TEXTURE_SIZE);
    // down-right
    coords[2] = (x + w) / GLfloat(TEXTURE_SIZE);   
    coords[3] = (TEXTURE_SIZE - y - h) / GLfloat(TEXTURE_SIZE);
    // up-left
    coords[4] = x / GLfloat(TEXTURE_SIZE);   
    coords[5] = (TEXTURE_SIZE - y) / GLfloat(TEXTURE_SIZE);
    // up-right
    coords[6] = (x + w) / GLfloat(TEXTURE_SIZE);   
    coords[7] = (TEXTURE_SIZE - y) / GLfloat(TEXTURE_SIZE);
}

// Draw a tile from a tetromino
void PlatformGL::drawTile(int x, int y, int tile, bool shadow) {
    mSpriteVertices[0] = GLfloat(x);             // down-left
    mSpriteVertices[1] = GLfloat(y + TILE_SIZE);
    mSpriteVertices[2] = 0.f;
    mSpriteVertices[3] = GLfloat(x + TILE_SIZE); // down-right
    mSpriteVertices[4] = GLfloat(y + TILE_SIZE);
    mSpriteVertices[5] = 0.f;
    mSpriteVertices[6] = GLfloat(x);             // up-left
    mSpriteVertices[7] = GLfloat(y);
    mSpriteVertices[8] = 0.f;
    mSpriteVertices[9] = GLfloat(x + TILE_SIZE); // up-right
    mSpriteVertices[10] = GLfloat(y);
    mSpriteVertices[11] = 0.f;

    setSpriteTextureCoord(mSpriteTexCoord, TILE_SIZE * tile, (TILE_SIZE + 1) * shadow, TILE_SIZE + 1, TILE_SIZE + 1);

    glVertexPointer(3, GL_FLOAT, 0, mSpriteVertices);
    glTexCoordPointer(2, GL_FLOAT, 0, mSpriteTexCoord);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, mTextureIndexes);
}

// Draw a number on the given position
void PlatformGL::drawNumber(int x, int y, long number, int length, int color) {
    //SDL_Rect recDestine;
    //SDL_Rect recSource;

    //recSource.y = NUMBER_HEIGHT * color;
    //recSource.w = NUMBER_WIDTH;
    //recSource.h = NUMBER_HEIGHT;
    //recDestine.y = y;

    //int pos = 0;
    //do {
    //    recDestine.x = x + NUMBER_WIDTH * (length - pos);
    //    recSource.x = NUMBER_WIDTH * (Sint16)(number % 10);
    //    SDL_BlitSurface(mBmpNumbers, &recSource, mScreen, &recDestine);
    //    number /= 10;
    //} while (++pos < length);
}

// Render the state of the game using platform functions
void PlatformGL::renderGame() {
    int i, j;

    // Check if the game state has changed, if so redraw
    if (mGame->hasChanged()) {

        // Draw background
        glVertexPointer(3, GL_FLOAT, 0, mBackgroundVertices);
        glTexCoordPointer(2, GL_FLOAT, 0, mBackgroundTexCoord);
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, mTextureIndexes);

        // Draw preview block
        if (mGame->showPreview()) {
            for (i = 0; i < Game::TETROMINO_SIZE; ++i) {
                for (j = 0; j < Game::TETROMINO_SIZE; ++j) {
                    if (mGame->nextBlock().cells[i][j] != Game::EMPTY_CELL) {
                        drawTile(PREVIEW_X + (TILE_SIZE * i),
                                 PREVIEW_Y + (TILE_SIZE * j),
                                 mGame->nextBlock().cells[i][j], false);
                    }
                }
            }
        }
#ifdef STC_SHOW_GHOST_PIECE
        // Draw shadow tetromino
        if (mGame->showShadow() && mGame->shadowGap() > 0) {
            for (i = 0; i < Game::TETROMINO_SIZE; ++i) {
                for (j = 0; j < Game::TETROMINO_SIZE; ++j) {
                    if (mGame->fallingBlock().cells[i][j] != Game::EMPTY_CELL) {
                        drawTile(BOARD_X + (TILE_SIZE * (mGame->fallingBlock().x + i)),
                                 BOARD_Y + (TILE_SIZE * (mGame->fallingBlock().y + mGame->shadowGap() + j)),
                                 mGame->fallingBlock().cells[i][j], true);
                    }
                }
            }
        }
#endif
        // Draw the cells in the board
        for (i = 0; i < Game::BOARD_TILEMAP_WIDTH; ++i) {
            for (j = 0; j < Game::BOARD_TILEMAP_HEIGHT; ++j) {
                if (mGame->getCell(i, j) != Game::EMPTY_CELL) {
                    drawTile(BOARD_X + (TILE_SIZE * i),
                             BOARD_Y + (TILE_SIZE * j),
                             mGame->getCell(i, j), false);
                }
            }
        }

        // Draw falling tetromino
        for (i = 0; i < Game::TETROMINO_SIZE; ++i) {
            for (j = 0; j < Game::TETROMINO_SIZE; ++j) {
                if (mGame->fallingBlock().cells[i][j] != Game::EMPTY_CELL) {
                    drawTile(BOARD_X + (TILE_SIZE * (mGame->fallingBlock().x + i)),
                             BOARD_Y + (TILE_SIZE * (mGame->fallingBlock().y + j)),
                             mGame->fallingBlock().cells[i][j], false);
                }
            }
        }

        /* Draw game statistic data */
        if (!mGame->isPaused()) {
            drawNumber(LEVEL_X, LEVEL_Y, mGame->stats().level, LEVEL_LENGTH, Game::COLOR_WHITE);
            drawNumber(LINES_X, LINES_Y, mGame->stats().lines, LINES_LENGTH, Game::COLOR_WHITE);
            drawNumber(SCORE_X, SCORE_Y, mGame->stats().score, SCORE_LENGTH, Game::COLOR_WHITE);

            drawNumber(TETROMINO_X, TETROMINO_L_Y, mGame->stats().pieces[Game::TETROMINO_L], TETROMINO_LENGTH, Game::COLOR_ORANGE);
            drawNumber(TETROMINO_X, TETROMINO_I_Y, mGame->stats().pieces[Game::TETROMINO_I], TETROMINO_LENGTH, Game::COLOR_CYAN);
            drawNumber(TETROMINO_X, TETROMINO_T_Y, mGame->stats().pieces[Game::TETROMINO_T], TETROMINO_LENGTH, Game::COLOR_PURPLE);
            drawNumber(TETROMINO_X, TETROMINO_S_Y, mGame->stats().pieces[Game::TETROMINO_S], TETROMINO_LENGTH, Game::COLOR_GREEN);
            drawNumber(TETROMINO_X, TETROMINO_Z_Y, mGame->stats().pieces[Game::TETROMINO_Z], TETROMINO_LENGTH, Game::COLOR_RED);
            drawNumber(TETROMINO_X, TETROMINO_O_Y, mGame->stats().pieces[Game::TETROMINO_O], TETROMINO_LENGTH, Game::COLOR_YELLOW);
            drawNumber(TETROMINO_X, TETROMINO_J_Y, mGame->stats().pieces[Game::TETROMINO_J], TETROMINO_LENGTH, Game::COLOR_BLUE);

            drawNumber(PIECES_X, PIECES_Y, mGame->stats().totalPieces, PIECES_LENGTH, Game::COLOR_WHITE);
        }

        // Inform the game that we are done with the changed state
        mGame->onChangeProcessed();

        // Swap video buffers
        SwapBuffers(mHandleDeviceContext);
    }

    // Resting game
    Sleep(SLEEP_TIME);
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

