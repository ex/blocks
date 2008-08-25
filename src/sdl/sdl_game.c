/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Simple pure SDL implementation. (No sound, no fonts)                     */
/*   We use SDL for rendering the game state, reading user input and timing.  */
/*                                                                            */
/*   Copyright (c) 2008 Laurens Rodriguez Oscanoa                             */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#include "sdl_game.h"
#include <stdlib.h>

#ifdef STC_USE_SIMPLE_SDL

/* Return the current system time in milliseconds */
long platformGetSystemTime() {
    return SDL_GetTicks();
}

/* Read input device and notify game */
void platformReadInput(StcGame *game) {
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
            switch(event.key.keysym.sym) {
            case SDLK_ESCAPE:
                game->errorCode = GAME_ERROR_USER_QUITS;
                break;
            case SDLK_s:
            case SDLK_DOWN:
                game->events |= EVENT_MOVE_DOWN;
                break;
            case SDLK_w:
            case SDLK_UP:
                game->events |= EVENT_ROTATE_CW;
                break;
            case SDLK_a:
            case SDLK_LEFT:
                game->events |= EVENT_MOVE_LEFT;
                break;
            case SDLK_d:
            case SDLK_RIGHT:
                game->events |= EVENT_MOVE_RIGHT;
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
            }
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
        recSource.x = NUMBER_WIDTH * (number % 10);
        SDL_BlitSurface(game->platform->bmpNumbers, &recSource, game->platform->screen, &recDestine);
        number /= 10;
    } while (++pos < length);
}

/*
 * Render the state of the game using platform functions
 */
void platformRenderGame(StcGame *game) {
    int i, j;

    /* Draw background */
    SDL_BlitSurface(game->platform->bmpBack, NULL, game->platform->screen, NULL);

    /* Draw preview block */
    if (game->showPreview) {
        for (i = 0; i < 4; ++i) {
            for (j = 0; j < 4; ++j) {
                if (game->nextBlock.cells[i][j] != EMPTY_CELL) {
                    drawTile(game, PREVIEW_X + (TILE_SIZE * i),
                            PREVIEW_Y + (TILE_SIZE * j), game->nextBlock.cells[i][j]);
                }
            }
        }
    }
    /* Draw the cells in the board */
    for (i = 0; i < BOARD_WIDTH; ++i) {
        for (j = 0; j < BOARD_HEIGHT; ++j) {
            if (game->map[i][j] != EMPTY_CELL) {
                drawTile(game, BOARD_X + (TILE_SIZE * i),
                        BOARD_Y + (TILE_SIZE * j), game->map[i][j]);
            }
        }
    }
    /* Draw falling tetromino */
    for (i = 0; i<4; ++i) {
        for (j = 0; j < 4; ++j) {
            if (game->fallingBlock.cells[i][j] != EMPTY_CELL) {
                drawTile(game, BOARD_X + (TILE_SIZE * (game->fallingBlock.x + i)), 
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
    /* Swap video buffers */
    SDL_Flip(game->platform->screen);
}

/*
 * Release platform allocated resources
 */
void platformEnd(StcGame *game) {
    /* Free all the created surfaces */
    SDL_FreeSurface(game->platform->screen);
    SDL_FreeSurface(game->platform->bmpTiles);
    SDL_FreeSurface(game->platform->bmpBack);
    SDL_FreeSurface(game->platform->bmpNumbers);

    /* Shut down SDL */
    SDL_Quit();
}

/*
 * Initializes platform, if there are no problems returns GAME_ERROR_NONE.
 */
int platformInit(StcGame *game) {

    /* First we initialize the platform data */
    game->platform = (StcPlatform *) malloc(sizeof(StcPlatform));
    if (game->platform == NULL) {
        return GAME_ERROR_NO_MEMORY;
    }

    /* Start video system */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return GAME_ERROR_NO_VIDEO;
    }

    /* Create game video surface */
    game->platform->screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT,
            SCREEN_BIT_DEPTH, SCREEN_VIDEO_MODE);

    if (!game->platform->screen) {
        return GAME_ERROR_NO_VIDEO;
    }

    /* Set window caption */
    SDL_WM_SetCaption(GAME_NAME, GAME_NAME);

    /* Load images for blocks and background */
    game->platform->bmpTiles = SDL_LoadBMP(BMP_TILE_BLOCKS);
    game->platform->bmpBack = SDL_LoadBMP(BMP_BACKGROUND);
    game->platform->bmpNumbers = SDL_LoadBMP(BMP_NUMBERS);

    if (!game->platform->bmpTiles || !game->platform->bmpBack || !game->platform->bmpNumbers) {
        return GAME_ERROR_NO_IMAGES;
    }

    return GAME_ERROR_NONE;
};

#endif /* STC_USE_SIMPLE_SDL */
