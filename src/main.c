/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   A very simple tetris clone.                                              */
/*                                                                            */
/*   Copyright (c) 2008 Laurens Rodriguez Oscanoa                             */
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
    StcGame *game;
    int errorCode;

    /* Create a new game */
    game = createGame();

    /* Check if we got a valid game */
    if (game) {
        /* We got a valid game, therefore start the game */
        errorCode = gameInit(game);
        if (errorCode == GAME_ERROR_NONE) {

            /* Loop until some error happens or the user quits */
            while (game->errorCode == GAME_ERROR_NONE) {
                gameUpdate(game);
            }
            /* Save error code and end game */
            errorCode = game->errorCode;
            gameEnd(game);
        }
        /* Liberate resources used by our game */
        deleteGame(game);

        /*  Return to the system */
        return errorCode;
    }
    /* If we get here the game was not created */
    return GAME_ERROR_NO_MEMORY;
}
