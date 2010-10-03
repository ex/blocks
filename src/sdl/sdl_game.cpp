/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Simple pure SDL implementation. (No sound, no fonts)                     */
/*   We use SDL for rendering the game state, reading user input and timing.  */
/*                                                                            */
/*   Copyright (c) 2010 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#include "sdl_game.hpp"
#include <stdlib.h>
#include <SDL_image.h>

/*
 * Initializes platform, if there are no problems returns GAME_ERROR_NONE.
 */
int StcPlatformSdl::init() {
    /* Start video system */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return GAME_ERROR_NO_VIDEO;
    }

    /* Create game video surface */
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT,
                              SCREEN_BIT_DEPTH,
                              SCREEN_VIDEO_MODE);
    if (screen == NULL) {
        return GAME_ERROR_NO_VIDEO;
    }

    /* Set window caption */
    SDL_WM_SetCaption(GAME_NAME " (C++)", GAME_NAME);

    /* Load images for blocks and background */
    bmpTiles = IMG_Load(BMP_TILE_BLOCKS);
    bmpBack = IMG_Load(BMP_BACKGROUND);
    bmpNumbers = IMG_Load(BMP_NUMBERS);

    if (bmpTiles == NULL || bmpBack == NULL || bmpNumbers == NULL) {
        return GAME_ERROR_NO_IMAGES;
    }

    /* Initialize delayed autoshift */
    lastTime = SDL_GetTicks();
    delayLeft = -1;
    delayRight = -1;
    delayDown = -1;
#ifdef STC_AUTO_ROTATION
    delayRotation = -1;
#endif

    return GAME_ERROR_NONE;
};

/* Return the current system time in milliseconds */
long StcPlatformSdl::getSystemTime() {
    return SDL_GetTicks();
}

/* Read input device and notify game */
void StcPlatformSdl::readInput(StcGame *game) {
    long timeNow, timeDelta;
    SDL_Event event;

    /* Grab events in the queue */
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        /* On quit game */
        case SDL_QUIT:
            game->errorCode = GAME_ERROR_USER_QUITS;
            break;
        /* On key pressed */
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                game->errorCode = GAME_ERROR_USER_QUITS;
                break;
            case SDLK_s:
            case SDLK_DOWN:
                game->events |= EVENT_MOVE_DOWN;
                delayDown = DAS_DELAY_TIMER;
                break;
            case SDLK_w:
            case SDLK_UP:
                game->events |= EVENT_ROTATE_CW;
#ifdef STC_AUTO_ROTATION
                delayRotation = ROTATION_AUTOREPEAT_DELAY;
#endif
                break;
            case SDLK_a:
            case SDLK_LEFT:
                game->events |= EVENT_MOVE_LEFT;
                delayLeft = DAS_DELAY_TIMER;
                break;
            case SDLK_d:
            case SDLK_RIGHT:
                game->events |= EVENT_MOVE_RIGHT;
                delayRight = DAS_DELAY_TIMER;
                break;
            case SDLK_SPACE:
                game->events |= EVENT_DROP;
                break;
            case SDLK_F5:
                game->events |= EVENT_RESTART;
                break;
            case SDLK_F1:
                game->events |= EVENT_PAUSE;
                break;
            case SDLK_F2:
                game->events |= EVENT_SHOW_NEXT;
                break;
#ifdef STC_SHOW_GHOST_PIECE
            case SDLK_F3:
                game->events |= EVENT_SHOW_SHADOW;
                break;
#endif /* STC_SHOW_GHOST_PIECE */
            default:
                break;
            }
            break;
        /* On key released */
        case SDL_KEYUP:
            switch (event.key.keysym.sym) {
            case SDLK_s:
            case SDLK_DOWN:
                delayDown = -1;
                break;
            case SDLK_a:
            case SDLK_LEFT:
                delayLeft = -1;
                break;
            case SDLK_d:
            case SDLK_RIGHT:
                delayRight = -1;
                break;
#ifdef STC_AUTO_ROTATION
            case SDLK_w:
            case SDLK_UP:
                delayRotation = -1;
                break;
#endif /* STC_AUTO_ROTATION */
            default:
                break;
            }
        default:
            break;
        }
    }

    /* Process delayed autoshift */
    timeNow = SDL_GetTicks();
    timeDelta = timeNow - lastTime;
    if (delayDown > 0) {
        delayDown -= timeDelta;
        if (delayDown <= 0) {
            delayDown = DAS_MOVE_TIMER;
            game->events |= EVENT_MOVE_DOWN;
        }
    }
    if (delayLeft > 0) {
        delayLeft -= timeDelta;
        if (delayLeft <= 0) {
            delayLeft = DAS_MOVE_TIMER;
            game->events |= EVENT_MOVE_LEFT;
        }
    }
    else if (delayRight > 0) {
        delayRight -= timeDelta;
        if (delayRight <= 0) {
            delayRight = DAS_MOVE_TIMER;
            game->events |= EVENT_MOVE_RIGHT;
        }
    }
#ifdef STC_AUTO_ROTATION
    if (delayRotation > 0) {
        delayRotation -= timeDelta;
        if (delayRotation <= 0) {
            delayRotation = ROTATION_AUTOREPEAT_TIMER;
            game->events |= EVENT_ROTATE_CW;
        }
    }
#endif /* STC_AUTO_ROTATION */
    lastTime = timeNow;
}

/* Draw a tile from a tetromino */
void StcPlatformSdl::drawTile(StcGame *game, int x, int y, int tile, int shadow) {
    SDL_Rect recDestine;
    SDL_Rect recSource;

    recDestine.x = x;
    recDestine.y = y;
    recSource.x = TILE_SIZE * tile;
    recSource.y = (TILE_SIZE + 1) * shadow;
    recSource.w = TILE_SIZE + 1;
    recSource.h = TILE_SIZE + 1;
    SDL_BlitSurface(bmpTiles, &recSource, screen, &recDestine);
}

/* Draw a number on the given position */
void StcPlatformSdl::drawNumber(StcGame *game, int x, int y, long number, int length, int color) {
    SDL_Rect recDestine;
    SDL_Rect recSource;

    recSource.y = NUMBER_HEIGHT * color;
    recSource.w = NUMBER_WIDTH;
    recSource.h = NUMBER_HEIGHT;
    recDestine.y = y;

    int pos = 0;
    do {
        recDestine.x = x + NUMBER_WIDTH * (length - pos);
        recSource.x = NUMBER_WIDTH * (Sint16)(number % 10);
        SDL_BlitSurface(bmpNumbers, &recSource, screen, &recDestine);
        number /= 10;
    } while (++pos < length);
}

/*
 * Render the state of the game using platform functions
 */
void StcPlatformSdl::renderGame(StcGame *game) {
    int i, j;

    /* Check if the game state has changed, if so redraw */
    if (game->stateChanged) {
        /* Draw background */
        SDL_BlitSurface(bmpBack, NULL, screen, NULL);

        /* Draw preview block */
        if (game->showPreview) {
            for (i = 0; i < TETROMINO_SIZE; ++i) {
                for (j = 0; j < TETROMINO_SIZE; ++j) {
                    if (game->nextBlock.cells[i][j] != EMPTY_CELL) {
                        drawTile(game,
                                 PREVIEW_X + (TILE_SIZE * i),
                                 PREVIEW_Y + (TILE_SIZE * j),
                                 game->nextBlock.cells[i][j]);
                    }
                }
            }
        }
#ifdef STC_SHOW_GHOST_PIECE
        /* Draw shadow tetromino */
        if (game->showShadow && game->shadowGap > 0) {
            for (i = 0; i < TETROMINO_SIZE; ++i) {
                for (j = 0; j < TETROMINO_SIZE; ++j) {
                    if (game->fallingBlock.cells[i][j] != EMPTY_CELL) {
                        drawTile(game,
                                 BOARD_X + (TILE_SIZE * (game->fallingBlock.x + i)),
                                 BOARD_Y + (TILE_SIZE * (game->fallingBlock.y + game->shadowGap + j)),
                                 game->fallingBlock.cells[i][j], 1);
                    }
                }
            }
        }
#endif
        /* Draw the cells in the board */
        for (i = 0; i < BOARD_WIDTH; ++i) {
            for (j = 0; j < BOARD_HEIGHT; ++j) {
                if (game->map[i][j] != EMPTY_CELL) {
                    drawTile(game,
                             BOARD_X + (TILE_SIZE * i),
                             BOARD_Y + (TILE_SIZE * j),
                             game->map[i][j]);
                }
            }
        }

        /* Draw falling tetromino */
        for (i = 0; i < TETROMINO_SIZE; ++i) {
            for (j = 0; j < TETROMINO_SIZE; ++j) {
                if (game->fallingBlock.cells[i][j] != EMPTY_CELL) {
                    drawTile(game,
                             BOARD_X + (TILE_SIZE * (game->fallingBlock.x + i)),
                             BOARD_Y + (TILE_SIZE * (game->fallingBlock.y + j)),
                             game->fallingBlock.cells[i][j]);
                }
            }
        }

        /* Draw game statistic data */
        if (!game->isPaused) {
            drawNumber(game, LEVEL_X, LEVEL_Y, game->stats.level, LEVEL_LENGTH, COLOR_WHITE);
            drawNumber(game, LINES_X, LINES_Y, game->stats.lines, LINES_LENGTH, COLOR_WHITE);
            drawNumber(game, SCORE_X, SCORE_Y, game->stats.score, SCORE_LENGTH, COLOR_WHITE);

            drawNumber(game, TETROMINO_X, TETROMINO_L_Y, game->stats.pieces[TETROMINO_L], TETROMINO_LENGTH, COLOR_ORANGE);
            drawNumber(game, TETROMINO_X, TETROMINO_I_Y, game->stats.pieces[TETROMINO_I], TETROMINO_LENGTH, COLOR_CYAN);
            drawNumber(game, TETROMINO_X, TETROMINO_T_Y, game->stats.pieces[TETROMINO_T], TETROMINO_LENGTH, COLOR_PURPLE);
            drawNumber(game, TETROMINO_X, TETROMINO_S_Y, game->stats.pieces[TETROMINO_S], TETROMINO_LENGTH, COLOR_GREEN);
            drawNumber(game, TETROMINO_X, TETROMINO_Z_Y, game->stats.pieces[TETROMINO_Z], TETROMINO_LENGTH, COLOR_RED);
            drawNumber(game, TETROMINO_X, TETROMINO_O_Y, game->stats.pieces[TETROMINO_O], TETROMINO_LENGTH, COLOR_YELLOW);
            drawNumber(game, TETROMINO_X, TETROMINO_J_Y, game->stats.pieces[TETROMINO_J], TETROMINO_LENGTH, COLOR_BLUE);

            drawNumber(game, PIECES_X, PIECES_Y, game->stats.totalPieces, PIECES_LENGTH, COLOR_WHITE);
        }

        /* Clear the game state */
        game->stateChanged = false;

        /* Swap video buffers */
        SDL_Flip(screen);
    }

    /* Resting game */
    SDL_Delay(DAS_MOVE_TIMER);
}

/* Initialize the random number generator */
void StcPlatformSdl::seedRandom(long seed) {
    srand(seed);
}

/* Return a random positive integer number */
int StcPlatformSdl::random() {
    return rand();
}

/*
 * Release platform allocated resources
 */
void StcPlatformSdl::end() {
    /* Free all the created surfaces */
    SDL_FreeSurface(screen);
    SDL_FreeSurface(bmpTiles);
    SDL_FreeSurface(bmpBack);
    SDL_FreeSurface(bmpNumbers);

    /* Shut down SDL */
    SDL_Quit();
}
