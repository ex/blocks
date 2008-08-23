/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Game logic implementation.                                               */
/*                                                                            */
/*   Copyright (C) 2008 Laurens Rodriguez Oscanoa                             */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#include <stdlib.h>
#include "defines.h"
#include "game.h"

/* Initialize tetromino cells for every tipe of tetromino */
static void setTetramino(int indexTetramino, StcTetramino *tetramino) {
    int i,j;

    /* Initialize tetromino cells to blank */
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            tetramino->cells[i][j] = EMPTY_CELL;

    /* Almost all the blocks have size 3 */
    tetramino->size = 3;

    /* Colors from: http://www.tetrisconcept.com/wiki/index.php/Tetromino */
    switch (indexTetramino) {
    case TETROMINO_I:
        tetramino->cells[0][1]=COLOR_CYAN;
        tetramino->cells[1][1]=COLOR_CYAN;
        tetramino->cells[2][1]=COLOR_CYAN;
        tetramino->cells[3][1]=COLOR_CYAN;
        tetramino->size = 4;
        break;
    case TETROMINO_O:
        tetramino->cells[1][1]=COLOR_YELLOW;
        tetramino->cells[1][2]=COLOR_YELLOW;
        tetramino->cells[2][1]=COLOR_YELLOW;
        tetramino->cells[2][2]=COLOR_YELLOW;
        break;
    case TETROMINO_T:
        tetramino->cells[1][1]=COLOR_PURPLE;
        tetramino->cells[0][2]=COLOR_PURPLE;
        tetramino->cells[1][2]=COLOR_PURPLE;
        tetramino->cells[2][2]=COLOR_PURPLE;
        break;
    case TETROMINO_S:
        tetramino->cells[2][1]=COLOR_GREEN;
        tetramino->cells[1][1]=COLOR_GREEN;
        tetramino->cells[1][2]=COLOR_GREEN;
        tetramino->cells[0][2]=COLOR_GREEN;
        break;
    case TETROMINO_Z:
        tetramino->cells[0][1]=COLOR_RED;
        tetramino->cells[1][1]=COLOR_RED;
        tetramino->cells[1][2]=COLOR_RED;
        tetramino->cells[2][2]=COLOR_RED;
        break;
    case TETROMINO_J:
        tetramino->cells[2][1]=COLOR_BLUE;
        tetramino->cells[1][1]=COLOR_BLUE;
        tetramino->cells[1][2]=COLOR_BLUE;
        tetramino->cells[1][3]=COLOR_BLUE;
        break;
    case TETROMINO_L:
        tetramino->cells[1][1]=COLOR_ORANGE;
        tetramino->cells[2][1]=COLOR_ORANGE;
        tetramino->cells[2][2]=COLOR_ORANGE;
        tetramino->cells[2][3]=COLOR_ORANGE;
        break;
    }
}

/* Create a new falling tetramino */
static void createTetromino(StcGame *game) {
    int i,j;

    /* Initialize position of falling tetromino */
    game->fallingBlock.x = (BOARD_WIDTH / 2) - 2;
    game->fallingBlock.y =- 1;

    /* Check if the game has started */
    if (game->hasStarted == 0)  {
        /* If not generate the falling tetromino */
        game->hasStarted = 1;
        setTetramino(rand() % 7, &game->fallingBlock);
    }
    else {
        for (i = 0; i < 4; ++i)
            for (j = 0; j < 4; ++j)
                game->fallingBlock.cells[i][j] = game->nextBlock.cells[i][j];
    }

    /* Initialize position for preview tetromino */
    game->nextBlock.x = NEXT_TETROMINO_X;
    game->nextBlock.y = NEXT_TETROMINO_Y;
    setTetramino(rand() % 7, &game->nextBlock);
}

/*  Starts a new game */
static void startGame(StcGame *game) {
    int x, y;

    game->systemTime = getSystemTime();
    game->hasStarted = 0;

    /* Initialize rand generator */
    srand(game->systemTime);

    /* Initialize game tile map */
    for (x = 0; x < BOARD_WIDTH; ++x) {
        for (y = 0; y < BOARD_HEIGHT + 1; ++y) {
            game->map[x][y] = EMPTY_CELL;
        }
    }
    createTetromino(game);
}

void gameUpdate(StcGame *game) {
    long sysTime;

    /* Read user input */
    gameReadInput(game);

    /* Update logic */
    sysTime = getSystemTime();
    if (sysTime - game->systemTime >= INI_DELAY_FALL) {
        moveTetramino(game, 0, 1);
        game->systemTime = sysTime;
    }

    /* Draw game state */
    gameRender(game);
}

StcGame *createGame() {
    /* Allocate space for our game object */
    StcGame *game = (StcGame *) malloc(sizeof(StcGame));

    if (game != NULL) {
        /* Setup data members */
        game->errorCode = 0;
        game->hasStarted = 0;
    }
    return game;
}

/*
 * Initialices the game, if there are no problems returns GAME_ERROR_NONE.
 */
int gameInit(StcGame *game) {
    int errorCode;

    errorCode = platformInit(game);
    if (errorCode == GAME_ERROR_NONE) {
        startGame(game);
        return GAME_ERROR_NONE;
    }
    return errorCode;
};

void gameEnd(StcGame *game) {
    /* Free platform resources */
    platformEnd(game);
}

void deleteGame(StcGame *game) {
    free(game);
}

void rotateTetramino(StcGame *game, int clockwise) {
    int i, j, temp[4][4];

    /* Copy & rotate the piece to the temporary array */
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            if (clockwise) {
                temp[3-j][i] = game->fallingBlock.cells[i][j];
            } else {
                temp[j][3-i] = game->fallingBlock.cells[i][j];
            }

    /* Check collision of the temporary array with map borders */
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            if (temp[i][j] != EMPTY_CELL)
                if (game->fallingBlock.x + i < 0 || game->fallingBlock.x + i > BOARD_WIDTH - 1 ||
                        game->fallingBlock.y + j < 0 || game->fallingBlock.y + j > BOARD_HEIGHT - 1)
                    return;

    /* Check collision of the temporary array with the blocks on the map */
    for (i = 0; i < BOARD_WIDTH; ++i)
        for (j = 0; j < BOARD_HEIGHT; ++j)
            if (i >= game->fallingBlock.x && i < game->fallingBlock.x + 4)
                if (j >= game->fallingBlock.y && j < game->fallingBlock.y +4)
                    if (game->map[i][j] != EMPTY_CELL)
                        if (temp[i - game->fallingBlock.x][j - game->fallingBlock.y] != EMPTY_CELL)
                            return;

    /* Successful!  copy the rotated temporary array to the original piece */
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            game->fallingBlock.cells[i][j] = temp[i][j];
}

int checkCollision(StcGame *game, int nx, int ny) {
    int newx, newy, i, j;

    newx = game->fallingBlock.x + nx;
    newy = game->fallingBlock.y + ny;

    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            if (game->fallingBlock.cells[i][j] != EMPTY_CELL)
                if (newx + i < 0 || newx + i > BOARD_WIDTH - 1 ||
                        newy + j < 0 || newy + j > BOARD_HEIGHT - 1)
                    return 1;

    for (i = 0; i < BOARD_WIDTH; ++i)
        for (j = 0; j < BOARD_HEIGHT; ++j)
            if (i >= newx && i < newx + 4)
                if (j >= newy && j < newy +4)
                    if (game->map[i][j] != EMPTY_CELL)
                        if (game->fallingBlock.cells[i - newx][j - newy] != EMPTY_CELL)
                            return 1;
    return 0;
}

void moveTetramino(StcGame *game, int x, int y) {
    int i, j, filled, killblock = 0;

    if (checkCollision(game, x, y)) {
        if (y == 1) {
            if (game->fallingBlock.y < 1) {
                startGame(game); /* game over */
            } else {
                 /* New block time! add this one to the list! */
                for (i = 0; i < 4; ++i)
                    for (j = 0; j < 4; ++j)
                        if (game->fallingBlock.cells[i][j] != EMPTY_CELL)
                            game->map[game->fallingBlock.x + i][game->fallingBlock.y + j] 
                                    = game->fallingBlock.cells[i][j];

                /* Check for cleared row */
                for (j = 0; j < BOARD_HEIGHT; ++j) {
                    filled = 1;
                    for (i = 0; i < BOARD_WIDTH; ++i)
                        if (game->map[i][j] == EMPTY_CELL)
                            filled = 0;

                    if (filled) {
                        /* Remove row j */
                        for (x = 0; x < BOARD_WIDTH; ++x)
                            for (y = j; y > 0; --y)
                                game->map[x][y] = game->map[x][y - 1];

                        killblock = 1;
                    }
                }

                if (killblock) {
                    for (i = 0; i < 4; ++i)
                        for (j = 0; j < 4; ++j)
                            game->fallingBlock.cells[i][j] = EMPTY_CELL;
                }
                createTetromino(game);
            }
        }
    }
    else {
        game->fallingBlock.x += x;
        game->fallingBlock.y += y;
    }
}

void dropTetramino(StcGame *game) {
   int y = 1;
   while (!checkCollision(game, 0, y)) {
       y++;
   }
   moveTetramino(game, 0, y-1);
}
