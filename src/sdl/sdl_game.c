/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Simple pure SDL implementation. (No sound, no fonts)                     */
/*   We use SDL for rendering the game state, reading user input and timing.  */
/*                                                                            */
/*   Copyright (C) 2008 Laurens Rodriguez Oscanoa                             */
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
            case SDLK_DOWN:
                game->events |= EVENT_MOVE_DOWN;
                break;
            case SDLK_UP:
                game->events |= EVENT_ROTATE_CW;
                break;
            case SDLK_LEFT:
                game->events |= EVENT_MOVE_LEFT;
                break;
            case SDLK_RIGHT:
                game->events |= EVENT_MOVE_RIGHT;
                break;
            case SDLK_SPACE:
                game->events |= EVENT_DROP;
                break;
            case SDLK_r:
                game->events |= EVENT_RESTART;
                break;
            case SDLK_F1:
                game->events |= EVENT_PAUSE;
                break;
            }
            break;
        }
    }
}

/* Draws a tile from a tetromino */
static void drawTile(StcGame *game, int x, int y, int tile) {
    SDL_Rect recDestine;
    SDL_Rect recSource;

    recDestine.x = (TILE_SIZE * x) + BOARD_X;
    recDestine.y = (TILE_SIZE * y) + BOARD_Y;
    recSource.x = tile*TILE_SIZE;
    recSource.y = 0;
    recSource.w = TILE_SIZE;
    recSource.h = TILE_SIZE;
    SDL_BlitSurface(game->platform->bmpTiles, &recSource, game->platform->screen, &recDestine);
}

/*
 * Render the state of the game using platform functions
 */
void platformRenderGame(StcGame *game) {
    int i, j;

    /* Draw background */
    SDL_BlitSurface(game->platform->bmpBack, NULL, game->platform->screen, NULL);

    /* Draw preview block */
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            if (game->nextBlock.cells[i][j] != EMPTY_CELL) {
                drawTile(game, game->nextBlock.x + i, game->nextBlock.y + j, game->nextBlock.cells[i][j]);
            }
        }
    }
    /* Draw the cells in the board */
    for (i = 0; i < BOARD_WIDTH; ++i) {
        for (j = 0; j < BOARD_HEIGHT; ++j) {
            if (game->map[i][j] != EMPTY_CELL) {
                drawTile(game, i, j, game->map[i][j]);
            }
        }
    }
    /* Draw falling tetromino */
    for (i = 0; i<4; ++i) {
        for(j=0; j<4; ++j) {
            if (game->fallingBlock.cells[i][j] != EMPTY_CELL) {
                drawTile(game, game->fallingBlock.x + i, game->fallingBlock.y + j, game->fallingBlock.cells[i][j]);
            }
        }
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
    game->platform->bmpTiles = SDL_LoadBMP("sdl/blocks.bmp");
    game->platform->bmpBack = SDL_LoadBMP("sdl/back.bmp");
    if (!game->platform->bmpTiles || !game->platform->bmpBack) {
        return GAME_ERROR_NO_IMAGES;
    }

    return GAME_ERROR_NONE;
};

#endif /* STC_USE_SIMPLE_SDL */
