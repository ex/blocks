/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Game class definition.                                                   */
/*                                                                            */
/*   Copyright (c) 2013 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#ifndef STC_SRC_GAME_HPP_
#define STC_SRC_GAME_HPP_

#include "platform.hpp"

// Game name
#define STC_GAME_NAME    "STC: simple tetris clone"

namespace stc
{

class Game 
{
public:
    //--------------------------------------------------------------------------
    // Game configuration.
    // Edit this section to change the sizes, scores or pace of the game.

    // Playfield size (in tiles)
    static const int BOARD_TILEMAP_WIDTH  = 8;
    static const int BOARD_TILEMAP_HEIGHT = 24;

    // Initial time delay (in milliseconds) between falling moves.
    static const int INIT_DELAY_FALL = 1000;

    // Score points given by filled rows (we use the original NES * 10)
    // http://tetris.wikia.com/wiki/Scoring
    static const int SCORE_1_FILLED_ROW = 400;
    static const int SCORE_2_FILLED_ROW = 1000;
    static const int SCORE_3_FILLED_ROW = 3000;
    static const int SCORE_4_FILLED_ROW = 12000;

    // The player gets points every time he accelerates downfall.
    // The added points are equal to SCORE_2_FILLED_ROW divided by this value.
    static const int SCORE_MOVE_DOWN_DIVISOR = 1000;

    // The player gets points every time he does a hard drop.
    // The added points are equal to SCORE_2_FILLED_ROW divided by these
    // values. If the player is not using the shadow he gets more points.
    static const int SCORE_DROP_DIVISOR             = 20;
    static const int SCORE_DROP_WITH_SHADOW_DIVISOR = 100;

    // Number of filled rows required to increase the game level.
    static const int FILLED_ROWS_FOR_LEVEL_UP = 10;

    // The falling delay is multiplied and divided by
    // these factors with every level up.
    static const int DELAY_FACTOR_FOR_LEVEL_UP = 9;
    static const int DELAY_DIVISOR_FOR_LEVEL_UP = 10;

    // Delayed autoshift initial delay.
    static const int DAS_DELAY_TIMER = 200;

    // Delayed autoshift timer for left and right moves.
    static const int DAS_MOVE_TIMER = 40;

#ifdef STC_AUTO_ROTATION
    // Rotation auto-repeat delay.
    static const int ROTATION_AUTOREPEAT_DELAY = 375;

    // Rotation autorepeat timer.
    static const int ROTATION_AUTOREPEAT_TIMER = 200;
#endif // STC_AUTO_ROTATION

    //--------------------------------------------------------------------------
    // Game constants. You likely don't need to change this section unless 
    // you're changing the gameplay.

    // Error codes
    enum
    { 
        ERROR_NONE         =  0,   // Everything is OK, oh wonders!
        ERROR_PLAYER_QUITS =  1,   // The user quits, our fail
        ERROR_NO_MEMORY    = -1,   // Not enough memory
        ERROR_NO_VIDEO     = -2,   // Video system was not initialized
        ERROR_NO_IMAGES    = -3,   // Problem loading the image files
        ERROR_PLATFORM     = -4,   // Problem creating platform
        ERROR_ASSERT       = -100  // Something went very very wrong...
    }; 

    // Game events
    enum {
        EVENT_NONE        = 0,
        EVENT_MOVE_DOWN   = 1,
        EVENT_MOVE_LEFT   = 1 << 1,
        EVENT_MOVE_RIGHT  = 1 << 2,
        EVENT_ROTATE_CW   = 1 << 3,  // rotate clockwise
        EVENT_ROTATE_CCW  = 1 << 4,  // rotate counter-clockwise
        EVENT_DROP        = 1 << 5,
        EVENT_PAUSE       = 1 << 6,
        EVENT_RESTART     = 1 << 7,
        EVENT_SHOW_NEXT   = 1 << 8,  // toggle show next tetromino
        EVENT_SHOW_SHADOW = 1 << 9,  // toggle show shadow
        EVENT_QUIT        = 1 << 10  // finish the game
    };

    // We are going to store the tetromino cells in a square matrix
    // of this size (this is the size of the biggest tetromino)
    static const int TETROMINO_SIZE  = 4;

    // Number of tetromino types.
    static const int TETROMINO_TYPES = 7;

    // Tetromino definitions.
    // They are indexes and must be between: 0 - [TETROMINO_TYPES - 1]
    // http://tetris.wikia.com/wiki/Tetromino
    // Initial cell disposition is commented below.
    enum
    { 
        //
        //              ....
        //              ####
        //              ....
        //              ....
        //
        TETROMINO_I = 0,
        //
        //              ##..
        //              ##..
        //              ....
        //              ....
        //
        TETROMINO_O = 1,
        //
        //              .#..
        //              ###.
        //              ....
        //              ....
        //
        TETROMINO_T = 2,
        //
        //              .##.
        //              ##..
        //              ....
        //              ....
        //
        TETROMINO_S = 3,
        //
        //              ##..
        //              .##.
        //              ....
        //              ....
        //
        TETROMINO_Z = 4,
        //
        //              #...
        //              ###.
        //              ....
        //              ....
        //
        TETROMINO_J = 5,
        //
        //              ..#.
        //              ###.
        //              ....
        //              ....
        //
        TETROMINO_L = 6
    };

    // Color indexes
    enum
    {
        COLOR_CYAN   = 1,
        COLOR_RED    = 2,
        COLOR_BLUE   = 3,
        COLOR_ORANGE = 4,
        COLOR_GREEN  = 5,
        COLOR_YELLOW = 6,
        COLOR_PURPLE = 7,
        COLOR_WHITE  = 0  // Used for effects (if any)
    };

    // This value used for empty tiles
    static const int EMPTY_CELL = -1;

    // Data structure that holds information about our tetromino blocks.
    struct StcTetromino
    {
        //
        //  Tetromino buffer: [x][y]
        //  +---- x
        //  |
        //  |
        //  y
        //
        int cells[TETROMINO_SIZE][TETROMINO_SIZE];
        int x;
        int y;
        int size;
        int type;
    };

    // Data structure for statistical data
    struct StcStatics
    {
        long score;         // user score for current game
        int lines;          // total number of lines cleared
        int totalPieces;    // total number of tetrominoes used
        int level;          // current game level
        int pieces[TETROMINO_TYPES]; // number of tetrominoes per type
    };

    // The platform must call this method after processing a changed state
    void onChangeProcessed()           { mStateChanged = false; }

    // Return true if the game state has changed, false otherwise
    bool hasChanged()                  { return mStateChanged; }

    // Return the cell at the specified position
    int getCell(int column, int row)   { return mMap[column][row]; }

    // Return a reference to the game statistic data
    StcStatics const &stats()          { return mStats; }

    // Return current falling tetromino
    StcTetromino const &fallingBlock() { return mFallingBlock; }

    // Return next tetromino
    StcTetromino const &nextBlock()    { return mNextBlock; }

    // Return current error code
    int errorCode()     { return mErrorCode; } 

    // Return true if the game is paused, false otherwise
    bool isPaused()     { return mIsPaused; }

    // Return true if the game has finished, false otherwise
    bool isOver()       { return mIsOver; }
	
    // Return true if we must show preview tetromino
    bool showPreview()  { return mShowPreview; }

#ifdef STC_SHOW_GHOST_PIECE
    // Return true if we must show ghost shadow
    bool showShadow()   { return mShowShadow; }

    // Return height gap between shadow and falling tetromino
    int shadowGap()     { return mShadowGap; }
#endif

    void init(Platform *targetPlatform);
    void end();
    void update();
    void onEventStart(int event);
    void onEventEnd(int event);

private:

    // Game events are stored in bits in this variable.
    // It must be cleared to EVENT_NONE after being used.
    unsigned int mEvents;

    // Matrix that holds the cells (tilemap)
    int mMap[BOARD_TILEMAP_WIDTH][BOARD_TILEMAP_HEIGHT];

    Platform    *mPlatform;     // platform interface
    StcStatics   mStats;        // statistic data
    StcTetromino mFallingBlock; // current falling tetromino
    StcTetromino mNextBlock;    // next tetromino

    bool mStateChanged; // true if game state has changed
    int  mErrorCode;    // stores current error code
    bool mIsPaused;     // true if the game is over
    bool mIsOver;       // true if the game is over
    bool mShowPreview;  // true if we must show the preview block

#ifdef STC_SHOW_GHOST_PIECE
    bool mShowShadow; // true if we must show the shadow block
    int  mShadowGap;  // distance between falling block and shadow
#endif

    long mSystemTime;   // system time in milliseconds
    int  mFallingDelay; // delay time for falling tetrominoes
    long mLastFallTime; // last time the falling tetromino dropped

    // For delayed autoshift: http://tetris.wikia.com/wiki/DAS
    int  mDelayLeft;
    int  mDelayRight;
    int  mDelayDown;
#ifdef STC_AUTO_ROTATION
    int  mDelayRotation;
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
}

#endif // STC_SRC_GAME_HPP_
