/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   A simple tetris clone.                                                   */
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

#include "game.h"

int main(int argc, char *argv[]) {
    /* Game object */
    StcGame game;

    /* Start the game */
    gameInit(&game);

    /* Loop until some error happens or the user quits */
    while (game.errorCode == ERROR_NONE) {
        gameUpdate(&game);
    }

    /* Game was interrupted or an error happened, end the game */
    gameEnd(&game);

    /*  Return to the system */
    return game.errorCode;
}
