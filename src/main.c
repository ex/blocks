/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   A very simple tetris clone.                                              */
/*                                                                            */
/*   Copyright (C) 2008 Laurens Rodriguez Oscanoa                             */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#include "game.h"

int main(int argc, char *argv[]) {
    /* Create a game */
    StcGame *game = createGame();

    /* Check if we got a valid game */
    if (game) {

        /* If so, start the game */
        if (gameInit(game) == GAME_ERROR_NONE) {

            /* Loop until some error happens or the user quits. */
            while (game->errorCode == GAME_ERROR_NONE) {
                gameUpdate(game);
            }
            gameEnd(game);
        }
        /* Liberate resources used by our game */
        deleteGame(game);
    }
    /* Return to the system */
    return 0;
}
