/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Constants and definitions for simple pure SDL implementation.            */
/*                                                                            */
/*   Copyright (c) 2010 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#include "../game.hpp"

#ifndef STC_SDL_GAME_HPP_
#define STC_SDL_GAME_HPP_

#include <SDL.h>

namespace Stc {

/*
 * Image files
 */
#define STC_BMP_TILE_BLOCKS     "sdl/blocks.png"
#define STC_BMP_BACKGROUND      "sdl/back.png"
#define STC_BMP_NUMBERS         "sdl/numbers.png"

/*
 * SDL platform implementation
 */
class PlatformSdl : public Platform {
  public:
    /*
     * UI layout (quantities are expressed in pixels)
     */

    /* Screen size */
    static const int SCREEN_WIDTH  = 480;
    static const int SCREEN_HEIGHT = 272;

    /* Size of square tile */
    static const int TILE_SIZE = 12;

    /* Board up-left corner coordinates */
    static const int BOARD_X = 180;
    static const int BOARD_Y = 4;

    /* Preview tetromino position */
    static const int PREVIEW_X = 112;
    static const int PREVIEW_Y = 210;

    /* Score position and length on screen */
    static const int SCORE_X      = 72;
    static const int SCORE_Y      = 52;
    static const int SCORE_LENGTH = 10;

    /* Lines position and length on screen */
    static const int LINES_X      = 108;
    static const int LINES_Y      = 34;
    static const int LINES_LENGTH = 5;

    /* Level position and length on screen */
    static const int LEVEL_X      = 108;
    static const int LEVEL_Y      = 16;
    static const int LEVEL_LENGTH = 5;

    /* Tetromino subtotals position */
    static const int TETROMINO_X   = 425;
    static const int TETROMINO_L_Y = 53;
    static const int TETROMINO_I_Y = 77;
    static const int TETROMINO_T_Y = 101;
    static const int TETROMINO_S_Y = 125;
    static const int TETROMINO_Z_Y = 149;
    static const int TETROMINO_O_Y = 173;
    static const int TETROMINO_J_Y = 197;

    /* Size of subtotals */
    static const int TETROMINO_LENGTH = 5;

    /* Tetromino total position */
    static const int PIECES_X      = 418;
    static const int PIECES_Y      = 221;
    static const int PIECES_LENGTH = 6;

    /* Size of number */
    static const int NUMBER_WIDTH  = 7;
    static const int NUMBER_HEIGHT = 9;

    /* Use 32 bits per pixel */
    static const int SCREEN_BIT_DEPTH = 32;

    /* Use video hardware and double buffering */
    static const int SCREEN_VIDEO_MODE = (SDL_HWSURFACE | SDL_DOUBLEBUF);

    /* Sleep time (in milliseconds) */
    static const int SLEEP_TIME = 40;

    /* Initializes platform */
    virtual int init(Game *game);

    /* Clear resources used by platform */
    virtual void end();

    /* Read input device and notify game */
    virtual void processEvents();

    /* Render the state of the game */
    virtual void renderGame();

    /* Return the current system time in milliseconds */
    virtual long getSystemTime();

    /* Initialize the random number generator */
    virtual void seedRandom(long seed);

    /* Return a random positive integer number */
    virtual int random();

  private:

    SDL_Surface* mScreen;
    SDL_Surface* mBmpTiles;
    SDL_Surface* mBmpBack;
    SDL_Surface* mBmpNumbers;

    Game* mGame;

    void drawTile(int x, int y, int tile, bool shadow);
    void drawNumber(int x, int y, long number, int length, int color);
};
}

#endif /* STC_SDL_GAME_HPP_ */
