/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Game logic implementation.                                               */
/*                                                                            */
/*   Copyright (c) 2008 Laurens Rodriguez Oscanoa                             */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#include <stdlib.h>
#include "defines.h"
#include "game.h"

/* Set matrix elements to indicated value */
static void setMatrixCells(int *matrix, int width, int height, int value) {
    int i, j;
    for (i = 0; i < width; ++i) {
        for (j = 0; j < height; ++j) {
            *(matrix + i + (j * width)) = value;
        }
    }
}

/* Initialize tetromino cells for every tipe of tetromino */
static void setTetramino(int indexTetramino, StcTetramino *tetramino) {

    /* Initialize tetromino cells to empty cells */
    setMatrixCells(&tetramino->cells[0][0], 4, 4, EMPTY_CELL);

    /* Almost all the blocks have size 3 */
    tetramino->size = 3;

    /* Initial configuration from: http://www.tetrisconcept.com/wiki/index.php/SRS */
    switch (indexTetramino) {
    case TETROMINO_I:
        tetramino->cells[0][1] = COLOR_CYAN;
        tetramino->cells[1][1] = COLOR_CYAN;
        tetramino->cells[2][1] = COLOR_CYAN;
        tetramino->cells[3][1] = COLOR_CYAN;
        tetramino->size = 4;
        break;
    case TETROMINO_O:
        tetramino->cells[0][0] = COLOR_YELLOW;
        tetramino->cells[0][1] = COLOR_YELLOW;
        tetramino->cells[1][0] = COLOR_YELLOW;
        tetramino->cells[1][1] = COLOR_YELLOW;
        tetramino->size = 2;
        break;
    case TETROMINO_T:
        tetramino->cells[0][1] = COLOR_PURPLE;
        tetramino->cells[1][0] = COLOR_PURPLE;
        tetramino->cells[1][1] = COLOR_PURPLE;
        tetramino->cells[2][1] = COLOR_PURPLE;
        break;
    case TETROMINO_S:
        tetramino->cells[0][1] = COLOR_GREEN;
        tetramino->cells[1][0] = COLOR_GREEN;
        tetramino->cells[1][1] = COLOR_GREEN;
        tetramino->cells[2][0] = COLOR_GREEN;
        break;
    case TETROMINO_Z:
        tetramino->cells[0][0] = COLOR_RED;
        tetramino->cells[1][0] = COLOR_RED;
        tetramino->cells[1][1] = COLOR_RED;
        tetramino->cells[2][1] = COLOR_RED;
        break;
    case TETROMINO_J:
        tetramino->cells[0][0] = COLOR_BLUE;
        tetramino->cells[0][1] = COLOR_BLUE;
        tetramino->cells[1][1] = COLOR_BLUE;
        tetramino->cells[2][1] = COLOR_BLUE;
        break;
    case TETROMINO_L:
        tetramino->cells[0][1] = COLOR_ORANGE;
        tetramino->cells[1][1] = COLOR_ORANGE;
        tetramino->cells[2][0] = COLOR_ORANGE;
        tetramino->cells[2][1] = COLOR_ORANGE;
        break;
    }
    tetramino->type = indexTetramino;
}

/*  Start a new game */
static void startGame(StcGame *game) {
    int i;

    /* Initialize game data */
    game->errorCode = GAME_ERROR_NONE;
    game->systemTime = platformGetSystemTime();
    game->lastFallTime = game->systemTime;
    game->isOver = 0;
    game->isPaused = 0;
    game->showPreview = 1;
    game->events = EVENT_NONE;
    game->delay = INI_DELAY_FALL;
    /* Initialize game statistics */
    game->stats.score = 0;
    game->stats.lines = 0;
    game->stats.totalPieces = 0;
    game->stats.level = 0;
    for (i = 0; i < 7; ++i) {
        game->stats.pieces[i] = 0;
    }

    /* Initialize rand generator */
    srand(game->systemTime);

    /* Initialize game tile map */
    setMatrixCells(&game->map[0][0], BOARD_WIDTH, BOARD_HEIGHT, EMPTY_CELL);

    /* Initialize falling tetromino */
    setTetramino(rand() % 7, &game->fallingBlock);
    game->fallingBlock.x = (BOARD_WIDTH - game->fallingBlock.size) / 2;
    game->fallingBlock.y = 0;

    /* Initialize preview tetromino */
    setTetramino(rand() % 7, &game->nextBlock);
}

/* Create new game object */
StcGame *createGame() {
    /* Allocate space for our game object */
    StcGame *game = (StcGame *) malloc(sizeof(StcGame));
    return game;
}

/*
 * Initializes the game, if there are no problems returns GAME_ERROR_NONE.
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

/*
 * Rotate falling tetromino. If there are no collisions when the 
 * tetromino is rotated this modifies the tetramino's cell buffer.
 */
void rotateTetramino(StcGame *game, int clockwise) {
    int i, j;
    int rotated[4][4];  /* temporary array to hold rotated cells */

    /* If TETRAMINO_O is falling return immediately */
    if (game->fallingBlock.type == TETROMINO_O) {
        return; /* rotation doesn't require any changes */
    }

    /* Initialize rotated cells to blank */
    setMatrixCells(&rotated[0][0], 4, 4, EMPTY_CELL);

    /* Copy rotated cells to the temporary array */
    for (i = 0; i < game->fallingBlock.size; ++i) {
        for (j = 0; j < game->fallingBlock.size; ++j) {
            if (clockwise) {
                rotated[game->fallingBlock.size - j - 1][i] = game->fallingBlock.cells[i][j];
            } else {
                rotated[j][game->fallingBlock.size - i - 1] = game->fallingBlock.cells[i][j];
            }
        }
    }
    /* Check collision of the temporary array */
    for (i = 0; i < game->fallingBlock.size; ++i) {
        for (j = 0; j < game->fallingBlock.size; ++j) {
            if (rotated[i][j] != EMPTY_CELL) {
                /* Check collision with left, right or bottom borders of the map */
                if ((game->fallingBlock.x + i < 0) || (game->fallingBlock.x + i >= BOARD_WIDTH)
                        || (game->fallingBlock.y + j >= BOARD_HEIGHT)) {
                    return; /* there was collision therefore return */
                }
                /* Check collision with existing cells in the map */
                if (game->map[i + game->fallingBlock.x][j + game->fallingBlock.y] != EMPTY_CELL) {
                    return; /* there was collision therefore return */
                }
            }
        }
    }
    /* There are no collisions, replace tetramino cells with rotated cells */
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            game->fallingBlock.cells[i][j] = rotated[i][j];
        }
    }
}

/*
 * Check if tetromino will collide with something if it is moved in the requested direction.
 * If there are collisions returns 1 else returns 0.
 */
static int checkCollision(StcGame *game, int dx, int dy) {
    int newx, newy, i, j;

    newx = game->fallingBlock.x + dx;
    newy = game->fallingBlock.y + dy;

    for (i = 0; i < game->fallingBlock.size; ++i) {
        for (j = 0; j < game->fallingBlock.size; ++j) {
            if (game->fallingBlock.cells[i][j] != EMPTY_CELL) {
                /* Check the tetramino would be inside the left, right and bottom borders */
                if ((newx + i < 0) || (newx + i >= BOARD_WIDTH)
                    || (newy + j >= BOARD_HEIGHT)) {
                    return 1;
                }
                /* Check the tetromino won't collide with existing cells in the map */
                if (game->map[newx + i][newy + j] != EMPTY_CELL) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

/* Game scoring: http://www.tetrisconcept.com/wiki/index.php/Scoring */
static void onFilledRows(StcGame *game, int filledRows) {
    /* Update total number of filled rows */
    game->stats.lines += filledRows;

    /* Increase score accordingly to the number of filled rows */
    switch (filledRows) {
    case 1:
        game->stats.score += (SCORE_1_FILLED_ROW * (game->stats.level + 1));
        break;
    case 2:
        game->stats.score += (SCORE_2_FILLED_ROW * (game->stats.level + 1));
        break;
    case 3:
        game->stats.score += (SCORE_3_FILLED_ROW * (game->stats.level + 1));
        break;
    case 4:
        game->stats.score += (SCORE_4_FILLED_ROW * (game->stats.level + 1));
        break;
    default:
        game->errorCode = GAME_ERROR_ASSERT;    /* This can't happen */
    }
    /* Check if we need to update level */
    if (game->stats.lines >= FILLED_ROWS_FOR_LEVEL_UP * (game->stats.level + 1)) {
        game->stats.level++;

        /* Increase speed for falling tetrominoes */
        game->delay *= DELAY_FACTOR_FOR_LEVEL_UP;
    }
}

/*
 * Move tetramino in direction especified by (x, y) (in tile units)
 * This function detects if there are filled rows or if the move 
 * lands a falling tetromino, also checks for game over condition.
 */
static void moveTetramino(StcGame *game, int x, int y) {
    int i, j, hasFullRow, numFilledRows;
    
    /* Check if the move would create a collision */
    if (checkCollision(game, x, y)) {
        /* In case of collision check if move was downwards (y == 1) */
        if (y == 1) {
            /* Check if collision occur when the falling 
             * tetromino is in the 1st or 2nd row */
            if (game->fallingBlock.y <= 1) {
                game->isOver = 1;   /* if this happens the game is over */
            }
            else {
                /* The falling tetromino has reached the bottom, 
                 * so we copy their cells to the board map */
                for (i = 0; i < game->fallingBlock.size; ++i) {
                    for (j = 0; j < game->fallingBlock.size; ++j) {
                        if (game->fallingBlock.cells[i][j] != EMPTY_CELL) {
                            game->map[game->fallingBlock.x + i][game->fallingBlock.y + j] 
                                    = game->fallingBlock.cells[i][j];
                        }
                    }
                }

                /* Check if the landing tetromino has created full rows */
                numFilledRows = 0;
                for (j = 1; j < BOARD_HEIGHT; ++j) {
                    hasFullRow = 1;
                    for (i = 0; i < BOARD_WIDTH; ++i) {
                        if (game->map[i][j] == EMPTY_CELL) {
                            hasFullRow = 0;
                            break;
                        }
                    }
                    /* If we found a full row we need to remove that row from the map
                     * we do that by just moving all the above rows one row below */
                    if (hasFullRow) {
                        for (x = 0; x < BOARD_WIDTH; ++x) {
                            for (y = j; y > 0; --y) {
                                game->map[x][y] = game->map[x][y - 1];
                            }
                        }
                        numFilledRows++;    /* increase filled row counter */
                    }
                }

                /* Update game statistics */
                if (numFilledRows) {
                    onFilledRows(game, numFilledRows);
                }
                game->stats.totalPieces++;
                game->stats.pieces[game->fallingBlock.type]++;
                
                /* Use preview tetromino as falling tetromino.
                 * Copy preview tetramino for falling tetramino */
                for (i = 0; i < 4; ++i) {
                    for (j = 0; j < 4; ++j) {
                        game->fallingBlock.cells[i][j] = game->nextBlock.cells[i][j];
                    }
                }
                game->fallingBlock.size = game->nextBlock.size;
                game->fallingBlock.type = game->nextBlock.type;

                /* Reset position */
                game->fallingBlock.y = 0;
                game->fallingBlock.x = (BOARD_WIDTH - game->fallingBlock.size) / 2;

                /* Create next preview tetromino */
                setTetramino(rand() % 7, &game->nextBlock);
            }
        }
    }
    else {
        /* There are no collisions, just move the tetramino */
        game->fallingBlock.x += x;
        game->fallingBlock.y += y;
    }
}

/* Hard drop */
static void dropTetramino(StcGame *game) {
   int y;
   y = 1;
   /* Calculate number of cells to drop */
   while (!checkCollision(game, 0, y)) {
       y++;
   }
   moveTetramino(game, 0, y - 1);
}

/*
 * Main function game called every frame
 */
void gameUpdate(StcGame *game) {
    long sysTime;

    /* Read user input */
    platformReadInput(game);

    /* Update game state */
    if (game->isOver) {
        if (game->events & EVENT_RESTART) {
            game->isOver = 0;
            startGame(game);
        }
    }
    else {
        sysTime = platformGetSystemTime();
        
        /* Always handle pause event */
        if (game->events & EVENT_PAUSE) {
            game->isPaused = !game->isPaused;
            game->events = EVENT_NONE;
        }

        /* Check if the game is paused */
        if (game->isPaused) {
            /* We achieve the effect of pausing the game
             * adding the last frame duration to lastFallTime */
            game->lastFallTime += (sysTime - game->systemTime);
        }
        else {
            if (game->events != EVENT_NONE) {
                if (game->events & EVENT_SHOW_NEXT) {
                    game->showPreview = !game->showPreview;
                }
                if (game->events & EVENT_DROP) {
                    dropTetramino(game);
                }
                if (game->events & EVENT_ROTATE_CW) {
                    rotateTetramino(game, 1);
                }
                if (game->events & EVENT_MOVE_RIGHT) {
                    moveTetramino(game, 1, 0);
                }
                else if (game->events & EVENT_MOVE_LEFT) {
                    moveTetramino(game, -1, 0);
                }
                if (game->events & EVENT_MOVE_DOWN) {
                    moveTetramino(game, 0, 1);
                }
                game->events = EVENT_NONE;
            }
            /* Check if it's time to move downwards the falling tetromino */
            if (sysTime - game->lastFallTime >= game->delay) {
                moveTetramino(game, 0, 1);
                game->lastFallTime = sysTime;
            }
        }
        game->systemTime = sysTime;
    }
    /* Draw game state */
    platformRenderGame(game);
}

