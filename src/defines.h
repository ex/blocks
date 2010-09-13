/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Game parameters.                                                         */
/*                                                                            */
/*   Some symbols you can define for the project:                             */
/*                                                                            */
/*   STC_USE_SIMPLE_SDL:        define this for basic SDL support.            */
/*                                                                            */
/*   STC_SHOW_GHOST_PIECE:      define this for showing the shadow piece.     */
/*                              http://tetris.wikia.com/wiki/Ghost_piece      */
/*                                                                            */
/*   STC_WALL_KICK_ENABLED:     define this for enabling wall kick.           */
/*                              http://tetris.wikia.com/wiki/Wall_kick        */
/*                                                                            */
/*   Copyright (c) 2010 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#ifndef STC_SRC_DEFINES_H_
#define STC_SRC_DEFINES_H_

/* Application name */
#define GAME_NAME       "STC: simple tetris clone"

/* Playfield size (in tiles) */
#define BOARD_WIDTH     (10)
#define BOARD_HEIGHT    (22)

/* Initial time delay (in milliseconds) between falling moves */
#define INIT_DELAY_FALL (1000)

/* Score points given by filled rows (we use the original NES * 10)
 * http://tetris.wikia.com/wiki/Scoring */
#define SCORE_1_FILLED_ROW  (400)
#define SCORE_2_FILLED_ROW  (1000)
#define SCORE_3_FILLED_ROW  (3000)
#define SCORE_4_FILLED_ROW  (12000)

/* User gets more score if he uses hard drop. (factor of SCORE_2_FILLED_ROW) */
#define SCORE_DROP_FACTOR (0.05f)
#define SCORE_DROP_WITH_SHADOW_FACTOR (0.01f)

/* User gets points every time he accelerates downfall (factor of SCORE_2_FILLED_ROW) */
#define SCORE_MOVE_DOWN_FACTOR (0.001f)

/* Number of filled rows required to increase the game level */
#define FILLED_ROWS_FOR_LEVEL_UP    (10)

/* The falling delay is multiplied by this factor with every level up */
#define DELAY_FACTOR_FOR_LEVEL_UP   (0.9f)

#endif /* STC_SRC_DEFINES_H_ */
