/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Game logic implementation.                                               */
/*                                                                            */
/*   Copyright (c) 2010 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#include "game.hpp"
#include <stdlib.h>

/* Set matrix elements to indicated value */
void StcGame::setMatrixCells(int *matrix, int width, int height, int value) {
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            *(matrix + i + (j * width)) = value;
        }
    }
}

/* Initialize tetromino cells for every type of tetromino */
void StcGame::setTetromino(int indexTetromino, StcTetromino *tetromino) {

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
void StcGame::start() {

    /* Initialize game data */
    errorCode = GAME_ERROR_NONE;
    systemTime = platform->getSystemTime();
    lastFallTime = systemTime;
    isOver = false;
    isPaused = false;
    showPreview = true;
    events = EVENT_NONE;
    delay = INIT_DELAY_FALL;
#ifdef STC_SHOW_GHOST_PIECE
    showShadow = true;
#endif

    /* Initialize game statistics */
    stats.score = 0;
    stats.lines = 0;
    stats.totalPieces = 0;
    stats.level = 0;
    for (int i = 0; i < TETROMINO_TYPES; ++i) {
        stats.pieces[i] = 0;
    }

    /* Initialize random generator */
    platform->seedRandom(systemTime);

    /* Initialize game tile map */
    setMatrixCells(&map[0][0], BOARD_WIDTH, BOARD_HEIGHT, EMPTY_CELL);

    /* Initialize falling tetromino */
    setTetromino(platform->random() % TETROMINO_TYPES, &fallingBlock);
    fallingBlock.x = (BOARD_WIDTH - fallingBlock.size) / 2;
    fallingBlock.y = 0;

    /* Initialize preview tetromino */
    setTetromino(platform->random() % TETROMINO_TYPES, &nextBlock);

    /* Initialize events */
    onTetrominoMoved();
}

/*
 * Initializes the game, if there are no problems returns GAME_ERROR_NONE.
 */
int StcGame::init(StcPlatform *targetPlatform) {
    if (targetPlatform != NULL) {
        platform = targetPlatform;
        int error = platform->init();
        if (error == GAME_ERROR_NONE) {
            start();
            return GAME_ERROR_NONE;
        }
        return error;
    }
    return GAME_ERROR_NO_MEMORY;
};

/* Free used resources */
void StcGame::end() {
    platform->end();
    delete platform;
    platform = NULL;
}

/*
 * Rotate falling tetromino. If there are no collisions when the 
 * tetromino is rotated this modifies the tetromino's cell buffer.
 */
void StcGame::rotateTetromino(bool clockwise) {
    int i, j;
    int rotated[TETROMINO_SIZE][TETROMINO_SIZE];  /* temporary array to hold rotated cells */

    /* If TETROMINO_O is falling return immediately */
    if (fallingBlock.type == TETROMINO_O) {
        return; /* rotation doesn't require any changes */
    }

    /* Initialize rotated cells to blank */
    setMatrixCells(&rotated[0][0], TETROMINO_SIZE, TETROMINO_SIZE, EMPTY_CELL);

    /* Copy rotated cells to the temporary array */
    for (i = 0; i < fallingBlock.size; ++i) {
        for (j = 0; j < fallingBlock.size; ++j) {
            if (clockwise) {
                rotated[fallingBlock.size - j - 1][i] = fallingBlock.cells[i][j];
            } else {
                rotated[j][fallingBlock.size - i - 1] = fallingBlock.cells[i][j];
            }
        }
    }
#ifdef STC_WALL_KICK_ENABLED
    int wallDisplace = 0;

    /* Check collision with left wall */
    if (fallingBlock.x < 0) {
        for (i = 0; (wallDisplace == 0) && (i < -fallingBlock.x); ++i) {
            for (j = 0; j < fallingBlock.size; ++j) {
                if (rotated[i][j] != EMPTY_CELL) {
                    wallDisplace = i - fallingBlock.x;
                    break;
                }
            }
        }
    }
    /* Or check collision with right wall */
    else if (fallingBlock.x > BOARD_WIDTH - fallingBlock.size) {
        i = fallingBlock.size - 1; 
        for (; (wallDisplace == 0) && (i >= BOARD_WIDTH - fallingBlock.x); --i) {
            for (j = 0; j < fallingBlock.size; ++j) {
                if (rotated[i][j] != EMPTY_CELL) {
                    wallDisplace = -fallingBlock.x - i + BOARD_WIDTH - 1;
                    break;
                }
            }
        }
    }

    /* Check collision with board floor and other cells on board */
    for (i = 0; i < fallingBlock.size; ++i) {
        for (j = 0; j < fallingBlock.size; ++j) {
            if (rotated[i][j] != EMPTY_CELL) {
                /* Check collision with bottom border of the map */
                if (fallingBlock.y + j >= BOARD_HEIGHT) {
                    return; /* there was collision therefore return */
                }
                /* Check collision with existing cells in the map */
                if (map[i + fallingBlock.x + wallDisplace][j + fallingBlock.y] != EMPTY_CELL) {
                    return; /* there was collision therefore return */
                }
            }
        }
    }
    /* Move the falling piece if there was wall collision and it's a legal move */
    if (wallDisplace != 0) {
        fallingBlock.x += wallDisplace;
    }
#else
    /* Check collision of the temporary array */
    for (i = 0; i < fallingBlock.size; ++i) {
        for (j = 0; j < fallingBlock.size; ++j) {
            if (rotated[i][j] != EMPTY_CELL) {
                /* Check collision with left, right or bottom borders of the map */
                if ((fallingBlock.x + i < 0) || (fallingBlock.x + i >= BOARD_WIDTH)
                        || (fallingBlock.y + j >= BOARD_HEIGHT)) {
                    return; /* there was collision therefore return */
                }
                /* Check collision with existing cells in the map */
                if (map[i + fallingBlock.x][j + fallingBlock.y] != EMPTY_CELL) {
                    return; /* there was collision therefore return */
                }
            }
        }
    }
#endif
    /* There are no collisions, replace tetromino cells with rotated cells */
    for (i = 0; i < TETROMINO_SIZE; ++i) {
        for (j = 0; j < TETROMINO_SIZE; ++j) {
            fallingBlock.cells[i][j] = rotated[i][j];
        }
    }
    onTetrominoMoved();
}

/*
 * Check if tetromino will collide with something if it is moved in the requested direction.
 * If there are collisions returns 1 else returns 0.
 */
bool StcGame::checkCollision(int dx, int dy) {

    int newx = fallingBlock.x + dx;
    int newy = fallingBlock.y + dy;

    for (int i = 0; i < fallingBlock.size; ++i) {
        for (int j = 0; j < fallingBlock.size; ++j) {
            if (fallingBlock.cells[i][j] != EMPTY_CELL) {
                /* Check the tetromino would be inside the left, right and bottom borders */
                if ((newx + i < 0) || (newx + i >= BOARD_WIDTH)
                    || (newy + j >= BOARD_HEIGHT)) {
                    return true;
                }
                /* Check the tetromino won't collide with existing cells in the map */
                if (map[newx + i][newy + j] != EMPTY_CELL) {
                    return true;
                }
            }
        }
    }
    return false;
}

/* Game scoring: http://tetris.wikia.com/wiki/Scoring */
void StcGame::onFilledRows(int filledRows) {
    /* Update total number of filled rows */
    stats.lines += filledRows;

    /* Increase score accordingly to the number of filled rows */
    switch (filledRows) {
    case 1:
        stats.score += (SCORE_1_FILLED_ROW * (stats.level + 1));
        break;
    case 2:
        stats.score += (SCORE_2_FILLED_ROW * (stats.level + 1));
        break;
    case 3:
        stats.score += (SCORE_3_FILLED_ROW * (stats.level + 1));
        break;
    case 4:
        stats.score += (SCORE_4_FILLED_ROW * (stats.level + 1));
        break;
    default:
        errorCode = GAME_ERROR_ASSERT;    /* This can't happen */
    }
    /* Check if we need to update level */
    if (stats.lines >= FILLED_ROWS_FOR_LEVEL_UP * (stats.level + 1)) {
        stats.level++;

        /* Increase speed for falling tetrominoes */
        delay = (int)(delay * DELAY_FACTOR_FOR_LEVEL_UP);
    }
}

/*
 * Move tetromino in the direction specified by (x, y) (in tile units)
 * This function detects if there are filled rows or if the move 
 * lands a falling tetromino, also checks for game over condition.
 */
void StcGame::moveTetromino(int x, int y) {
    int i, j;
    
    /* Check if the move would create a collision */
    if (checkCollision(x, y)) {
        /* In case of collision check if move was downwards (y == 1) */
        if (y == 1) {
            /* Check if collision occurs when the falling 
             * tetromino is on the 1st or 2nd row */
            if (fallingBlock.y <= 1) {
                isOver = true;   /* if this happens the game is over */
            }
            else {
                /* The falling tetromino has reached the bottom, 
                 * so we copy their cells to the board map */
                for (i = 0; i < fallingBlock.size; ++i) {
                    for (j = 0; j < fallingBlock.size; ++j) {
                        if (fallingBlock.cells[i][j] != EMPTY_CELL) {
                            map[fallingBlock.x + i][fallingBlock.y + j] 
                                    = fallingBlock.cells[i][j];
                        }
                    }
                }

                /* Check if the landing tetromino has created full rows */
                int numFilledRows = 0;
                for (j = 1; j < BOARD_HEIGHT; ++j) {
                    int hasFullRow = true;
                    for (i = 0; i < BOARD_WIDTH; ++i) {
                        if (map[i][j] == EMPTY_CELL) {
                            hasFullRow = false;
                            break;
                        }
                    }
                    /* If we found a full row we need to remove that row from the map
                     * we do that by just moving all the above rows one row below */
                    if (hasFullRow) {
                        for (x = 0; x < BOARD_WIDTH; ++x) {
                            for (y = j; y > 0; --y) {
                                map[x][y] = map[x][y - 1];
                            }
                        }
                        numFilledRows++;    /* increase filled row counter */
                    }
                }

                /* Update game statistics */
                if (numFilledRows > 0) {
                    onFilledRows(numFilledRows);
                }
                stats.totalPieces++;
                stats.pieces[fallingBlock.type]++;
                
                /* Use preview tetromino as falling tetromino.
                 * Copy preview tetromino for falling tetromino */
                for (i = 0; i < TETROMINO_SIZE; ++i) {
                    for (j = 0; j < TETROMINO_SIZE; ++j) {
                        fallingBlock.cells[i][j] = nextBlock.cells[i][j];
                    }
                }
                fallingBlock.size = nextBlock.size;
                fallingBlock.type = nextBlock.type;

                /* Reset position */
                fallingBlock.y = 0;
                fallingBlock.x = (BOARD_WIDTH - fallingBlock.size) / 2;
                onTetrominoMoved();

                /* Create next preview tetromino */
                setTetromino(platform->random() % TETROMINO_TYPES, &nextBlock);
            }
        }
    }
    else {
        /* There are no collisions, just move the tetromino */
        fallingBlock.x += x;
        fallingBlock.y += y;
    }
    onTetrominoMoved();
}

/* Hard drop */
void StcGame::dropTetromino() {
#ifdef STC_SHOW_GHOST_PIECE
    moveTetromino(0, shadowGap);
    moveTetromino(0, 1); /* Force lock */

    /* Update score */
    if (showShadow) {
        stats.score += (long)(SCORE_DROP_WITH_SHADOW_FACTOR * SCORE_2_FILLED_ROW * (stats.level + 1));
    }
    else {
        stats.score += (long)(SCORE_DROP_FACTOR * SCORE_2_FILLED_ROW * (stats.level + 1));
    }
#else
    int y = 0;
    /* Calculate number of cells to drop */
    while (!checkCollision(0, ++y));
    moveTetromino(0, y - 1);
    moveTetromino(0, 1); /* Force lock */

    /* Update score */
    stats.score += (long)(SCORE_DROP_FACTOR * SCORE_2_FILLED_ROW * (stats.level + 1));
#endif
}

/*
 * Main function game called every frame
 */
void StcGame::update() {

    /* Read user input */
    platform->readInput(this);

    /* Update game state */
    if (isOver) {
        if ((events & EVENT_RESTART) != 0) {
            isOver = false;
            start();
        }
    }
    else {
        long sysTime = platform->getSystemTime();
        
        /* Always handle pause event */
        if ((events & EVENT_PAUSE) != 0) {
            isPaused = !isPaused;
            events = EVENT_NONE;
        }

        /* Check if the game is paused */
        if (isPaused) {
            /* We achieve the effect of pausing the game
             * adding the last frame duration to lastFallTime */
            lastFallTime += (sysTime - systemTime);
        }
        else {
            if (events != EVENT_NONE) {
                if ((events & EVENT_SHOW_NEXT) != 0)  {
                    showPreview = !showPreview;
                    stateChanged = true;
                }
#ifdef STC_SHOW_GHOST_PIECE
                if ((events & EVENT_SHOW_SHADOW) != 0) {
                    showShadow = !showShadow;
                    stateChanged = true;
                }
#endif
                if ((events & EVENT_DROP) != 0) {
                    dropTetromino();
                }
                if ((events & EVENT_ROTATE_CW) != 0) {
                    rotateTetromino(true);
                }
                if ((events & EVENT_MOVE_RIGHT) != 0) {
                    moveTetromino(1, 0);
                }
                else if ((events & EVENT_MOVE_LEFT) != 0) {
                    moveTetromino(-1, 0);
                }
                if ((events & EVENT_MOVE_DOWN) != 0) {
                    /* Update score if the user accelerates downfall */
                    stats.score += (long)(SCORE_MOVE_DOWN_FACTOR * (SCORE_2_FILLED_ROW * (stats.level + 1)));

                    moveTetromino(0, 1);
                }
                events = EVENT_NONE;
            }
            /* Check if it's time to move downwards the falling tetromino */
            if (sysTime - lastFallTime >= delay) {
                moveTetromino(0, 1);
                lastFallTime = sysTime;
            }
        }
        systemTime = sysTime;
    }
    /* Draw game state */
    platform->renderGame(this);
}

/* This event is called when the falling tetromino is moved */
void StcGame::onTetrominoMoved() {
#ifdef STC_SHOW_GHOST_PIECE
    int y = 0;
    /* Calculate number of cells where shadow tetromino would be */
    while (!checkCollision(0, ++y));
    shadowGap = y - 1;
#endif
    stateChanged = true;
}
