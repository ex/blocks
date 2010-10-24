/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Constants and definitions for simple pure SDL implementation.            */
/*                                                                            */
/*   Copyright (c) 2010 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#include "../game.h"

#ifndef STC_SDL_GAME_H_
#define STC_SDL_GAME_H_

#include <SDL.h>

/*
 * Image files
 */
#define STC_BMP_TILE_BLOCKS     "sdl/blocks.bmp"
#define STC_BMP_BACKGROUND      "sdl/back.bmp"
#define STC_BMP_NUMBERS         "sdl/numbers.bmp"

/*
 * UI layout (quantities are expressed in pixels)
 */

/* Screen size */
enum { SCREEN_WIDTH  = 480 };
enum { SCREEN_HEIGHT = 272 };

/* Size of square tile */
enum { TILE_SIZE = 12 };

/* Board up-left corner coordinates */
enum { BOARD_X = 180 };
enum { BOARD_Y = 4 };

/* Preview tetromino position */
enum { PREVIEW_X = 112 };
enum { PREVIEW_Y = 210 };

/* Score position and length on screen */
enum { SCORE_X      = 72 };
enum { SCORE_Y      = 52 };
enum { SCORE_LENGTH = 10 };

/* Lines position and length on screen */
enum { LINES_X      = 108 };
enum { LINES_Y      = 34 };
enum { LINES_LENGTH = 5 };

/* Level position and length on screen */
enum { LEVEL_X      = 108 };
enum { LEVEL_Y      = 16 };
enum { LEVEL_LENGTH = 5 };

/* Tetromino subtotals position */
enum { TETROMINO_X   = 425 };
enum { TETROMINO_L_Y = 53 };
enum { TETROMINO_I_Y = 77 };
enum { TETROMINO_T_Y = 101 };
enum { TETROMINO_S_Y = 125 };
enum { TETROMINO_Z_Y = 149 };
enum { TETROMINO_O_Y = 173 };
enum { TETROMINO_J_Y = 197 };

/* Size of subtotals */
enum { TETROMINO_LENGTH = 5 };

/* Tetromino total position */
enum { PIECES_X      = 418 };
enum { PIECES_Y      = 221 };
enum { PIECES_LENGTH = 6 };

/* Size of number */
enum { NUMBER_WIDTH  = 7 };
enum { NUMBER_HEIGHT = 9 };

/* Use 32 bits per pixel */
enum { SCREEN_BIT_DEPTH = 32 };

/* Use video hardware and double buffering */
enum { SCREEN_VIDEO_MODE = (SDL_HWSURFACE | SDL_DOUBLEBUF) };

/* Sleep time (in milliseconds) */
enum { SLEEP_TIME = 40 };

/* Here we define the platform dependent data structure */
struct StcPlatform {
    SDL_Surface* screen;
    SDL_Surface* bmpTiles;
    SDL_Surface* bmpBack;
    SDL_Surface* bmpNumbers;
};

#endif /* STC_SDL_GAME_H_ */
