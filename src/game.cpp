/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Game logic implementation.                                               */
/*                                                                            */
/*   Copyright (c) 2012 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#include "game.hpp"
#include <stdlib.h>

namespace stc
{

// Set matrix elements to indicated value
void Game::setMatrixCells(int *matrix, int width, int height, int value)
{
    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            *(matrix + i + (j * width)) = value;
        }
    }
}

// Initialize tetromino cells for every type of tetromino
void Game::setTetromino(int indexTetromino, StcTetromino *tetromino)
{
    // Initialize tetromino cells to empty cells
    setMatrixCells(&tetromino->cells[0][0], TETROMINO_SIZE, TETROMINO_SIZE, EMPTY_CELL);

    // Almost all the blocks have size 3
    tetromino->size = TETROMINO_SIZE - 1;

    // Initial configuration from: http://tetris.wikia.com/wiki/SRS
    switch (indexTetromino)
    {
    case TETROMINO_I:
        tetromino->cells[0][1] = COLOR_CYAN;
        tetromino->cells[1][1] = COLOR_CYAN;
        tetromino->cells[2][1] = COLOR_CYAN;
        tetromino->cells[3][1] = COLOR_CYAN;
        tetromino->size = TETROMINO_SIZE;
        break;
    case TETROMINO_O:
        tetromino->cells[0][0] = COLOR_YELLOW;
        tetromino->cells[0][1] = COLOR_YELLOW;
        tetromino->cells[1][0] = COLOR_YELLOW;
        tetromino->cells[1][1] = COLOR_YELLOW;
        tetromino->size = TETROMINO_SIZE - 2;
        break;
    case TETROMINO_T:
        tetromino->cells[0][1] = COLOR_PURPLE;
        tetromino->cells[1][0] = COLOR_PURPLE;
        tetromino->cells[1][1] = COLOR_PURPLE;
        tetromino->cells[2][1] = COLOR_PURPLE;
        break;
    case TETROMINO_S:
        tetromino->cells[0][1] = COLOR_GREEN;
        tetromino->cells[1][0] = COLOR_GREEN;
        tetromino->cells[1][1] = COLOR_GREEN;
        tetromino->cells[2][0] = COLOR_GREEN;
        break;
    case TETROMINO_Z:
        tetromino->cells[0][0] = COLOR_RED;
        tetromino->cells[1][0] = COLOR_RED;
        tetromino->cells[1][1] = COLOR_RED;
        tetromino->cells[2][1] = COLOR_RED;
        break;
    case TETROMINO_J:
        tetromino->cells[0][0] = COLOR_BLUE;
        tetromino->cells[0][1] = COLOR_BLUE;
        tetromino->cells[1][1] = COLOR_BLUE;
        tetromino->cells[2][1] = COLOR_BLUE;
        break;
    case TETROMINO_L:
        tetromino->cells[0][1] = COLOR_ORANGE;
        tetromino->cells[1][1] = COLOR_ORANGE;
        tetromino->cells[2][0] = COLOR_ORANGE;
        tetromino->cells[2][1] = COLOR_ORANGE;
        break;
    }
    tetromino->type = indexTetromino;
}

// Start a new game
void Game::start()
{
    // Initialize game data
    mErrorCode = ERROR_NONE;
    mSystemTime = mPlatform->getSystemTime();
    mLastFallTime = mSystemTime;
    mIsOver = false;
    mIsPaused = false;
    mShowPreview = true;
    mEvents = EVENT_NONE;
    mFallingDelay = INIT_DELAY_FALL;
#ifdef STC_SHOW_GHOST_PIECE
    mShowShadow = true;
#endif

    // Initialize game statistics
    mStats.score = 0;
    mStats.lines = 0;
    mStats.totalPieces = 0;
    mStats.level = 0;
    for (int i = 0; i < TETROMINO_TYPES; ++i)
    {
        mStats.pieces[i] = 0;
    }

    // Initialize game tile map
    setMatrixCells(&mMap[0][0], BOARD_TILEMAP_WIDTH, BOARD_TILEMAP_HEIGHT, EMPTY_CELL);

    // Initialize falling tetromino
    setTetromino(mPlatform->random() % TETROMINO_TYPES, &mFallingBlock);
    mFallingBlock.x = (BOARD_TILEMAP_WIDTH - mFallingBlock.size) / 2;
    mFallingBlock.y = 0;

    // Initialize preview tetromino
    setTetromino(mPlatform->random() % TETROMINO_TYPES, &mNextBlock);

    // Initialize events
    onTetrominoMoved();

    // Initialize delayed autoshift
    mDelayLeft = -1;
    mDelayRight = -1;
    mDelayDown = -1;
#ifdef STC_AUTO_ROTATION
    mDelayRotation = -1;
#endif
}

// Initialize the game. The error code (if any) is saved in [mErrorcode].
void Game::init(Platform *targetPlatform)
{
    // Store platform reference and start it
    mPlatform = targetPlatform;

    // Initialize platform
    mErrorCode = mPlatform->init(this);

    if (mErrorCode == ERROR_NONE)
    {
        // If everything is OK start the game
        start();
    }
};

// Free used resources
void Game::end()
{
    mPlatform->end();
}

// Rotate falling tetromino. If there are no collisions when the
// tetromino is rotated this modifies the tetromino's cell buffer.
void Game::rotateTetromino(bool clockwise)
{
    int i, j;
    int rotated[TETROMINO_SIZE][TETROMINO_SIZE];  // temporary array to hold rotated cells

    // If TETROMINO_O is falling return immediately
    if (mFallingBlock.type == TETROMINO_O)
    {
        return; // rotation doesn't require any changes
    }

    // Initialize rotated cells to blank
    setMatrixCells(&rotated[0][0], TETROMINO_SIZE, TETROMINO_SIZE, EMPTY_CELL);

    // Copy rotated cells to the temporary array
    for (i = 0; i < mFallingBlock.size; ++i)
    {
        for (j = 0; j < mFallingBlock.size; ++j)
        {
            if (clockwise)
            {
                rotated[mFallingBlock.size - j - 1][i] = mFallingBlock.cells[i][j];
            }
            else
            {
                rotated[j][mFallingBlock.size - i - 1] = mFallingBlock.cells[i][j];
            }
        }
    }
#ifdef STC_WALL_KICK_ENABLED
    int wallDisplace = 0;

    // Check collision with left wall
    if (mFallingBlock.x < 0)
    {
        for (i = 0; (wallDisplace == 0) && (i < -mFallingBlock.x); ++i)
        {
            for (j = 0; j < mFallingBlock.size; ++j)
            {
                if (rotated[i][j] != EMPTY_CELL)
                {
                    wallDisplace = i - mFallingBlock.x;
                    break;
                }
            }
        }
    }
    // Or check collision with right wall
    else if (mFallingBlock.x > BOARD_TILEMAP_WIDTH - mFallingBlock.size)
    {
        i = mFallingBlock.size - 1;
        for (; (wallDisplace == 0) && (i >= BOARD_TILEMAP_WIDTH - mFallingBlock.x); --i)
        {
            for (j = 0; j < mFallingBlock.size; ++j)
            {
                if (rotated[i][j] != EMPTY_CELL)
                {
                    wallDisplace = -mFallingBlock.x - i + BOARD_TILEMAP_WIDTH - 1;
                    break;
                }
            }
        }
    }

    // Check collision with board floor and other cells on board
    for (i = 0; i < mFallingBlock.size; ++i)
    {
        for (j = 0; j < mFallingBlock.size; ++j)
        {
            if (rotated[i][j] != EMPTY_CELL)
            {
                // Check collision with bottom border of the map
                if (mFallingBlock.y + j >= BOARD_TILEMAP_HEIGHT)
                {
                    return; // there was collision therefore return
                }

                // Check collision with existing cells in the map
                if (mMap[i + mFallingBlock.x + wallDisplace][j + mFallingBlock.y] != EMPTY_CELL)
                {
                    return; // there was collision therefore return
                }
            }
        }
    }

    // Move the falling piece if there was wall collision and it's a legal move
    if (wallDisplace != 0)
    {
        mFallingBlock.x += wallDisplace;
    }
#else
    // Check collision of the temporary array
    for (i = 0; i < mFallingBlock.size; ++i)
    {
        for (j = 0; j < mFallingBlock.size; ++j)
        {
            if (rotated[i][j] != EMPTY_CELL)
            {
                // Check collision with left, right or bottom borders of the map
                if ((mFallingBlock.x + i < 0) 
                        || (mFallingBlock.x + i >= BOARD_TILEMAP_WIDTH)
                        || (mFallingBlock.y + j >= BOARD_TILEMAP_HEIGHT))
                {
                    return; // there was collision therefore return
                }

                // Check collision with existing cells in the map
                if (mMap[i + mFallingBlock.x][j + mFallingBlock.y] != EMPTY_CELL)
                {
                    return; // there was collision therefore return
                }
            }
        }
    }
#endif // STC_WALL_KICK_ENABLED

    // There are no collisions, replace tetromino cells with rotated cells
    for (i = 0; i < TETROMINO_SIZE; ++i)
    {
        for (j = 0; j < TETROMINO_SIZE; ++j)
        {
            mFallingBlock.cells[i][j] = rotated[i][j];
        }
    }
    onTetrominoMoved();
}

// Check if tetromino will collide with something if it is moved in the requested direction.
// If there are collisions returns 1 else returns 0.
bool Game::checkCollision(int dx, int dy)
{

    int newx = mFallingBlock.x + dx;
    int newy = mFallingBlock.y + dy;

    for (int i = 0; i < mFallingBlock.size; ++i)
    {
        for (int j = 0; j < mFallingBlock.size; ++j)
        {
            if (mFallingBlock.cells[i][j] != EMPTY_CELL)
            {
                // Check the tetromino would be inside the left, right and bottom borders
                if ((newx + i < 0) || (newx + i >= BOARD_TILEMAP_WIDTH)
                                   || (newy + j >= BOARD_TILEMAP_HEIGHT))
                {
                    return true;
                }

                // Check the tetromino won't collide with existing cells in the map
                if (mMap[newx + i][newy + j] != EMPTY_CELL)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

// Game scoring: http://tetris.wikia.com/wiki/Scoring
void Game::onFilledRows(int filledRows)
{
    // Update total number of filled rows
    mStats.lines += filledRows;

    // Increase score accordingly to the number of filled rows
    switch (filledRows)
    {
    case 1:
        mStats.score += (SCORE_1_FILLED_ROW * (mStats.level + 1));
        break;
    case 2:
        mStats.score += (SCORE_2_FILLED_ROW * (mStats.level + 1));
        break;
    case 3:
        mStats.score += (SCORE_3_FILLED_ROW * (mStats.level + 1));
        break;
    case 4:
        mStats.score += (SCORE_4_FILLED_ROW * (mStats.level + 1));
        break;
    default:
        // This shouldn't happen, but if happens kill the game
        mErrorCode = ERROR_ASSERT;
    }

    // Check if we need to update the level
    if (mStats.lines >= FILLED_ROWS_FOR_LEVEL_UP * (mStats.level + 1))
    {
        mStats.level++;

        // Increase speed for falling tetrominoes
        mFallingDelay = (int)(DELAY_FACTOR_FOR_LEVEL_UP * mFallingDelay 
                              / DELAY_DIVISOR_FOR_LEVEL_UP);
    }
}

// Move tetromino in the direction specified by (x, y) (in tile units)
// This function detects if there are filled rows or if the move
// lands a falling tetromino, also checks for game over condition.
void Game::moveTetromino(int x, int y)
{
    int i, j;

    // Check if the move would create a collision
    if (checkCollision(x, y))
    {
        // In case of collision check if move was downwards (y == 1)
        if (y == 1)
        {
            // Check if collision occurs when the falling
            // tetromino is on the 1st or 2nd row
            if (mFallingBlock.y <= 1)
            {
                mIsOver = true; // if this happens the game is over
            }
            else
            {
                // The falling tetromino has reached the bottom,
                // so we copy their cells to the board map
                for (i = 0; i < mFallingBlock.size; ++i)
                {
                    for (j = 0; j < mFallingBlock.size; ++j)
                    {
                        if (mFallingBlock.cells[i][j] != EMPTY_CELL)
                        {
                            mMap[mFallingBlock.x + i][mFallingBlock.y + j]
                                    = mFallingBlock.cells[i][j];
                        }
                    }
                }

                // Check if the landing tetromino has created full rows
                int numFilledRows = 0;
                for (j = 1; j < BOARD_TILEMAP_HEIGHT; ++j)
                {
                    bool hasFullRow = true;
                    for (i = 0; i < BOARD_TILEMAP_WIDTH; ++i)
                    {
                        if (mMap[i][j] == EMPTY_CELL)
                        {
                            hasFullRow = false;
                            break;
                        }
                    }

                    // If we found a full row we need to remove that row from the map
                    // we do that by just moving all the above rows one row below
                    if (hasFullRow)
                    {
                        for (x = 0; x < BOARD_TILEMAP_WIDTH; ++x)
                        {
                            for (y = j; y > 0; --y)
                            {
                                mMap[x][y] = mMap[x][y - 1];
                            }
                        }
                        numFilledRows++; // increase filled row counter
                    }
                }

                // Update game statistics
                if (numFilledRows > 0)
                {
                    onFilledRows(numFilledRows);
                }
                mStats.totalPieces++;
                mStats.pieces[mFallingBlock.type]++;

                // Use preview tetromino as falling tetromino.
                // Copy preview tetromino for falling tetromino
                for (i = 0; i < TETROMINO_SIZE; ++i)
                {
                    for (j = 0; j < TETROMINO_SIZE; ++j)
                    {
                        mFallingBlock.cells[i][j] = mNextBlock.cells[i][j];
                    }
                }
                mFallingBlock.size = mNextBlock.size;
                mFallingBlock.type = mNextBlock.type;

                // Reset position
                mFallingBlock.y = 0;
                mFallingBlock.x = (BOARD_TILEMAP_WIDTH - mFallingBlock.size) / 2;
                onTetrominoMoved();

                // Create next preview tetromino
                setTetromino(mPlatform->random() % TETROMINO_TYPES, &mNextBlock);
            }
        }
    }
    else
    {
        // There are no collisions, just move the tetromino
        mFallingBlock.x += x;
        mFallingBlock.y += y;
    }
    onTetrominoMoved();
}

// Hard drop
void Game::dropTetromino()
{
#ifdef STC_SHOW_GHOST_PIECE
    // Shadow has already calculated the landing position.
    mFallingBlock.y += mShadowGap;

    // Force lock.
    moveTetromino(0, 1); 

    // Update score
    if (mShowShadow)
    {
        mStats.score += (long)(SCORE_2_FILLED_ROW * (mStats.level + 1) 
                               / SCORE_DROP_WITH_SHADOW_DIVISOR);
    }
    else
    {
        mStats.score += (long)(SCORE_2_FILLED_ROW * (mStats.level + 1)
                               / SCORE_DROP_DIVISOR);
    }
#else
    int y = 0;
    // Calculate number of cells to drop
    while (!checkCollision(0, ++y));
    moveTetromino(0, y - 1);
    moveTetromino(0, 1); // Force lock

    // Update score
    mStats.score += (long)(SCORE_2_FILLED_ROW * (mStats.level + 1) 
                           / SCORE_DROP_DIVISOR);
#endif
}

// Main function game called every frame
void Game::update()
{
    // Read player input
    mPlatform->processEvents();

    // Update game state
    if (mIsOver)
    {
        if ((mEvents & EVENT_RESTART) != 0)
        {
            mIsOver = false;
            start();
        }
    }
    else
    {
		// Always handle restart event
        if ((mEvents & EVENT_RESTART) != 0)
        {
            start();
			return;
        }
		
        long currentTime = mPlatform->getSystemTime();

        // Process delayed autoshift
        int timeDelta = (int)(currentTime - mSystemTime);
        if (mDelayDown > 0)
        {
            mDelayDown -= timeDelta;
            if (mDelayDown <= 0)
            {
                mDelayDown = DAS_MOVE_TIMER;
                mEvents |= EVENT_MOVE_DOWN;
            }
        }
        if (mDelayLeft > 0)
        {
            mDelayLeft -= timeDelta;
            if (mDelayLeft <= 0)
            {
                mDelayLeft = DAS_MOVE_TIMER;
                mEvents |= EVENT_MOVE_LEFT;
            }
        }
        else if (mDelayRight > 0)
        {
            mDelayRight -= timeDelta;
            if (mDelayRight <= 0)
            {
                mDelayRight = DAS_MOVE_TIMER;
                mEvents |= EVENT_MOVE_RIGHT;
            }
        }
#ifdef STC_AUTO_ROTATION
        if (mDelayRotation > 0)
        {
            mDelayRotation -= timeDelta;
            if (mDelayRotation <= 0)
            {
                mDelayRotation = ROTATION_AUTOREPEAT_TIMER;
                mEvents |= EVENT_ROTATE_CW;
            }
        }
#endif // STC_AUTO_ROTATION

        // Always handle pause event
        if ((mEvents & EVENT_PAUSE) != 0)
        {
            mIsPaused = !mIsPaused;
            mEvents = EVENT_NONE;
        }

        // Check if the game is paused
        if (mIsPaused)
        {
            // We achieve the effect of pausing the game
            // adding the last frame duration to lastFallTime
            mLastFallTime += (currentTime - mSystemTime);
        }
        else
        {
            if (mEvents != EVENT_NONE)
            {
                if ((mEvents & EVENT_SHOW_NEXT) != 0)
                {
                    mShowPreview = !mShowPreview;
                    mStateChanged = true;
                }
#ifdef STC_SHOW_GHOST_PIECE
                if ((mEvents & EVENT_SHOW_SHADOW) != 0)
                {
                    mShowShadow = !mShowShadow;
                    mStateChanged = true;
                }
#endif
                if ((mEvents & EVENT_DROP) != 0)
                {
                    dropTetromino();
                }

                if ((mEvents & EVENT_ROTATE_CW) != 0)
                {
                    rotateTetromino(true);
                }

                if ((mEvents & EVENT_MOVE_RIGHT) != 0)
                {
                    moveTetromino(1, 0);
                }
                else if ((mEvents & EVENT_MOVE_LEFT) != 0)
                {
                    moveTetromino(-1, 0);
                }

                if ((mEvents & EVENT_MOVE_DOWN) != 0)
                {
                    // Update score if the player accelerates downfall
                    mStats.score += (long)(SCORE_2_FILLED_ROW * (mStats.level + 1) 
                                           / SCORE_MOVE_DOWN_DIVISOR);

                    moveTetromino(0, 1);
                }
                mEvents = EVENT_NONE;
            }

            // Check if it's time to move downwards the falling tetromino
            if (currentTime - mLastFallTime >= mFallingDelay)
            {
                moveTetromino(0, 1);
                mLastFallTime = currentTime;
            }
        }
        // Save current time for next game update
        mSystemTime = currentTime;
    }
    // Draw game state
    mPlatform->renderGame();
}

// This event is called when the falling tetromino is moved
void Game::onTetrominoMoved()
{
#ifdef STC_SHOW_GHOST_PIECE
    int y = 0;
    // Calculate number of cells where shadow tetromino would be
    while (!checkCollision(0, ++y));
    mShadowGap = y - 1;
#endif
    mStateChanged = true;
}

// Process a key down event
void Game::onEventStart(int command)
{
    switch (command)
    {
    case EVENT_QUIT:
        mErrorCode = ERROR_PLAYER_QUITS;
        break;
    case EVENT_MOVE_DOWN:
        mEvents |= EVENT_MOVE_DOWN;
        mDelayDown = DAS_DELAY_TIMER;
        break;
    case EVENT_ROTATE_CW:
        mEvents |= EVENT_ROTATE_CW;
#ifdef STC_AUTO_ROTATION
        mDelayRotation = ROTATION_AUTOREPEAT_DELAY;
#endif
        break;
    case EVENT_MOVE_LEFT:
        mEvents |= EVENT_MOVE_LEFT;
        mDelayLeft = DAS_DELAY_TIMER;
        break;
    case EVENT_MOVE_RIGHT:
        mEvents |= EVENT_MOVE_RIGHT;
        mDelayRight = DAS_DELAY_TIMER;
        break;
    case EVENT_DROP:        // Fall through
    case EVENT_RESTART:     // Fall through
    case EVENT_PAUSE:       // Fall through
    case EVENT_SHOW_NEXT:   // Fall through
    case EVENT_SHOW_SHADOW:
        mEvents |= command;
        break;
    }
}

// Process a key up event
void Game::onEventEnd(int command)
{
    switch (command)
    {
    case EVENT_MOVE_DOWN:
        mDelayDown = -1;
        break;
    case EVENT_MOVE_LEFT:
        mDelayLeft = -1;
        break;
    case EVENT_MOVE_RIGHT:
        mDelayRight = -1;
        break;
#ifdef STC_AUTO_ROTATION
    case EVENT_ROTATE_CW:
        mDelayRotation = -1;
        break;
#endif // STC_AUTO_ROTATION
    }
}
}

