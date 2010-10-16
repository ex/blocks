/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Game class definition.                                                   */
/*                                                                            */
/*   Copyright (c) 2010 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#ifndef STC_SRC_GAME_HPP_
#define STC_SRC_GAME_HPP_

#include "platform.hpp"

/* Game name */
#define STC_GAME_NAME    "STC: simple tetris clone"

class Game {
  public:
    /*
     * Game configuration.
     * Edit this section to change the size, scores or pace of the game.
     */

    /* Playfield size (in tiles) */
    static const int BOARD_TILEMAP_WIDTH  = 10;
    static const int BOARD_TILEMAP_HEIGHT = 22;

    /* Initial time delay (in milliseconds) between falling moves */
    static const int INIT_DELAY_FALL = 1000;

    /* Score points given by filled rows (we use the original NES * 10)
     * http://tetris.wikia.com/wiki/Scoring */
    static const int SCORE_1_FILLED_ROW = 400;
    static const int SCORE_2_FILLED_ROW = 1000;
    static const int SCORE_3_FILLED_ROW = 3000;
    static const int SCORE_4_FILLED_ROW = 12000;

    /* Score points for hard drop. The player gets more score points
     * if he uses a hard drop. (these values divide SCORE_2_FILLED_ROW) */
    static const int SCORE_DROP_DIVISOR             = 20;
    static const int SCORE_DROP_WITH_SHADOW_DIVISOR = 100;

    /* Player gets points every time he accelerates downfall
     * (this value divides SCORE_2_FILLED_ROW */
    static const int SCORE_MOVE_DOWN_DIVISOR = 1000;

    /* Number of filled rows required to increase the game level */
    static const int FILLED_ROWS_FOR_LEVEL_UP = 10;

    /* The falling delay is multiplied and divided by
     * these factors with every level up */
    static const int DELAY_FACTOR_FOR_LEVEL_UP = 9;
    static const int DELAY_DIVISOR_FOR_LEVEL_UP = 10;

    /* Delayed autoshift initial delay */
    static const int DAS_DELAY_TIMER = 200;

    /* Delayed autoshift timer for left and right moves */
    static const int DAS_MOVE_TIMER = 40;

#ifdef STC_AUTO_ROTATION
    /* Rotation auto-repeat delay */
    static const int ROTATION_AUTOREPEAT_DELAY = 375;

    /* Rotation autorepeat timer */
    static const int ROTATION_AUTOREPEAT_TIMER = 200;
#endif /* STC_AUTO_ROTATION */

    /*
     * Game constants.
     * You likely don't need to change this section unless you're changing the gameplay.
     */
    static const int ERROR_NONE         =  0;    /* Everything is OK, oh wonders!    */
    static const int ERROR_PLAYER_QUITS =  1;    /* The player quits, our fail       */
    static const int ERROR_NO_VIDEO     = -2;    /* Video system was not initialized */
    static const int ERROR_NO_IMAGES    = -3;    /* Problem loading the image files  */
    static const int ERROR_ASSERT       = -100;  /* Something went very very wrong   */

    /*
     * Game events
     */
    static const int EVENT_NONE        = (0);
    static const int EVENT_MOVE_DOWN   = (1 << 1);
    static const int EVENT_MOVE_LEFT   = (1 << 2);
    static const int EVENT_MOVE_RIGHT  = (1 << 3);
    static const int EVENT_ROTATE_CW   = (1 << 4);   /* rotate clockwise         */
    static const int EVENT_ROTATE_CCW  = (1 << 5);   /* rotate counter-clockwise */
    static const int EVENT_DROP        = (1 << 6);
    static const int EVENT_PAUSE       = (1 << 7);
    static const int EVENT_RESTART     = (1 << 8);
    static const int EVENT_SHOW_NEXT   = (1 << 9);   /* toggle show next tetromino */
    static const int EVENT_SHOW_SHADOW = (1 << 10);  /* toggle show shadow         */
    static const int EVENT_QUIT        = (1 << 11);  /* finish the game            */

    /* We are going to store the tetromino cells in a square matrix
     * of this size (this is the size of the biggest tetromino) */
    static const int TETROMINO_SIZE  = 4;

    /* Number of tetromino types */
    static const int TETROMINO_TYPES = 7;

    /*
     * Tetromino definitions (they are indexes and must be between: 0 - [TETROMINO_TYPES - 1])
     * http://tetris.wikia.com/wiki/Tetromino
     *
            ....
            ####
            ....
            ....
     */
    static const int TETROMINO_I = 0;
    /*
            ##..
            ##..
            ....
            ....
     */
    static const int TETROMINO_O = 1;
    /*
            .#..
            ###.
            ....
            ....
     */
    static const int TETROMINO_T = 2;
    /*
            .##.
            ##..
            ....
            ....
     */
    static const int TETROMINO_S = 3;
    /*
            ##..
            .##.
            ....
            ....
     */
    static const int TETROMINO_Z = 4;
    /*
            #...
            ###.
            ....
            ....
     */
    static const int TETROMINO_J = 5;
    /*
            ..#.
            ###.
            ....
            ....
     */
    static const int TETROMINO_L = 6;

    /* Tetromino color indexes (must be between 0 - TETROMINO_TYPES) */
    static const int COLOR_CYAN   = 1;
    static const int COLOR_RED    = 2;
    static const int COLOR_BLUE   = 3;
    static const int COLOR_ORANGE = 4;
    static const int COLOR_GREEN  = 5;
    static const int COLOR_YELLOW = 6;
    static const int COLOR_PURPLE = 7;
    static const int COLOR_WHITE  = 0;     /* Used for effects (if any) */

    /* This value used for empty tiles */
    static const int EMPTY_CELL = -1;

    /* Data structure that holds information about our tetromino blocks. */
    struct StcTetromino {
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
    };

    /* Data structure for statistical data */
    struct StcStatics {
        long score;         /* user score for current game      */
        int lines;          /* total number of lines cleared    */
        int totalPieces;    /* total number of tetrominoes used */
        int level;          /* current game level               */
        int pieces[TETROMINO_TYPES];    /* number of tetrominoes per type */
    };

  public:

    /* Public property used to flag that the game state has changed */
    bool stateChanged;

    /* Return the cell at the specified position */
    int getCell(int column, int row) { return mMap[column][row]; }

    /* Return a reference to the sgame tatistic data */
    StcStatics const &stats() { return mStats; }

    /* Return current falling tetromino */
    StcTetromino const &fallingBlock() { return mFallingBlock; }

    /* Return next tetromino */
    StcTetromino const &nextBlock() { return mNextBlock; }

    /* Return current error code */    
    int errorCode()     { return mErrorCode; } 

    /* Return true if the game is paused, false otherwise */
    bool isPaused()     { return mIsPaused; }

    /* Return true if we must show preview tetromino */    
    bool showPreview()  { return mShowPreview; }

#ifdef STC_SHOW_GHOST_PIECE
    /* Return true if we must show ghost shadow */
    bool showShadow()   { return mShowShadow; }

    /* Return height gap between shadow and falling tetromino */
    int shadowGap()     { return mShadowGap; }
#endif

    void init(Platform *targetPlatform);
    void end();
    void update();
    void onKeyDown(int command);
    void onKeyUp(int command);

  private:

    /* Game events are stored in bits in this variable.
     * It must be cleared to EVENT_NONE after being used */
    int mEvents;

    /* Matrix that holds the cells (tilemap) */
    int mMap[BOARD_TILEMAP_WIDTH][BOARD_TILEMAP_HEIGHT];

    StcStatics mStats;           /* statistic data */
    StcTetromino mFallingBlock;  /* current falling tetromino */
    StcTetromino mNextBlock;     /* next tetromino            */

    int mErrorCode;
    bool mIsPaused;
    bool mShowPreview;
#ifdef STC_SHOW_GHOST_PIECE
    bool mShowShadow;
    int mShadowGap;
#endif

    Platform *mPlatform; /* platform interface           */
    long mSystemTime;    /* system time in milliseconds  */
    int mFallingDelay;   /* delay time for falling tetrominoes  */
    bool mIsOver;        /* true if the game is over, false otherwise      */
    long mLastFallTime;  /* last time the game moved the falling tetromino */

    /* For delayed autoshift: http://tetris.wikia.com/wiki/DAS */
    int mDelayLeft;
    int mDelayRight;
    int mDelayDown;
#ifdef STC_AUTO_ROTATION
    int mDelayRotation;
#endif

    void setMatrixCells(int *matrix, int width, int height, int value);
    void setTetromino(int indexTetromino, StcTetromino *tetromino);
    void start();
    void rotateTetromino(bool clockwise);
    bool checkCollision(int dx, int dy);
    void onFilledRows(int filledRows);
    void moveTetromino(int x, int y);
    void dropTetromino();
    void onTetrominoMoved();
};

#endif /* STC_SRC_GAME_HPP_ */
