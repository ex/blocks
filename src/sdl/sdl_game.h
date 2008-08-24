/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Declarations for simple pure SDL implementation.                         */
/*                                                                            */
/*   Copyright (C) 2008 Laurens Rodriguez Oscanoa                             */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#include "../game.h"

#ifdef STC_USE_SIMPLE_SDL

#include <SDL.h>

/* Use 32 bits per pixel */
#define SCREEN_BIT_DEPTH        (32)

/* Use video hardware and double buffering */
#define SCREEN_VIDEO_MODE       (SDL_HWSURFACE | SDL_DOUBLEBUF)

/* Here we define the platform dependent data structure */
typedef struct StcPlatform {
    SDL_Surface* screen;
    SDL_Surface* bmpTiles;
    SDL_Surface* bmpBack;
} StcPlatform;

#endif /* STC_USE_SIMPLE_SDL */
