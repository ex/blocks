/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Game logic implementation.                                               */
/*                                                                            */
/*   Copyright (c) 2012 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#include "game.h"
#include <stdlib.h>

/* Here we define the game private internal data */
struct StcGamePrivate {

    /* Game events are stored in bits in this variable.
     * It must be cleared to EVENT_NONE after being used */
    int events;

    int  isOver;       /* 1 if the game is over, 0 otherwise      */
    long systemTime;   /* system time in milliseconds             */
    int  fallingDelay; /* delay time for falling tetrominoes      */
    long lastFallTime; /* last time the falling tetromino dropped */

    /* For delayed autoshift: http://tetris.wikia.com/wiki/DAS */
    int delayLeft;
    int delayRight;
    int delayDown;
#ifdef STC_AUTO_ROTATION
    int delayRotation;
#endif
};

static void onTetrominoMoved(StcGame *game);

/* Set matrix elements to indicated value */
static void setMatrixCells(int *matrix, int width, int height, int value) {
    int i, j;
    for (i = 0; i < width; ++i) {
        for (j = 0; j < height; ++j) {
            *(matrix + i + (j * width)) = value;
        }
    }
}

/* Initialize tetromino cells for every type of tetromino */
static void setTetromino(int indexTetromino, StcTetromino *tetromino) {

    /* Initialize tetromino cells to empty cells */
    setMatrixCells(&tetromino->cells[0][0], TETROMINO_SIZE, TETROMINO_SIZE, EMPTY_CELL);

    /* Almost all the blocks have size 3 */
    tetromino->size = TETROMINO_SIZE - 1;

    /* Initial configuration from: http://tetris.wikia.com/wiki/SRS */
    switch (indexTetromino) {
    case TETROMINO_I:
        tetromino->cells[0][1] = COLOR_CYAN;
        tetromino->cells[1][1] = COLOR_CYAN;
        tetromino->cells[2][1] = COLOR_CYAN;
        tetromino->cells[3][1] = COLOR_CYAN;
        tetromino->size = TETROMINO_SIZE;
        break;
    case TETROMINO_O:
        tetromino->cells[0][0] = COLOR_YELLOW;
        tetromino->cells[0][1] = COLOR_YELLOW;
        tetromino->cells[1][0] = COLOR_YELLOW;
        tetromino->cells[1][1] = COLOR_YELLOW;
        tetromino->size = TETROMINO_SIZE - 2;
        break;
    case TETROMINO_T:
        tetromino->cells[0][1] = COLOR_PURPLE;
        tetromino->cells[1][0] = COLOR_PURPLE;
        tetromino->cells[1][1] = COLOR_PURPLE;
        tetromino->cells[2][1] = COLOR_PURPLE;
        break;
    case TETROMINO_S:
        tetromino->cells[0][1] = COLOR_GREEN;
        tetromino->cells[1][0] = COLOR_GREEN;
        tetromino->cells[1][1] = COLOR_GREEN;
        tetromino->cells[2][0] = COLOR_GREEN;
        break;
    case TETROMINO_Z:
        tetromino->cells[0][0] = COLOR_RED;
        tetromino->cells[1][0] = COLOR_RED;
        tetromino->cells[1][1] = COLOR_RED;
        tetromino->cells[2][1] = COLOR_RED;
        break;
    case TETROMINO_J:
        tetromino->cells[0][0] = COLOR_BLUE;
        tetromino->cells[0][1] = COLOR_BLUE;
        tetromino->cells[1][1] = COLOR_BLUE;
        tetromino->cells[2][1] = COLOR_BLUE;
        break;
    case TETROMINO_L:
        tetromino->cells[0][1] = COLOR_ORANGE;
        tetromino->cells[1][1] = COLOR_ORANGE;
        tetromino->cells[2][0] = COLOR_ORANGE;
        tetromino->cells[2][1] = COLOR_ORANGE;
        break;
    }
    tetromino->type = indexTetromino;
}

/* Start a new game */
static void startGame(StcGame *game) {
    int i;

    /* Initialize game data */
    game->errorCode = ERROR_NONE;
    game->data->systemTime = platformGetSystemTime();
    game->data->lastFallTime = game->data->systemTime;
    game->data->isOver = 0;
    game->isPaused = 0;
    game->showPreview = 1;
    game->data->events = EVENT_NONE;
    game->data->fallingDelay = STC_INIT_DELAY_FALL;
#ifdef STC_SHOW_GHOST_PIECE
    game->showShadow = 1;
#endif

    /* Initialize game statistics */
    game->stats.score = 0;
    game->stats.lines = 0;
    game->stats.totalPieces = 0;
    game->stats.level = 0;
    for (i = 0; i < TETROMINO_TYPES; ++i) {
        game->stats.pieces[i] = 0;
    }

    /* Initialize game tile map */
    setMatrixCells(&game->map[0][0], BOARD_TILEMAP_WIDTH, BOARD_TILEMAP_HEIGHT, EMPTY_CELL);

    /* Initialize falling tetromino */
    setTetromino(platformRandom() % TETROMINO_TYPES, &game->fallingBlock);
    game->fallingBlock.x = (BOARD_TILEMAP_WIDTH - game->fallingBlock.size) / 2;
    game->fallingBlock.y = 0;

    /* Initialize preview tetromino */
    setTetromino(platformRandom() % TETROMINO_TYPES, &game->nextBlock);

    /* Initialize events */
    onTetrominoMoved(game);

    /* Initialize delayed autoshift */
    game->data->delayLeft = -1;
    game->data->delayRight = -1;
    game->data->delayDown = -1;
#ifdef STC_AUTO_ROTATION
    game->data->delayRotation = -1;
#endif
}

/*
 * Initialize the game. The error code (if any) is saved in [game->errorCode].
 */
void gameInit(StcGame *game) {
    /* Allocate space for our game internal data */
    game->data = (StcGamePrivate *) malloc(sizeof(StcGamePrivate));
    if (game->data == NULL) {
        game->errorCode = ERROR_NO_MEMORY;
    }
    else {
        /* Initialize platform */
        game->errorCode = platformInit(game);

        if (game->errorCode != ERROR_NONE) {
            /* If platform was correctly initialized, start the game */
            startGame(game);
        }
    }
}

/* Free used resources */
void gameEnd(StcGame *game) {
    if (game->data != NULL) {
        free(game->data);
        game->data = NULL;
    }
    platformEnd(game);
}

/*
 * Rotate falling tetromino. If there are no collisions when the
 * tetromino is rotated this modifies the tetromino's cell buffer.
 */
void rotateTetromino(StcGame *game, int clockwise) {
    int i, j;
#ifdef STC_WALL_KICK_ENABLED
    int wallDisplace;
#endif
    int rotated[TETROMINO_SIZE][TETROMINO_SIZE];  /* temporary array to hold rotated cells */

    /* If TETROMINO_O is falling return immediately */
    if (game->fallingBlock.type == TETROMINO_O) {
        return; /* rotation doesn't require any changes */
    }

    /* Initialize rotated cells to blank */
    setMatrixCells(&rotated[0][0], TETROMINO_SIZE, TETROMINO_SIZE, EMPTY_CELL);

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
#ifdef STC_WALL_KICK_ENABLED
    wallDisplace = 0;

    /* Check collision with left wall */
    if (game->fallingBlock.x < 0) {
        for (i = 0; (wallDisplace == 0) && (i < -game->fallingBlock.x); ++i) {
            for (j = 0; j < game->fallingBlock.size; ++j) {
                if (rotated[i][j] != EMPTY_CELL) {
                    wallDisplace = i - game->fallingBlock.x;
                    break;
                }
            }
        }
    }
    /* Or check collision with right wall */
    else if (game->fallingBlock.x > BOARD_TILEMAP_WIDTH - game->fallingBlock.size) {
        i = game->fallingBlock.size - 1;
        for (; (wallDisplace == 0) && (i >= BOARD_TILEMAP_WIDTH - game->fallingBlock.x); --i) {
            for (j = 0; j < game->fallingBlock.size; ++j) {
                if (rotated[i][j] != EMPTY_CELL) {
                    wallDisplace = -game->fallingBlock.x - i + BOARD_TILEMAP_WIDTH - 1;
                    break;
                }
            }
        }
    }

    /* Check collision with board floor and other cells on board */
    for (i = 0; i < game->fallingBlock.size; ++i) {
        for (j = 0; j < game->fallingBlock.size; ++j) {
            if (rotated[i][j] != EMPTY_CELL) {
                /* Check collision with bottom border of the map */
                if (game->fallingBlock.y + j >= BOARD_TILEMAP_HEIGHT) {
                    return; /* there was collision therefore return */
                }
                /* Check collision with existing cells in the map */
                if (game->map[i + game->fallingBlock.x + wallDisplace][j + game->fallingBlock.y] != EMPTY_CELL) {
                    return; /* there was collision therefore return */
                }
            }
        }
    }
    /* Move the falling piece if there was wall collision and it's a legal move */
    if (wallDisplace != 0) {
        game->fallingBlock.x += wallDisplace;
    }
#else
    /* Check collision of the temporary array */
    for (i = 0; i < game->fallingBlock.size; ++i) {
        for (j = 0; j < game->fallingBlock.size; ++j) {
            if (rotated[i][j] != EMPTY_CELL) {
                /* Check collision with left, right or bottom borders of the map */
                if ((game->fallingBlock.x + i < 0) || (game->fallingBlock.x + i >= BOARD_TILEMAP_WIDTH)
                        || (game->fallingBlock.y + j >= BOARD_TILEMAP_HEIGHT)) {
                    return; /* there was collision therefore return */
                }
                /* Check collision with existing cells in the map */
                if (game->map[i + game->fallingBlock.x][j + game->fallingBlock.y] != EMPTY_CELL) {
                    return; /* there was collision therefore return */
                }
            }
        }
    }
#endif
    /* There are no collisions, replace tetromino cells with rotated cells */
    for (i = 0; i < TETROMINO_SIZE; ++i) {
        for (j = 0; j < TETROMINO_SIZE; ++j) {
            game->fallingBlock.cells[i][j] = rotated[i][j];
        }
    }
    onTetrominoMoved(game);
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
                /* Check the tetromino would be inside the left, right and bottom borders */
                if ((newx + i < 0) || (newx + i >= BOARD_TILEMAP_WIDTH)
                    || (newy + j >= BOARD_TILEMAP_HEIGHT)) {
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

/* Game scoring: http://tetris.wikia.com/wiki/Scoring */
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
        /* This shouldn't happen, but if happens kill the game */
        game->errorCode = ERROR_ASSERT;
    }
    /* Check if we need to update level */
    if (game->stats.lines >= FILLED_ROWS_FOR_LEVEL_UP * (game->stats.level + 1)) {
        game->stats.level++;

        /* Increase speed for falling tetrominoes */
        game->data->fallingDelay = (int)(DELAY_FACTOR_FOR_LEVEL_UP * game->data->fallingDelay 
                                         / DELAY_DIVISOR_FOR_LEVEL_UP);
    }
}

/*
 * Move tetromino in the direction specified by (x, y) (in tile units)
 * This function detects if there are filled rows or if the move
 * lands a falling tetromino, also checks for game over condition.
 */
static void moveTetromino(StcGame *game, int x, int y) {
    int i, j, hasFullRow, numFilledRows;

    /* Check if the move would create a collision */
    if (checkCollision(game, x, y)) {
        /* In case of collision check if move was downwards (y == 1) */
        if (y == 1) {
            /* Check if collision occurs when the falling
             * tetromino is on the 1st or 2nd row */
            if (game->fallingBlock.y <= 1) {
                game->data->isOver = 1;   /* if this happens the game is over */
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
                for (j = 1; j < BOARD_TILEMAP_HEIGHT; ++j) {
                    hasFullRow = 1;
                    for (i = 0; i < BOARD_TILEMAP_WIDTH; ++i) {
                        if (game->map[i][j] == EMPTY_CELL) {
                            hasFullRow = 0;
                            break;
                        }
                    }
                    /* If we found a full row we need to remove that row from the map
                     * we do that by just moving all the above rows one row below */
                    if (hasFullRow != 0) {
                        for (x = 0; x < BOARD_TILEMAP_WIDTH; ++x) {
                            for (y = j; y > 0; --y) {
                                game->map[x][y] = game->map[x][y - 1];
                            }
                        }
                        numFilledRows++;    /* increase filled row counter */
                    }
                }

                /* Update game statistics */
                if (numFilledRows > 0) {
                    onFilledRows(game, numFilledRows);
                }
                game->stats.totalPieces++;
                game->stats.pieces[game->fallingBlock.type]++;

                /* Use preview tetromino as falling tetromino.
                 * Copy preview tetromino for falling tetromino */
                for (i = 0; i < TETROMINO_SIZE; ++i) {
                    for (j = 0; j < TETROMINO_SIZE; ++j) {
                        game->fallingBlock.cells[i][j] = game->nextBlock.cells[i][j];
                    }
                }
                game->fallingBlock.size = game->nextBlock.size;
                game->fallingBlock.type = game->nextBlock.type;

                /* Reset position */
                game->fallingBlock.y = 0;
                game->fallingBlock.x = (BOARD_TILEMAP_WIDTH - game->fallingBlock.size) / 2;
                onTetrominoMoved(game);

                /* Create next preview tetromino */
                setTetromino(platformRandom() % TETROMINO_TYPES, &game->nextBlock);
            }
        }
    }
    else {
        /* There are no collisions, just move the tetromino */
        game->fallingBlock.x += x;
        game->fallingBlock.y += y;
    }
    onTetrominoMoved(game);
}

/* Hard drop */
static void dropTetromino(StcGame *game) {
#ifdef STC_SHOW_GHOST_PIECE
    /* Shadow has already calculated the landing position. */
    game->fallingBlock.y += game->shadowGap;

    /* Force lock. */
    moveTetromino(game, 0, 1); 

    /* Update score */
    if (game->showShadow) {
        game->stats.score += (long)(SCORE_2_FILLED_ROW * (game->stats.level + 1) 
                                    / SCORE_DROP_WITH_SHADOW_DIVISOR);
    }
    else {
        game->stats.score += (long)(SCORE_2_FILLED_ROW * (game->stats.level + 1) 
                                    / SCORE_DROP_DIVISOR);
    }
#else
    int y = 0;
    /* Calculate number of cells to drop */
    while (!checkCollision(game, 0, ++y));
    moveTetromino(game, 0, y - 1);
    moveTetromino(game, 0, 1); /* Force lock */

    /* Update score */
    game->stats.score += (long)(SCORE_2_FILLED_ROW * (game->stats.level + 1) 
                                / SCORE_DROP_DIVISOR);
#endif
}

/*
 * Main function game called every frame
 */
void gameUpdate(StcGame *game) {
    long currentTime;
    int timeDelta;

    /* Read user input */
    platformProcessEvents(game);

    /* Update game state */
    if (game->data->isOver != 0) {
        if ((game->data->events & EVENT_RESTART) != 0) {
            game->data->isOver = 0;
            startGame(game);
        }
    }
    else {
        currentTime = platformGetSystemTime();

        /* Process delayed autoshift */
        timeDelta = (int)(currentTime - game->data->systemTime);
        if (game->data->delayDown > 0) {
            game->data->delayDown -= timeDelta;
            if (game->data->delayDown <= 0) {
                game->data->delayDown = DAS_MOVE_TIMER;
                game->data->events |= EVENT_MOVE_DOWN;
            }
        }
        if (game->data->delayLeft > 0) {
            game->data->delayLeft -= timeDelta;
            if (game->data->delayLeft <= 0) {
                game->data->delayLeft = DAS_MOVE_TIMER;
                game->data->events |= EVENT_MOVE_LEFT;
            }
        }
        else if (game->data->delayRight > 0) {
            game->data->delayRight -= timeDelta;
            if (game->data->delayRight <= 0) {
                game->data->delayRight = DAS_MOVE_TIMER;
                game->data->events |= EVENT_MOVE_RIGHT;
            }
        }
    #ifdef STC_AUTO_ROTATION
        if (game->data->delayRotation > 0) {
            game->data->delayRotation -= timeDelta;
            if (game->data->delayRotation <= 0) {
                game->data->delayRotation = ROTATION_AUTOREPEAT_TIMER;
                game->data->events |= EVENT_ROTATE_CW;
            }
        }
    #endif /* STC_AUTO_ROTATION */

        /* Always handle pause event */
        if ((game->data->events & EVENT_PAUSE) != 0) {
            game->isPaused = !game->isPaused;
            game->data->events = EVENT_NONE;
        }

        /* Check if the game is paused */
        if (game->isPaused != 0) {
            /* We achieve the effect of pausing the game
             * adding the last frame duration to lastFallTime */
            game->data->lastFallTime += (currentTime - game->data->systemTime);
        }
        else {
            if ((game->data->events != EVENT_NONE) != 0) {
                if (game->data->events & EVENT_SHOW_NEXT) {
                    game->showPreview = !game->showPreview;
                    game->stateChanged = 1;
                }
#ifdef STC_SHOW_GHOST_PIECE
                if ((game->data->events & EVENT_SHOW_SHADOW) != 0) {
                    game->showShadow = !game->showShadow;
                    game->stateChanged = 1;
                }
#endif
                if ((game->data->events & EVENT_DROP) != 0) {
                    dropTetromino(game);
                }
                if ((game->data->events & EVENT_ROTATE_CW) != 0) {
                    rotateTetromino(game, 1);
                }
                if ((game->data->events & EVENT_MOVE_RIGHT) != 0) {
                    moveTetromino(game, 1, 0);
                }
                else if ((game->data->events & EVENT_MOVE_LEFT) != 0) {
                    moveTetromino(game, -1, 0);
                }
                if ((game->data->events & EVENT_MOVE_DOWN) != 0) {
                    /* Update score if the user accelerates downfall */
                    game->stats.score += (long)(SCORE_2_FILLED_ROW * (game->stats.level + 1) 
                                                / SCORE_MOVE_DOWN_DIVISOR);

                    moveTetromino(game, 0, 1);
                }
                game->data->events = EVENT_NONE;
            }
            /* Check if it's time to move downwards the falling tetromino */
            if (currentTime - game->data->lastFallTime >= game->data->fallingDelay) {
                moveTetromino(game, 0, 1);
                game->data->lastFallTime = currentTime;
            }
        }
        /* Save current time for next game update */
        game->data->systemTime = currentTime;
    }
    /* Draw game state */
    platformRenderGame(game);
}

/* This event is called when the falling tetromino is moved */
static void onTetrominoMoved(StcGame *game) {
#ifdef STC_SHOW_GHOST_PIECE
    int y = 0;
    /* Calculate number of cells where shadow tetromino would be */
    while (!checkCollision(game, 0, ++y));
    game->shadowGap = y - 1;
#endif
    game->stateChanged = 1;
}

/* Process a key down event */
void gameOnEventStart(StcGame *game, int command) {
    switch (command) {
    case EVENT_QUIT:
        game->errorCode = ERROR_PLAYER_QUITS;
        break;
    case EVENT_MOVE_DOWN:
        game->data->events |= EVENT_MOVE_DOWN;
        game->data->delayDown = DAS_DELAY_TIMER;
        break;
    case EVENT_ROTATE_CW:
        game->data->events |= EVENT_ROTATE_CW;
#ifdef STC_AUTO_ROTATION
        game->data->delayRotation = ROTATION_AUTOREPEAT_DELAY;
#endif
        break;
    case EVENT_MOVE_LEFT:
        game->data->events |= EVENT_MOVE_LEFT;
        game->data->delayLeft = DAS_DELAY_TIMER;
        break;
    case EVENT_MOVE_RIGHT:
        game->data->events |= EVENT_MOVE_RIGHT;
        game->data->delayRight = DAS_DELAY_TIMER;
        break;
    case EVENT_DROP:        /* Fall through */
    case EVENT_RESTART:     /* Fall through */
    case EVENT_PAUSE:       /* Fall through */
    case EVENT_SHOW_NEXT:   /* Fall through */
    case EVENT_SHOW_SHADOW:
        game->data->events |= command;
        break;
    }
}

/* Process a key up event */
void gameOnEventEnd(StcGame *game, int command) {
    switch (command) {
    case EVENT_MOVE_DOWN:
        game->data->delayDown = -1;
        break;
    case EVENT_MOVE_LEFT:
        game->data->delayLeft = -1;
        break;
    case EVENT_MOVE_RIGHT:
        game->data->delayRight = -1;
        break;
#ifdef STC_AUTO_ROTATION
    case EVENT_ROTATE_CW:
        game->data->delayRotation = -1;
        break;
#endif /* STC_AUTO_ROTATION */
    }
}

