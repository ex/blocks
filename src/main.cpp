/* ========================================================================= */
/*                          STC - SIMPLE TETRIS CLONE                        */
/* ------------------------------------------------------------------------- */
/*   A simple tetris clone.                                                  */
/*   OpenGL branch using some parts of source code found in the book:        */
/*   Beginning OpenGL, second edition by Luke Benstead.                      */
/*                                                                           */
/*   Some symbols you can define for the project:                            */
/*                                                                           */
/*   STC_SHOW_GHOST_PIECE:      define this for showing the shadow piece.    */
/*                              http://tetris.wikia.com/wiki/Ghost_piece     */
/*                                                                           */
/*   STC_WALL_KICK_ENABLED:     define this for enabling wall kick.          */
/*                              http://tetris.wikia.com/wiki/Wall_kick       */
/*                                                                           */
/*   STC_AUTO_ROTATION:         define this for enabling auto-rotation of    */
/*                              the falling piece.                           */
/*                                                                           */
/* ------------------------------------------------------------------------- */
/*   Copyright (c) 2011 Laurens Rodriguez Oscanoa.                           */
/*                                                                           */
/*   Permission is hereby granted, free of charge, to any person             */
/*   obtaining a copy of this software and associated documentation          */
/*   files (the "Software"), to deal in the Software without                 */
/*   restriction, including without limitation the rights to use,            */
/*   copy, modify, merge, publish, distribute, sublicense, and/or sell       */
/*   copies of the Software, and to permit persons to whom the               */
/*   Software is furnished to do so, subject to the following                */
/*   conditions:                                                             */
/*                                                                           */
/*   The above copyright notice and this permission notice shall be          */
/*   included in all copies or substantial portions of the Software.         */
/*                                                                           */
/*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,         */
/*   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES         */
/*   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                */
/*   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT             */
/*   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,            */
/*   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING            */
/*   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR           */
/*   OTHER DEALINGS IN THE SOFTWARE.                                         */
/* ------------------------------------------------------------------------- */
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include <windows.h>
#include "PlatformGL.hpp"
#include "../../../trunk/src/game.hpp"

using stc::Game;
using stc::PlatformGL;

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR cmdLine,
                   int cmdShow)
{
    // Game object.
    Game game;

    // Platform object.
    PlatformGL platform(hInstance);

    // Start the game.
    game.init(&platform);

    // Loop until some error happens or the user quits.
    while (game.errorCode() == Game::ERROR_NONE) {
        game.update();
    }

    // Game was interrupted or an error happened, end the game.
    game.end();

    // Return to the system.
    return game.errorCode();
}
