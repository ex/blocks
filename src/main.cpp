/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   A simple tetris clone.                                                   */
/*                                                                            */
/*   Copyright (c) 2010 Laurens Rodriguez Oscanoa.                            */
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

#include <stdlib.h>
#include "game.hpp"
#include "sdl/sdl_game.hpp"

int main(int argc, char *argv[]) {
    /* Create new game. */
    StcGame *game = new StcGame();

    /* Check if we got a valid game. */
    if (game != NULL) {
        /* We got a valid game, therefore start the game. */
        int errorCode = game->init(new StcPlatformSdl());
        if (errorCode == GAME_ERROR_NONE) {

            /* Loop until some error happens or the user quits. */
            while (game->errorCode == GAME_ERROR_NONE) {
                game->update();
            }
            /* Save error code and end game. */
            errorCode = game->errorCode;
            game->end();
        }
        /* Free resources used by our game. */
        delete game;

        /* Return to the system. */
        return errorCode;
    }
    /* If we get here the game was not created. */
    return GAME_ERROR_NO_MEMORY;
}
