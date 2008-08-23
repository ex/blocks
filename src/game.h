/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Game functions.                                                          */
/*                                                                            */
/*   Copyright (C) 2008 Laurens Rodriguez Oscanoa                             */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#ifndef STC_SRC_GAME_H_
#define STC_SRC_GAME_H_

#include "defines.h"

/*
 * Game error codes
 */
#define GAME_ERROR_NONE         (0)     /* Everything is OK, wonders!         */
#define GAME_ERROR_NO_MEMORY    (1)     /* Not enough memory                  */
#define GAME_ERROR_NO_VIDEO     (2)     /* Video system was not initialized   */
#define GAME_ERROR_NO_IMAGES    (3)     /* Problem loading the image files    */
#define GAME_ERROR_USER_QUITS   (10)    /* If the user quits... :D            */

#define EMPTY_CELL  (-1)    /* This value used for empty tiles */

/*
 * Data structure that holds information about our tetramino blocks
 */
typedef struct StcTetramino {
    int cells[4][4];
    int x;
    int y;
    int size;
} StcTetramino;

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
     * Matrix that holds the tile map data
     */
    int map[BOARD_WIDTH][BOARD_HEIGHT + 1];

    int errorCode;              /* game error code              */
    StcPlatform *platform;      /* platform data                */
    StcTetramino nextBlock;     /* next tetromino               */
    StcTetramino fallingBlock;  /* current falling tetromino    */

    int hasStarted;
    long systemTime;
    long gameTime;

    /*
     * Statistic data
     */
    long score;
    int lines;
    int pieces;
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

/* Rotate the tetramino that is falling */
void rotateTetramino(StcGame *gameInstance, int clockwise);

/* Check collision of tetromino */
int checkCollision(StcGame *gameInstance, int nx, int ny);

/* Move tetramino */
void moveTetramino(StcGame *gameInstance, int x, int y);

/* Drop tetramino */
void dropTetramino(StcGame *gameInstance);


/*
 * External interface for platform implementation
 */
extern int platformInit(StcGame *gameInstance);
extern void platformEnd(StcGame *gameInstance);
extern void gameReadInput(StcGame *gameInstance);
extern void gameRender(StcGame *gameInstance);
extern long getSystemTime();


#endif /* STC_SRC_GAME_H_ */
