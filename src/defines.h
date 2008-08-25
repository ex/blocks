/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Game parameters and definitions.                                         */
/*                                                                            */
/*   Copyright (c) 2008 Laurens Rodriguez Oscanoa                             */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#ifndef STC_SRC_DEFINES_H_
#define STC_SRC_DEFINES_H_

/* Here we define the platform to use */
#define STC_USE_SIMPLE_SDL  /* Using basic SDL, no additional libraries */

/* Application name */
#define GAME_NAME       "STC: simple tetris clone"

/* Playfield size (in tiles) */
#define BOARD_WIDTH     (10)
#define BOARD_HEIGHT    (22)

/* Initial time delay (in miliseconds) between falling moves */
#define INI_DELAY_FALL  (1000)

/* Score points given by filled rows
 * http://www.tetrisconcept.com/wiki/index.php/Scoring */
#define SCORE_1_FILLED_ROW  (40)
#define SCORE_2_FILLED_ROW  (100)
#define SCORE_3_FILLED_ROW  (300)
#define SCORE_4_FILLED_ROW  (1200)

/* Number of filled rows required to increase the game level */
#define FILLED_ROWS_FOR_LEVEL_UP    (10)

/* The falling delay is multiplied by this factor with every level up */
#define DELAY_FACTOR_FOR_LEVEL_UP   (0.9f)

#endif /* STC_SRC_DEFINES_H_ */
