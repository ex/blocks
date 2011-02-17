/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   A simple tetris clone.                                                   */
/*   OpenGL branch using sample code found in the book:                       */
/*   Beginning OpenGL, second edition by Luke Bendstead.                      */
/*                                                                            */
/*   Some symbols you can define for the project:                             */
/*                                                                            */
/*   STC_SHOW_GHOST_PIECE:      define this for showing the shadow piece.     */
/*                              http://tetris.wikia.com/wiki/Ghost_piece      */
/*                                                                            */
/*   STC_WALL_KICK_ENABLED:     define this for enabling wall kick.           */
/*                              http://tetris.wikia.com/wiki/Wall_kick        */
/*                                                                            */
/*   STC_AUTO_ROTATION:         define this for enabling auto-rotation of     */
/*                              the falling piece.                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/*   Copyright (c) 2011 Laurens Rodriguez Oscanoa.                            */
/*                                                                            */
/*   Permission is hereby granted, free of charge, to any person              */
/*   obtaining a copy of this software and associated documentation           */
/*   files (the "Software"), to deal in the Software without                  */
/*   restriction, including without limitation the rights to use,             */
/*   copy, modify, merge, publish, distribute, sublicense, and/or sell        */
/*   copies of the Software, and to permit persons to whom the                */
/*   Software is furnished to do so, subject to the following                 */
/*   conditions:                                                              */
/*                                                                            */
/*   The above copyright notice and this permission notice shall be           */
/*   included in all copies or substantial portions of the Software.          */
/*                                                                            */
/*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,          */
/*   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES          */
/*   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                 */
/*   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT              */
/*   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,             */
/*   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING             */
/*   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR            */
/*   OTHER DEALINGS IN THE SOFTWARE.                                          */
/* -------------------------------------------------------------------------- */
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

// Must be declared so that our local glxext.h is picked up, rather than the system one
#define GLX_GLXEXT_LEGACY 

#ifdef _WIN32
#include <windows.h>
#include "PlatformGL.hpp"
#else
#include "PlatformGLX.hpp"
#endif

#include "targa/TargaImage.h"

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR cmdLine,
                   int cmdShow) {
#else
int main(int argc, char** argv) {
#endif

#ifdef _WIN32
    // This is our window.
    PlatformGL platform(hInstance);
#else
    SimpleGLXWindow programWindow;
#endif

    TargaImage t;
    t.load("numbers.tga");

    // Attempt to create the window.
    if (!platform.create()) {
        // If it fails
#ifdef _WIN32
        MessageBox(NULL, "Unable to create the OpenGL Window", "An error occurred", MB_ICONERROR | MB_OK);
#endif
        platform.destroy(); // Reset the display and exit
        return 1;
    }

    // Initialize our example.
    if (!platform.init()) {
#ifdef _WIN32
        MessageBox(NULL, "Could not initialize the application", "An error occurred", MB_ICONERROR | MB_OK);
#endif
        platform.destroy(); // Reset the display and exit
        return 1;
    }

    // This is the mainloop, we render frames until isRunning returns false.
    while (platform.isRunning()) {
        platform.processEvents(); // Process any window events

        // We get the time that passed since the last frame.
        float elapsedTime = platform.getElapsedSeconds();

        platform.prepare(elapsedTime); // Do any pre-rendering logic.
        platform.render(); // Render the scene.
        platform.swapBuffers();
    }

    platform.shutdown(); // Free any resources.
    platform.destroy();  // Destroy the program window.
    return 0; // Return success.
}
