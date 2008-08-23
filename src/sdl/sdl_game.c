/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Simple pure SDL implementation. (No sound, no fonts)                     */
/*   We use SDL for rendering the game state, reading user input and timers.  */
/*                                                                            */
/*   Copyright (C) 2008 Laurens Rodriguez Oscanoa                             */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#include "sdl_game.h"
#include <stdlib.h>

#ifdef STC_USE_SIMPLE_SDL

long getSystemTime() {
    return SDL_GetTicks();
}

void gameReadInput(StcGame *game) {
    SDL_Event Event;

    /* Grab events in the queue */
    while (SDL_PollEvent(&Event)) {
        switch (Event.type) {
        /* On quit game */
        case SDL_QUIT:
            game->errorCode = GAME_ERROR_USER_QUITS;
            break;
        /* On key pressed */
        case SDL_KEYDOWN:
            switch(Event.key.keysym.sym) {
            case SDLK_ESCAPE:
                game->errorCode = GAME_ERROR_USER_QUITS;
                break;
            case SDLK_DOWN:
                moveTetramino(game, 0, 1);
                break;
            case SDLK_UP:
                rotateTetramino(game, 1);
                break;
            case SDLK_LEFT:
                moveTetramino(game, -1, 0);
                break;
            case SDLK_RIGHT:
                moveTetramino(game, 1, 0);
                break;
            case SDLK_SPACE:
                dropTetramino(game);
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
void gameRender(StcGame *game) {
    int x, y;

    /* Draw background */
    SDL_BlitSurface(game->platform->bmpBack, NULL, game->platform->screen, NULL);

    /* Draw preview block */
    for (x = 0; x < 4; ++x)
        for (y = 0; y < 4; ++y)
            if (game->nextBlock.cells[x][y] != EMPTY_CELL)
                drawTile(game, game->nextBlock.x + x, game->nextBlock.y + y, game->nextBlock.cells[x][y]);

    /* Draw the map */
    for (x = 0; x < BOARD_WIDTH; ++x)
        for (y = 0; y < BOARD_HEIGHT; ++y)
            if (game->map[x][y] != EMPTY_CELL)
                drawTile(game, x, y, game->map[x][y]);

    /* Draw falling tetromino */
    for (x = 0; x<4; ++x)
        for(y=0; y<4; ++y)
            if (game->fallingBlock.cells[x][y] != EMPTY_CELL)
                drawTile(game, game->fallingBlock.x+x, game->fallingBlock.y+y, game->fallingBlock.cells[x][y]);

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
 * Initialices platform, if there are no problems returns GAME_ERROR_NONE.
 */
int platformInit(StcGame *game) {

    /* First we initialize the platform data */
    game->platform = (StcPlatform *) malloc(sizeof(StcPlatform));
    if (game->platform == NULL) {
        return GAME_ERROR_NO_MEMORY;
    }
    else {
        game->platform->screen = (SDL_Surface *) malloc(sizeof(SDL_Surface *));
        game->platform->bmpTiles = (SDL_Surface *) malloc(sizeof(SDL_Surface *));
        game->platform->bmpBack = (SDL_Surface *) malloc(sizeof(SDL_Surface *));
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
