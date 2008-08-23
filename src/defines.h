/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Game parameters and definitions.                                         */
/*                                                                            */
/*   Copyright (C) 2008 Laurens Rodriguez Oscanoa                             */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#ifndef STC_SRC_DEFINES_H_
#define STC_SRC_DEFINES_H_

#define STC_USE_SIMPLE_SDL  /* Use simple SDL for drawing and controlling */

/*
 * Game parameters
 */
#define GAME_NAME       "STC: simple tetris clone"

/* Screen size (in pixels) */
#define SCREEN_WIDTH    (480)
#define SCREEN_HEIGHT   (272)

/* Size of square tile (in pixels) */
#define TILE_SIZE       (12)

/* Board size (in tiles) */
#define BOARD_WIDTH     (10)
#define BOARD_HEIGHT    (22)

/* Board up-left corner coordinates (in pixels) */
#define BOARD_X         (180)
#define BOARD_Y         (4)

/* Preview tetromino position (in tiles with respect to the board) */
#define NEXT_TETROMINO_X    (-7)
#define NEXT_TETROMINO_Y    (15)

/* Time (in miliseconds) between falling moves */
#define INI_DELAY_FALL  (1000)

/* Tetromino definitions */
/* http://www.tetrisconcept.com/wiki/index.php/Tetromino */
#define TETROMINO_I     (0)
#define TETROMINO_O     (1)
#define TETROMINO_T     (2)
#define TETROMINO_S     (3)
#define TETROMINO_Z     (4)
#define TETROMINO_J     (5)
#define TETROMINO_L     (6)

/* Tetromino color indexes */
#define COLOR_CYAN      (1)
#define COLOR_RED       (2)
#define COLOR_BLUE      (3)
#define COLOR_ORANGE    (4)
#define COLOR_GREEN     (5)
#define COLOR_YELLOW    (6)
#define COLOR_PURPLE    (7)
#define COLOR_WHITE     (0)     /* Used for effects (if any) */

#endif /* STC_SRC_DEFINES_H_ */
