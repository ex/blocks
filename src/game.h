/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Game constants and definitions.                                          */
/*                                                                            */
/*   Copyright (c) 2011 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#ifndef STC_SRC_GAME_H_
#define STC_SRC_GAME_H_

#include "platform.h"

/*
 * Game configuration.
 * Edit this section to change the sizes, scores or pace of the game.
 */

/* Application name */
#define STC_GAME_NAME       "STC: simple tetris clone"

/* Playfield size (in tiles) */
enum { BOARD_TILEMAP_WIDTH  = 10 };
enum { BOARD_TILEMAP_HEIGHT = 22 };

/* Initial time delay (in milliseconds) between falling moves */
enum { STC_INIT_DELAY_FALL = 1000 };

/* Score points given by filled rows (we use the original NES * 10)
 * http://tetris.wikia.com/wiki/Scoring */
enum { SCORE_1_FILLED_ROW = 400 };
enum { SCORE_2_FILLED_ROW = 1000 };
enum { SCORE_3_FILLED_ROW = 3000 };
enum { SCORE_4_FILLED_ROW = 12000 };

/* The player gets points every time he accelerates downfall.
 * The added points are equal to SCORE_2_FILLED_ROW divided by this value */
enum { SCORE_MOVE_DOWN_DIVISOR = 1000 };

/* The player gets points every time he does a hard drop.
 * The added points are equal to SCORE_2_FILLED_ROW divided by these
 * values. If the player is not using the shadow he gets more points */
enum { SCORE_DROP_DIVISOR             = 20 };
enum { SCORE_DROP_WITH_SHADOW_DIVISOR = 100 };

/* Number of filled rows required to increase the game level */
enum { FILLED_ROWS_FOR_LEVEL_UP = 10 };

/* The falling delay is multiplied and divided by
 * these factors with every level up */
enum { DELAY_FACTOR_FOR_LEVEL_UP  = 9 };
enum { DELAY_DIVISOR_FOR_LEVEL_UP = 10 };

/* Delayed autoshift initial delay */
enum { DAS_DELAY_TIMER = 200 };

/* Delayed autoshift timer for left and right moves */
enum { DAS_MOVE_TIMER  = 40 };

#ifdef STC_AUTO_ROTATION
/* Rotation auto-repeat delay */
enum { ROTATION_AUTOREPEAT_DELAY = 375 };

/* Rotation autorepeat timer */
enum { ROTATION_AUTOREPEAT_TIMER = 200 };
#endif /* STC_AUTO_ROTATION */

/*
 * Game constants.
 * You likely don't need to change this section unless you're changing the gameplay.
 */

/* Error codes */
enum { 
    ERROR_NONE         =  0,   /* Everything is OK, oh wonders!     */
    ERROR_PLAYER_QUITS =  1,   /* The player quits, our fail        */
    ERROR_NO_MEMORY    = -1,   /* Not enough memory                 */
    ERROR_NO_VIDEO     = -2,   /* Video system was not initialized  */
    ERROR_NO_IMAGES    = -3,   /* Problem loading the image files   */
    ERROR_ASSERT       = -100  /* Something went very very wrong... */
}; 

/* Game events */
enum {
    EVENT_NONE        = 0,
    EVENT_MOVE_DOWN   = 1,
    EVENT_MOVE_LEFT   = 1 << 1,
    EVENT_MOVE_RIGHT  = 1 << 2,
    EVENT_ROTATE_CW   = 1 << 3,  /* rotate clockwise           */
    EVENT_ROTATE_CCW  = 1 << 4,  /* rotate counter-clockwise   */
    EVENT_DROP        = 1 << 5,
    EVENT_PAUSE       = 1 << 6,
    EVENT_RESTART     = 1 << 7,
    EVENT_SHOW_NEXT   = 1 << 8,  /* toggle show next tetromino */
    EVENT_SHOW_SHADOW = 1 << 9,  /* toggle show shadow         */
    EVENT_QUIT        = 1 << 10  /* finish the game            */
};

/* We are going to store the tetromino cells in a square matrix */
/* of this size (this is the size of the biggest tetromino)     */
enum { TETROMINO_SIZE  = 4 };

/* Number of tetromino types */
enum { TETROMINO_TYPES = 7 };

/* Tetromino definitions.
 * They are indexes and must be between: 0 - [TETROMINO_TYPES - 1]
 * http://tetris.wikia.com/wiki/Tetromino
 * Initial cell disposition is commented below.
 */
enum { 
    /*
     *              ....
     *              ####
     *              ....
     *              ....
     */
    TETROMINO_I = 0,
    /*
     *              ##..
     *              ##..
     *              ....
     *              ....
     */
    TETROMINO_O = 1,
    /*
     *              .#..
     *              ###.
     *              ....
     *              ....
     */
    TETROMINO_T = 2,
    /*
     *              .##.
     *              ##..
     *              ....
     *              ....
     */
    TETROMINO_S = 3,
    /*
     *              ##..
     *              .##.
     *              ....
     *              ....
     */
    TETROMINO_Z = 4,
    /*
     *              #...
     *              ###.
     *              ....
     *              ....
     */
    TETROMINO_J = 5,
    /*
     *              ..#.
     *              ###.
     *              ....
     *              ....
     */
    TETROMINO_L = 6
};

/* Color indexes */
enum {
    COLOR_CYAN   = 1,
    COLOR_RED    = 2,
    COLOR_BLUE   = 3,
    COLOR_ORANGE = 4,
    COLOR_GREEN  = 5,
    COLOR_YELLOW = 6,
    COLOR_PURPLE = 7,
    COLOR_WHITE  = 0     /* Used for effects (if any) */
};

/* This value used for empty tiles */
enum { EMPTY_CELL = -1 };

/*
 * Data structure that holds information about our tetromino blocks.
 */
typedef struct StcTetromino {
    /*
     *  Tetromino buffer: [x][y]
     *  +---- x
     *  |
     *  |
     *  y
     */
    int cells[TETROMINO_SIZE][TETROMINO_SIZE];
    int x;
    int y;
    int size;
    int type;
} StcTetromino;

/* Game private data forward declaration */
typedef struct StcGamePrivate StcGamePrivate;

/*
 * Data structure that holds information about our game _object_.
 * With a little more of work we could create accessors for every
 * property this _object_ shares, thus avoiding the possibility 
 * of external code messing with our _object_ internal state, (as the 
 * C++ version shows using inline accessors and constant references).
 * For simplicity (and speed), I'm leaving it like this.
 */
typedef struct StcGame {

    /* Statistic data */
    struct {
        long score;         /* user score for current game      */
        int lines;          /* total number of lines cleared    */
        int totalPieces;    /* total number of tetrominoes used */
        int level;          /* current game level               */
        int pieces[TETROMINO_TYPES]; /* number of tetrominoes per type */
    } stats;

    /* Matrix that holds the cells (tilemap) */
    int map[BOARD_TILEMAP_WIDTH][BOARD_TILEMAP_HEIGHT];

    StcTetromino nextBlock;     /* next tetromino                */
    StcTetromino fallingBlock;  /* current falling tetromino     */

    int stateChanged;    /* 1 if game state changed, 0 otherwise */
    int errorCode;       /* game error code                      */
    int isPaused;        /* 1 if the game is paused, 0 otherwise */
    int showPreview;     /* 1 if we must show preview tetromino  */
#ifdef STC_SHOW_GHOST_PIECE
    int showShadow; /* 1 if we must show ghost shadow            */
    int shadowGap;  /* distance between falling block and shadow */
#endif

    StcPlatform      *platform; /* platform hidden implementation */
    StcGamePrivate   *data;     /* hidden game properties         */

} StcGame;


/*
 * Main game functions
 */
void gameInit(StcGame *game);
void gameUpdate(StcGame *game);
void gameEnd(StcGame *game);
void gameOnEventStart(StcGame *game, int command);
void gameOnEventEnd(StcGame *game, int command);

#endif /* STC_SRC_GAME_H_ */
