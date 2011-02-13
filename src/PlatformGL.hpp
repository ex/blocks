/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   PlatformGL interface definition.                                           */
/*                                                                            */
/*   Copyright (c) 2010 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */
#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <windows.h>

class PlatformGL {
  public:
    PlatformGL(HINSTANCE hInstance);

    bool create(int width, int height, int bpp, bool fullscreen);
    void destroy();
    void processEvents();
    
    bool isRunning(); // Is the window running?.
    float getElapsedSeconds();

    void swapBuffers() { SwapBuffers(mHandleDeviceContext); }

    static LRESULT CALLBACK StaticWndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

    bool init();
    void prepare(float dt);
    void render();
    void shutdown();
    void onResize(int width, int height);

  private:
    void setupPixelFormat(void);

    bool mIsRunning; // Is the window still running?.
    bool mIsFullscreen; 

    float mLastTime;
    float mRotationAngle;

    HWND       mHandleWindow;        // Window handle
    HGLRC      mHandleRenderContext; // Rendering context
    HDC        mHandleDeviceContext; // Device context
    RECT       mWindowRect;          // Window bounds
    HINSTANCE  mHandleInstance;      // Application instance
    WNDCLASSEX mWindowClass;
};

#endif
