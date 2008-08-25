/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Declarations for simple pure SDL implementation.                         */
/*                                                                            */
/*   Copyright (c) 2008 Laurens Rodriguez Oscanoa                             */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#include "../game.h"

#ifdef STC_USE_SIMPLE_SDL

#include <SDL.h>

/* 
 * UI layout (quantities are expressed in pixels)
 */

/* Screen size */
#define SCREEN_WIDTH    (480)
#define SCREEN_HEIGHT   (272)

/* Size of square tile */
#define TILE_SIZE       (12)

/* Board up-left corner coordinates */
#define BOARD_X         (180)
#define BOARD_Y         (4)

/* Preview tetromino position */
#define PREVIEW_X       (96)
#define PREVIEW_Y       (184)

/* Score position and length on screen */
#define SCORE_X         (72)
#define SCORE_Y         (52)
#define SCORE_LENGTH    (10)

/* Lines position and length on screen */
#define LINES_X         (108)
#define LINES_Y         (34)
#define LINES_LENGTH    (5)

/* Level position and length on screen */
#define LEVEL_X         (108)
#define LEVEL_Y         (16)
#define LEVEL_LENGTH    (5)

/* Tetromino subtotals position */
#define TETROMINO_X     (425)
#define TETROMINO_L_Y   (53)
#define TETROMINO_I_Y   (77)
#define TETROMINO_T_Y   (101)
#define TETROMINO_S_Y   (125)
#define TETROMINO_Z_Y   (149)
#define TETROMINO_O_Y   (173)
#define TETROMINO_J_Y   (197)
#define TETROMINO_LENGTH    (5)

/* Tetromino total position */
#define PIECES_X        (418)
#define PIECES_Y        (221)
#define PIECES_LENGTH   (6)

/* Size of number */
#define NUMBER_WIDTH    (7)
#define NUMBER_HEIGHT   (9)

/* 
 * Image files
 */ 
#define BMP_TILE_BLOCKS     "sdl/blocks.bmp"
#define BMP_BACKGROUND      "sdl/back.bmp"
#define BMP_NUMBERS         "sdl/numbers.bmp"

/* Use 32 bits per pixel */
#define SCREEN_BIT_DEPTH        (32)

/* Use video hardware and double buffering */
#define SCREEN_VIDEO_MODE       (SDL_HWSURFACE | SDL_DOUBLEBUF)

/* Here we define the platform dependent data structure */
typedef struct StcPlatform {
    SDL_Surface* screen;
    SDL_Surface* bmpTiles;
    SDL_Surface* bmpBack;
    SDL_Surface* bmpNumbers;
} StcPlatform;

#endif /* STC_USE_SIMPLE_SDL */
