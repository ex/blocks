/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Simple pure SDL implementation. (No sound, no fonts)                     */
/*   Using SDL for game state rendering, user input and timing.               */
/*                                                                            */
/*   Copyright (c) 2011 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#include "sdl_game.h"
#include <stdlib.h>

/*
 * Initialize platform, if there are no problems return ERROR_NONE.
 */
int platformInit(StcGame *game) {

    /* First we initialize the platform data */
    game->platform = (StcPlatform *) malloc(sizeof(StcPlatform));
    if (game->platform == NULL) {
        return ERROR_NO_MEMORY;
    }

    /* Start video system */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return ERROR_NO_VIDEO;
    }

    /* Create game video surface */
    game->platform->screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT,
                                              SCREEN_BIT_DEPTH,
                                              SCREEN_VIDEO_MODE);
    if (game->platform->screen == NULL) {
        return ERROR_NO_VIDEO;
    }

    /* Set window caption */
    SDL_WM_SetCaption(STC_GAME_NAME " (ANSI C)", STC_GAME_NAME);

    /* Load images for blocks and background */
    game->platform->bmpTiles = SDL_LoadBMP(STC_BMP_TILE_BLOCKS);
    game->platform->bmpBack = SDL_LoadBMP(STC_BMP_BACKGROUND);
    game->platform->bmpNumbers = SDL_LoadBMP(STC_BMP_NUMBERS);

    if (game->platform->bmpTiles == NULL || game->platform->bmpBack == NULL
                                         || game->platform->bmpNumbers == NULL) {
        return ERROR_NO_IMAGES;
    }

    return ERROR_NONE;
}

/* Return the current system time in milliseconds */
long platformGetSystemTime() {
    return SDL_GetTicks();
}

/* Process events and notify game */
void platformProcessEvents(StcGame *game) {
    SDL_Event event;

    /* Grab events in the queue */
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        /* On quit game */
        case SDL_QUIT:
            gameOnEventStart(game, EVENT_QUIT);
            break;
        /* On key pressed */
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                gameOnEventStart(game, EVENT_QUIT);
                break;
            case SDLK_s:
            case SDLK_DOWN:
                gameOnEventStart(game, EVENT_MOVE_DOWN);
                break;
            case SDLK_w:
            case SDLK_UP:
                gameOnEventStart(game, EVENT_ROTATE_CW);
                break;
            case SDLK_a:
            case SDLK_LEFT:
                gameOnEventStart(game, EVENT_MOVE_LEFT);
                break;
            case SDLK_d:
            case SDLK_RIGHT:
                gameOnEventStart(game, EVENT_MOVE_RIGHT);
                break;
            case SDLK_SPACE:
                gameOnEventStart(game, EVENT_DROP);
                break;
            case SDLK_F5:
                gameOnEventStart(game, EVENT_RESTART);
                break;
            case SDLK_F1:
                gameOnEventStart(game, EVENT_PAUSE);
                break;
            case SDLK_F2:
                gameOnEventStart(game, EVENT_SHOW_NEXT);
                break;
#ifdef STC_SHOW_GHOST_PIECE
            case SDLK_F3:
                gameOnEventStart(game, EVENT_SHOW_SHADOW);
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
                gameOnEventEnd(game, EVENT_MOVE_DOWN);
                break;
            case SDLK_a:
            case SDLK_LEFT:
                gameOnEventEnd(game, EVENT_MOVE_LEFT);
                break;
            case SDLK_d:
            case SDLK_RIGHT:
                gameOnEventEnd(game, EVENT_MOVE_RIGHT);
                break;
#ifdef STC_AUTO_ROTATION
            case SDLK_w:
            case SDLK_UP:
                gameOnEventEnd(game, EVENT_ROTATE_CW);
                break;
#endif /* STC_AUTO_ROTATION */
            default:
                break;
            }
        default:
            break;
        }
    }
}

/* Draw a tile from a tetromino */
static void drawTile(StcGame *game, int x, int y, int tile) {
    SDL_Rect recDestine;
    SDL_Rect recSource;

    recDestine.x = x;
    recDestine.y = y;
    recSource.x = TILE_SIZE * tile;
    recSource.y = 0;
    recSource.w = TILE_SIZE;
    recSource.h = TILE_SIZE;
    SDL_BlitSurface(game->platform->bmpTiles, &recSource, game->platform->screen, &recDestine);
}

/* Draw a number on the given position */
static void drawNumber(StcGame *game, int x, int y, long number, int length, int color) {
    SDL_Rect recDestine;
    SDL_Rect recSource;
    int pos;

    recSource.y = NUMBER_HEIGHT * color;
    recSource.w = NUMBER_WIDTH;
    recSource.h = NUMBER_HEIGHT;
    recDestine.y = y;

    pos = 0;
    do {
        recDestine.x = x + NUMBER_WIDTH * (length - pos);
        recSource.x = NUMBER_WIDTH * (Sint16)(number % 10);
        SDL_BlitSurface(game->platform->bmpNumbers, &recSource, game->platform->screen, &recDestine);
        number /= 10;
    } while (++pos < length);
}

/*
 * Render the state of the game using platform functions
 */
void platformRenderGame(StcGame *game) {
    int i, j;

    /* Check if the game state has changed, if so redraw */
    if (game->stateChanged != 0) {

        /* Draw background */
        SDL_BlitSurface(game->platform->bmpBack, NULL, game->platform->screen, NULL);

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
                                 game->fallingBlock.cells[i][j] + TETROMINO_TYPES + 1);
                    }
                }
            }
        }
#endif
        /* Draw the cells in the board */
        for (i = 0; i < BOARD_TILEMAP_WIDTH; ++i) {
            for (j = 0; j < BOARD_TILEMAP_HEIGHT; ++j) {
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
        if (game->isPaused == 0) {
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
        game->stateChanged = 0;

        /* Swap video buffers */
        SDL_Flip(game->platform->screen);
    }

    /* Resting game */
    SDL_Delay(SLEEP_TIME);
}

/* Initialize the random number generator */
void platformSeedRandom(long seed) {
    srand(seed);
}

/* Return a random positive integer number */
int platformRandom() {
    return rand();
}

/*
 * Release platform allocated resources
 */
void platformEnd(StcGame *game) {
    if (game->platform != NULL) {
        /* Free all the created surfaces */
        SDL_FreeSurface(game->platform->bmpTiles);
        SDL_FreeSurface(game->platform->bmpBack);
        SDL_FreeSurface(game->platform->bmpNumbers);
        SDL_FreeSurface(game->platform->screen);

        /* Shut down SDL */
        SDL_Quit();

        /* Release allocated memory */
        free(game->platform);
        game->platform = NULL;
    }
}
