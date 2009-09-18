/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Game constants and definitions.                                          */
/*                                                                            */
/*   Copyright (c) 2009 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#ifndef STC_SRC_GAME_H_
#define STC_SRC_GAME_H_

#include "defines.h"

/*
 * Game error codes
 */
#define GAME_ERROR_NONE         (0)     /* Everything is OK, oh wonders!      */
#define GAME_ERROR_USER_QUITS   (1)     /* The user quits (bored?), our fail  */
#define GAME_ERROR_NO_MEMORY    (-1)    /* Not enough memory                  */
#define GAME_ERROR_NO_VIDEO     (-2)    /* Video system was not initialized   */
#define GAME_ERROR_NO_IMAGES    (-3)    /* Problem loading the image files    */
#define GAME_ERROR_ASSERT       (-100)  /* Something went very very wrong...  */

/* 
 * Game events
 */
#define EVENT_NONE          (0)
#define EVENT_MOVE_DOWN     (1 << 1)
#define EVENT_MOVE_LEFT     (1 << 2)
#define EVENT_MOVE_RIGHT    (1 << 3)
#define EVENT_ROTATE_CW     (1 << 4)    /* rotate clockwise         */
#define EVENT_ROTATE_CCW    (1 << 5)    /* rotate counter-clockwise */
#define EVENT_DROP          (1 << 6)
#define EVENT_PAUSE         (1 << 7)
#define EVENT_RESTART       (1 << 8)
#define EVENT_SHOW_NEXT     (1 << 9)    /* toggle show next tetromino */
#define EVENT_SHOW_SHADOW   (1 << 10)   /* toggle show shadow */

/* Number of tetromino types */
#define TETROMINO_TYPES (7)

/* We are going to store the tetromino cells in a square matrix */
/* of this size (this is the size of the biggest tetromino)     */
#define TETROMINO_SIZE (4)

/* Tetromino definitions (used as indexes: must be between 0 - [TETROMINO_TYPES - 1])
 * http://tetris.wikia.com/wiki/Tetromino */
#define TETROMINO_I     (0)
#define TETROMINO_O     (1)
#define TETROMINO_T     (2)
#define TETROMINO_S     (3)
#define TETROMINO_Z     (4)
#define TETROMINO_J     (5)
#define TETROMINO_L     (6)

/* Tetromino color indexes (must be between 0 - TETROMINO_TYPES) */
#define COLOR_CYAN      (1)
#define COLOR_RED       (2)
#define COLOR_BLUE      (3)
#define COLOR_ORANGE    (4)
#define COLOR_GREEN     (5)
#define COLOR_YELLOW    (6)
#define COLOR_PURPLE    (7)
#define COLOR_WHITE     (0)     /* Used for effects (if any) */

/* This value used for empty tiles */
#define EMPTY_CELL  (-1)    

/*
 * Data structure that holds information about our tetromino blocks
 */
typedef struct StcTetromino {
    int cells[TETROMINO_SIZE][TETROMINO_SIZE];
    int x;
    int y;
    int size;
    int type;
} StcTetromino;

/*
 * Data structure that is going to hold platform dependent
 * information about our game. It's defined in the platform header.
 */
typedef struct StcPlatform StcPlatform;

/*
 * Data structure that holds information about our game *object*
 */
typedef struct StcGame {
    /*
     * Matrix that holds the cells (tilemap)
     */
    int map[BOARD_WIDTH][BOARD_HEIGHT];

    StcTetromino nextBlock;     /* next tetromino               */
    StcTetromino fallingBlock;  /* current falling tetromino    */
    StcPlatform *platform;      /* platform hidden data         */
    int errorCode;              /* game error code              */
    long systemTime;            /* system time in milliseconds  */
    int delay;          /* delay time for falling tetrominoes   */
    int isOver;         /* 1 if the game is over, 0 otherwise   */
    int isPaused;       /* 1 if the game is paused, 0 otherwise */
    int showPreview;    /* 1 if we must show preview tetromino  */
    long lastFallTime;  /* last time the game moved the falling tetromino */
    int stateChanged;   /* 1 if game state changed, 0 otherwise */
    int scoreChanged;   /* 1 if game score changed, 0 otherwise */

#ifdef STC_SHOW_GHOST_PIECE
    int showShadow;     /* 1 if we must show ghost shadow       */
    int shadowGap;      /* height gap between shadow and falling tetromino */
#endif

    /*
     * Game events are stored in bits in this variable.
     * must be cleared to EVENT_NONE after being used.
     */
    int events;

    /*
     * Statistic data
     */
    struct {
        long score;         /* user score for current game      */
        int lines;          /* total number of lines cleared    */
        int totalPieces;    /* total number of tetrominoes used */
        int level;          /* current game level               */
        int pieces[TETROMINO_TYPES];    /* number of tetrominoes per type */
    } stats;
} StcGame;

/*
 * Creates a game and returns a pointer to a valid game object
 * or a NULL pointer in case of error.
 */
StcGame *createGame();

/*
 * Release resources used by the game.
 */
void deleteGame(StcGame *pGame);

/*
 * Main game functions
 */
int gameInit(StcGame *gameInstance);
void gameEnd(StcGame *gameInstance);
void gameUpdate(StcGame *gameInstance);

/*
 * External interface for platform implementation
 */
extern int platformInit(StcGame *gameInstance);
extern void platformEnd(StcGame *gameInstance);

/* Read input device and notify game */
extern void platformReadInput(StcGame *gameInstance);

/* Render the state of the game */
extern void platformRenderGame(StcGame *gameInstance);

/* Return the current system time in milliseconds */
extern long platformGetSystemTime();

#endif /* STC_SRC_GAME_H_ */
