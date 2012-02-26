/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   A simple tetris clone for cocos2d-x.                                     */
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
/*   Copyright (c) 2012 Laurens Rodriguez Oscanoa.                            */
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

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN

// Windows Header Files
#include <windows.h>
#include <tchar.h>

// C RunTime Header Files
#include <CCStdC.h>

#include "../AppDelegate.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

    // Create the application instance
	AppDelegate app;

    return cocos2d::CCApplication::sharedApplication().run();
}
