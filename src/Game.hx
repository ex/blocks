/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   A simple tetris clone.                                                   */
/*                                                                            */
/*   Copyright (c) 2014 Laurens Rodriguez Oscanoa.                            */
/*                                                                            */
/*   Permission is hereby granted, free of charge, to any person              */
/*   obtaining a copy of this software and associated documentation           */
/*   files (the "Software"), to deal in the Software without                  */
/*   restriction, including without limitation the rights to use,             */
/*   copy, modify, merge, publish, distribute, sublicense, and/or sell        */
/*   copies of the Software, and to permit persons to whom the                */
/*   Software is furnished to do so, subject to the following                 */
/*   conditions:                                                              */
/*                                                                            */
/*   The above copyright notice and this permission notice shall be           */
/*   included in all copies or substantial portions of the Software.          */
/*                                                                            */
/*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,          */
/*   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES          */
/*   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                 */
/*   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT              */
/*   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,             */
/*   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING             */
/*   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR            */
/*   OTHER DEALINGS IN THE SOFTWARE.                                          */
/* -------------------------------------------------------------------------- */

// Core gameplay class
class Game
{
    // Application name
    private static inline var GAME_NAME:String = "STC: simple tetris clone (Haxe)";

    // Playfield size (in tiles)
    public static inline var BOARD_WIDTH:Int = 10;
    public static inline var BOARD_HEIGHT:Int = 22;

    // Initial time delay (in milliseconds) between falling moves
    private static inline var INIT_DELAY_FALL:Int = 1000;

    // Score points given by filled rows
    // http://www.tetrisconcept.com/wiki/index.php/Scoring
    private static inline var SCORE_1_FILLED_ROW:Int = 400;
    private static inline var SCORE_2_FILLED_ROW:Int = 1000;
    private static inline var SCORE_3_FILLED_ROW:Int = 3000;
    private static inline var SCORE_4_FILLED_ROW:Int = 12000;

    // User gets more score if he uses hard drop. (from SCORE_2_FILLED_ROW)
    private static inline var SCORE_DROP_FACTOR:Float = 0.05;
    private static inline var SCORE_DROP_WITH_SHADOW_FACTOR:Float = 0.01;

    // User gets points every time he accelerates downfall (from SCORE_2_FILLED_ROW)
    private static inline var SCORE_MOVE_DOWN_FACTOR:Float = 0.001;

    // Float of filled rows required to increase the game level
    private static inline var FILLED_ROWS_FOR_LEVEL_UP:Int = 10;

    // The falling delay is multiplied by this factor with every level up
    private static inline var DELAY_FACTOR_FOR_LEVEL_UP:Float = 0.9;

    // Delayed autoshift initial delay.
    private static inline var DAS_DELAY_TIMER:Int = 200;

    // Delayed autoshift timer for left and right moves.
    private static inline var DAS_MOVE_TIMER:Int = 40;

    // Rotation auto-repeat delay.
    private static inline var ROTATION_AUTOREPEAT_DELAY:Int = 375;

    // Rotation autorepeat timer.
    private static inline var ROTATION_AUTOREPEAT_TIMER:Int = 200;

    // Game error codes
    private static inline var GAME_ERROR_NONE:Int   = 0;     // Everything is OK, oh wonders!
    private static inline var GAME_ERROR_ASSERT:Int = -100;  // Something went very very wrong...

    // Game events
    public static inline var EVENT_NONE:Int        = 0;
    public static inline var EVENT_MOVE_DOWN:Int   = 1 << 1;
    public static inline var EVENT_MOVE_LEFT:Int   = 1 << 2;
    public static inline var EVENT_MOVE_RIGHT:Int  = 1 << 3;
    public static inline var EVENT_ROTATE_CW:Int   = 1 << 4;    // rotate clockwise
    public static inline var EVENT_ROTATE_CCW:Int  = 1 << 5;    // rotate counter-clockwise
    public static inline var EVENT_DROP:Int        = 1 << 6;
    public static inline var EVENT_PAUSE:Int       = 1 << 7;
    public static inline var EVENT_RESTART:Int     = 1 << 8;
    public static inline var EVENT_SHOW_NEXT:Int   = 1 << 9;    // toggle show next tetromino
    public static inline var EVENT_SHOW_SHADOW:Int = 1 << 10;   // toggle show shadow

    // Float of tetromino types
    public static inline var TETROMINO_TYPES:Int = 7;

    // Tetromino definitions (used as indexes: must be between 0 - [TETROMINO_TYPES - 1])
    // http://www.tetrisconcept.com/wiki/index.php/Tetromino
    //
    //  ....
    //  ****
    //  ....
    //  ....
    //
    public static inline var TETROMINO_I:Int = 0;
    //
    //  **..
    //  **..
    //  ....
    //  ....
    //
    public static inline var TETROMINO_O:Int = 1;
    //
    //  .*..
    //  ***.
    //  ....
    //  ....
    //
    public static inline var TETROMINO_T:Int = 2;
    //
    //  .**.
    //  **..
    //  ....
    //  ....
    //
    public static inline var TETROMINO_S:Int = 3;
    //
    //  **..
    //  .**.
    //  ....
    //  ....
    //
    public static inline var TETROMINO_Z:Int = 4;
    //
    //  *...
    //  ***.
    //  ....
    //  ....
    //
    public static inline var TETROMINO_J:Int = 5;
    //
    //  ..*.
    //  ***.
    //  ....
    //  ....
    //
    public static inline var TETROMINO_L:Int = 6;

    // Tetromino color indexes (must be between 0 - TETROMINO_TYPES)
    public static inline var COLOR_CYAN:Int   = 1;
    public static inline var COLOR_RED:Int    = 2;
    public static inline var COLOR_BLUE:Int   = 3;
    public static inline var COLOR_ORANGE:Int = 4;
    public static inline var COLOR_GREEN:Int  = 5;
    public static inline var COLOR_YELLOW:Int = 6;
    public static inline var COLOR_PURPLE:Int = 7;
    public static inline var COLOR_WHITE:Int  = 0;    // Used for effects (if any)

    // This value used for empty tiles
    public static inline var EMPTY_CELL:Int = -1;

    // Matrix that holds the cells (tilemap)
    public var map:Array<Array<Int>>;

    public var nextBlock:StcTetromino;    // next tetromino
    public var fallingBlock:StcTetromino; // current falling tetromino
    public var errorCode:Int;             // game error code
    public var systemTime:Float;          // system time in miliseconds
    public var delay:Int;                 // delay time for falling tetrominoes
    public var isOver:Bool;        // true if the game is over, false otherwise
    public var isPaused:Bool;      // true if the game is paused, false otherwise
    public var showPreview:Bool;   // true if we must show preview tetromino
    public var showShadow:Bool;    // true if we must show ghost shadow
    public var lastFallTime:Float; // last time game moved the falling tetromino
    public var stateChanged:Bool;  // true if game state changed, false otherwise
    public var scoreChanged:Bool;  // true if game score changed, false otherwise

    public var shadowGap:Int;    // height gap between shadow and falling tetromino

    // "Master mode" aka invisible board mode.
    private static inline var MASTER_FACTOR:Int = 5;
    private var mMasterMode:Bool;
    public function getMasterMode():Bool
    {
        return mMasterMode;
    }
    public function setMasterMode(mode:Bool):Void
    {
        mMasterMode = mode;
        stateChanged = true;
    }

    // Game events are stored in bits in this variable.
    // must be cleared to EVENT_NONE after being used.
    public var mEvents:Int;

    // Statistic data
    public var stats:StcStats;

    // Platform implementation
    private var mPlatform:PlatformBase;

    // For delayed autoshift: http://tetris.wikia.com/wiki/DAS
    private var mDelayLeft:Int;
    private var mDelayRight:Int;
    private var mDelayDown:Int;
    private var mDelayRotation:Int;

    public function new(platform:Dynamic)
    {
        mMasterMode = false;
        mPlatform = platform;
    }

    // Set matrix elements to indicated value
    private function setMatrixCells(matrix:Array<Array<Int>>, value:Int):Void
    {
        for (i in 0 ... matrix.length)
        {
            for (j in 0 ... matrix[i].length)
            {
                matrix[i][j] = value;
            }
        }
    }

    // Returns initialized matrix with elements set to indicated value
    public static function initMatrixCells(width:Int, height:Int, value:Int):Array<Array<Int>>
    {
        var matrix:Array<Array<Int>> = new Array();
        for (i in 0 ... width)
        {
            matrix.push(new Array());
            for (j in 0 ... height)
            {
                matrix[i][j] = value;
            }
        }
        return matrix;
    }

    // Initialize tetromino cells for every tipe of tetromino
    private function setTetromino(indexTetromino:Int, tetromino:StcTetromino):Void
    {
        // Initialize tetromino cells to empty cells
        setMatrixCells(tetromino.cells, EMPTY_CELL);

        // Almost all the blocks have size 3
        tetromino.size = 3;

        // Initial configuration from: http://www.tetrisconcept.com/wiki/index.php/SRS
        switch (indexTetromino)
        {
            case TETROMINO_I:
                tetromino.cells[0][1] = COLOR_CYAN;
                tetromino.cells[1][1] = COLOR_CYAN;
                tetromino.cells[2][1] = COLOR_CYAN;
                tetromino.cells[3][1] = COLOR_CYAN;
                tetromino.size = 4;
            case TETROMINO_O:
                tetromino.cells[0][0] = COLOR_YELLOW;
                tetromino.cells[0][1] = COLOR_YELLOW;
                tetromino.cells[1][0] = COLOR_YELLOW;
                tetromino.cells[1][1] = COLOR_YELLOW;
                tetromino.size = 2;
            case TETROMINO_T:
                tetromino.cells[0][1] = COLOR_PURPLE;
                tetromino.cells[1][0] = COLOR_PURPLE;
                tetromino.cells[1][1] = COLOR_PURPLE;
                tetromino.cells[2][1] = COLOR_PURPLE;
            case TETROMINO_S:
                tetromino.cells[0][1] = COLOR_GREEN;
                tetromino.cells[1][0] = COLOR_GREEN;
                tetromino.cells[1][1] = COLOR_GREEN;
                tetromino.cells[2][0] = COLOR_GREEN;
            case TETROMINO_Z:
                tetromino.cells[0][0] = COLOR_RED;
                tetromino.cells[1][0] = COLOR_RED;
                tetromino.cells[1][1] = COLOR_RED;
                tetromino.cells[2][1] = COLOR_RED;
            case TETROMINO_J:
                tetromino.cells[0][0] = COLOR_BLUE;
                tetromino.cells[0][1] = COLOR_BLUE;
                tetromino.cells[1][1] = COLOR_BLUE;
                tetromino.cells[2][1] = COLOR_BLUE;
            case TETROMINO_L:
                tetromino.cells[0][1] = COLOR_ORANGE;
                tetromino.cells[1][1] = COLOR_ORANGE;
                tetromino.cells[2][0] = COLOR_ORANGE;
                tetromino.cells[2][1] = COLOR_ORANGE;
        }
        tetromino.type = indexTetromino;
    }

    //  Start a new game
    public function startGame():Void
    {
        // Initialize game data
        nextBlock = new StcTetromino();
        fallingBlock = new StcTetromino();
        errorCode = GAME_ERROR_NONE;
        systemTime = mPlatform.getSystemTime();
        lastFallTime = systemTime;
        isOver = false;
        isPaused = false;
        showPreview = true;
        mEvents = EVENT_NONE;
        delay = INIT_DELAY_FALL;
        showShadow = true;

        // Initialize game statistics
        stats = new StcStats();

        // Initialize game tile map
        map = initMatrixCells(BOARD_WIDTH, BOARD_HEIGHT, EMPTY_CELL);

        // Initialize falling tetromino
        setTetromino(Std.random(TETROMINO_TYPES), fallingBlock);
        fallingBlock.x = Std.int((BOARD_WIDTH - fallingBlock.size) / 2);
        fallingBlock.y = 0;

        // Initialize preview tetromino
        setTetromino(Std.random(TETROMINO_TYPES), nextBlock);

        // Initialize events
        onTetrominoMoved();
        scoreChanged = true;

        // Initialize delayed autoshift
        mDelayLeft = -1;
        mDelayRight = -1;
        mDelayDown = -1;
        mDelayRotation = -1;
    }

    // Rotate falling tetromino. If there are no collisions when the
    // tetromino is rotated this modifies the tetromino's cell buffer.
    //
    private function rotateTetromino(clockwise:Int):Void
    {
        var i:Int, j:Int;
        var rotated:Array<Array<Int>>;  // temporary array to hold rotated cells

        // If TETROMINO_O is falling return immediately
        if (fallingBlock.type == TETROMINO_O)
        {
            return; // rotation doesn't require any changes
        }

        // Initialize rotated cells to blank
        rotated = initMatrixCells(4, 4, EMPTY_CELL);

        // Copy rotated cells to the temporary array
        for (i in 0 ... fallingBlock.size)
        {
            for (j in 0 ... fallingBlock.size)
            {
                if (clockwise != 0)
                {
                    rotated[fallingBlock.size - j - 1][i] = fallingBlock.cells[i][j];
                }
                else
                {
                    rotated[j][fallingBlock.size - i - 1] = fallingBlock.cells[i][j];
                }
            }
        }

        var wallDisplace:Int = 0;

        // Check collision with left wall
        if (fallingBlock.x < 0)
        {
            i = 0;
            while ((wallDisplace == 0) && (i < -fallingBlock.x))
            {
                for (j in 0 ... fallingBlock.size)
                {
                    if (rotated[i][j] != EMPTY_CELL)
                    {
                        wallDisplace = i - fallingBlock.x;
                        break;
                    }
                }
                ++i;
            }
        }
        // Or check collision with right wall
        else if (fallingBlock.x > BOARD_WIDTH - fallingBlock.size)
        {
            i = fallingBlock.size - 1;
            while ((wallDisplace == 0) && (i >= BOARD_WIDTH - fallingBlock.x))
            {
                for (j in 0 ... fallingBlock.size)
                {
                    if (rotated[i][j] != EMPTY_CELL)
                    {
                        wallDisplace = -fallingBlock.x - i + BOARD_WIDTH - 1;
                        break;
                    }
                }
                --i;
            }
        }

        // Check collision with board floor and other cells on board
        for (i in 0 ... fallingBlock.size)
        {
            for (j in 0 ... fallingBlock.size)
            {
                if (rotated[i][j] != EMPTY_CELL)
                {
                    // Check collision with bottom border of the map
                    if (fallingBlock.y + j >= BOARD_HEIGHT)
                    {
                        return; // there was collision therefore return
                    }
                    // Check collision with existing cells in the map
                    if (map[i + fallingBlock.x + wallDisplace][j + fallingBlock.y] != EMPTY_CELL)
                    {
                        return; // there was collision therefore return
                    }
                }
            }
        }
        // Move the falling piece if there was wall collision and it's a legal move
        if (wallDisplace != 0)
        {
            fallingBlock.x += wallDisplace;
        }

        // There are no collisions, replace tetromino cells with rotated cells
        for (i in 0 ... 4)
        {
            for (j in 0 ... 4)
            {
                fallingBlock.cells[i][j] = rotated[i][j];
            }
        }
        onTetrominoMoved();
    }

    // Check if tetromino will collide with something if it is moved in the requested direction.
    // If there are collisions returns true else returns false.
    public function checkCollision(dx:Int, dy:Int):Bool
    {
        var newx:Int, newy:Int, i:Int, j:Int;

        newx = fallingBlock.x + dx;
        newy = fallingBlock.y + dy;

        for (i in 0 ... fallingBlock.size)
        {
            for (j in 0 ... fallingBlock.size)
            {
                if (fallingBlock.cells[i][j] != EMPTY_CELL)
                {
                    // Check the tetromino would be inside the left, right and bottom borders
                    if ((newx + i < 0) || (newx + i >= BOARD_WIDTH) || (newy + j >= BOARD_HEIGHT))
                    {
                        return true;
                    }
                    // Check the tetromino won't collide with existing cells in the map
                    if (map[newx + i][newy + j] != EMPTY_CELL)
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    // Game scoring: http://www.tetrisconcept.com/wiki/index.php/Scoring
    private function onFilledRows(filledRows:Int):Void
    {
        // Update total number of filled rows
        stats.lines += filledRows;

        // Increase score accordingly to the number of filled rows
        var score:Float;
        switch (filledRows)
        {
            case 1:
                score = (SCORE_1_FILLED_ROW * (stats.level + 1));
            case 2:
                score = (SCORE_2_FILLED_ROW * (stats.level + 1));
            case 3:
                score = (SCORE_3_FILLED_ROW * (stats.level + 1));
            case 4:
                score = (SCORE_4_FILLED_ROW * (stats.level + 1));
            default:
                errorCode = GAME_ERROR_ASSERT;    // This can't happen
                return;
        }
        if (mMasterMode)
        {
            score *= MASTER_FACTOR;
        }
        stats.score += score;

        // Check if we need to update level
        if (stats.lines >= FILLED_ROWS_FOR_LEVEL_UP * (stats.level + 1))
        {
            stats.level++;

            // Increase speed for falling tetrominoes
            delay = Std.int(delay * DELAY_FACTOR_FOR_LEVEL_UP);
        }

        mPlatform.onFilledRows();
    }

    // Move tetromino in the direction especified by (x, y) (in tile units)
    // This function detects if there are filled rows or if the move
    // lands a falling tetromino, also checks for game over condition.
    private function moveTetromino(x:Int, y:Int):Void
    {
        var i:Int, j:Int, hasFullRow:Bool, numFilledRows:Int;

        // Check if the move would create a collision
        if (checkCollision(x, y))
        {
            // In case of collision check if move was downwards (y == 1)
            if (y == 1)
            {
                // Check if collision occurs when the falling
                // tetromino is on the 1st or 2nd row
                if (fallingBlock.y <= 1)
                {
                    isOver = true;   // if this happens the game is over
                    mPlatform.onGameOver(isOver);
                }
                else
                {
                    // The falling tetromino has reached the bottom,
                    // so we copy their cells to the board map
                    for (i in 0 ... fallingBlock.size)
                    {
                        for (j in 0 ... fallingBlock.size)
                        {
                            if (fallingBlock.cells[i][j] != EMPTY_CELL)
                            {
                                map[fallingBlock.x + i][fallingBlock.y + j]
                                        = fallingBlock.cells[i][j];
                            }
                        }
                    }
                    mPlatform.onTetrominoLand();

                    // Check if the landing tetromino has created full rows
                    numFilledRows = 0;
                    for (j in 1 ... BOARD_HEIGHT)
                    {
                        hasFullRow = true;
                        for (i in 0 ... BOARD_WIDTH)
                        {
                            if (map[i][j] == EMPTY_CELL)
                            {
                                hasFullRow = false;
                                break;
                            }
                        }
                        // If we found a full row we need to remove that row from the map
                        // we do that by just moving all the above rows one row below
                        if (hasFullRow)
                        {
                            for (x in 0 ... BOARD_WIDTH)
                            {
                                y = j;
                                while (y > 0)
                                {
                                    map[x][y] = map[x][y - 1];
                                    --y;
                                }
                            }
                            numFilledRows++;    // increase filled row counter
                        }
                    }

                    // Update game statistics
                    if (numFilledRows != 0)
                    {
                        onFilledRows(numFilledRows);
                    }
                    stats.totalPieces++;
                    stats.pieces[fallingBlock.type]++;
                    scoreChanged = true;

                    // Use preview tetromino as falling tetromino.
                    // Copy preview tetromino for falling tetromino
                    for (i in 0 ... 4)
                    {
                        for (j in 0 ... 4)
                        {
                            fallingBlock.cells[i][j] = nextBlock.cells[i][j];
                        }
                    }
                    fallingBlock.size = nextBlock.size;
                    fallingBlock.type = nextBlock.type;

                    // Reset position
                    fallingBlock.y = 0;
                    fallingBlock.x = Std.int((BOARD_WIDTH - fallingBlock.size) / 2);
                    onTetrominoMoved();

                    // Create next preview tetromino
                    setTetromino(Std.random(TETROMINO_TYPES), nextBlock);
                }
            }
            // Nothing changes
            return;
        }
        else
        {
            // There are no collisions, just move the tetromino
            fallingBlock.x += x;
            fallingBlock.y += y;
        }
        onTetrominoMoved();
    }

    // Hard drop
    private function dropTetromino():Void
    {
        // Shadow has already calculated the landing position.
        fallingBlock.y += shadowGap;

        // Force lock.
        moveTetromino(0, 1);

        // Update score
        if (showShadow)
        {
            stats.score += SCORE_DROP_WITH_SHADOW_FACTOR * (SCORE_2_FILLED_ROW * (stats.level + 1));
        }
        else
        {
            stats.score += SCORE_DROP_FACTOR * (SCORE_2_FILLED_ROW * (stats.level + 1));
        }
        scoreChanged = true;
    }

    // Main function game called every frame
    public function update():Void
    {
        // Update game state
        if (isOver)
        {
            if ((mEvents & EVENT_RESTART) != 0)
            {
                isOver = false;
                mPlatform.onGameOver(isOver);
                startGame();
            }
        }
        else
        {
            var currentTime:Float = mPlatform.getSystemTime();

            // Process delayed autoshift
            var timeDelta:Int = Std.int(currentTime - systemTime);
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

            if (mDelayDown > 0)
            {
                mDelayDown -= timeDelta;
                if (mDelayDown <= 0)
                {
                    mDelayDown = DAS_MOVE_TIMER;
                    mEvents |= EVENT_MOVE_DOWN;
                }
            }

            if (mDelayRotation > 0)
            {
                mDelayRotation -= timeDelta;
                if (mDelayRotation <= 0)
                {
                    mDelayRotation = ROTATION_AUTOREPEAT_TIMER;
                    mEvents |= EVENT_ROTATE_CW;
                }
            }

            // Always handle pause event
            if ((mEvents & EVENT_PAUSE) != 0)
            {
                isPaused = !isPaused;
                mEvents = EVENT_NONE;
                mPlatform.onGamePaused(isPaused);
            }

            // Check if the game is paused
            if (isPaused)
            {
                // We achieve the effect of pausing the game
                // adding the last frame duration to lastFallTime
                lastFallTime += (currentTime - systemTime);
            }
            else
            {
                if (mEvents != EVENT_NONE)
                {
                    if ((mEvents & EVENT_SHOW_NEXT) != 0)
                    {
                        showPreview = !showPreview;
                        stateChanged = true;
                    }
                    if ((mEvents & EVENT_SHOW_SHADOW) != 0)
                    {
                        showShadow = !showShadow;
                        stateChanged = true;
                    }
                    if ((mEvents & EVENT_DROP) != 0)
                    {
                        dropTetromino();
                    }
                    if ((mEvents & EVENT_ROTATE_CW) != 0)
                    {
                        rotateTetromino(1);
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
                        // Update score if the user accelerates downfall
                        stats.score += SCORE_MOVE_DOWN_FACTOR * (SCORE_2_FILLED_ROW * (stats.level + 1));
                        scoreChanged = true;

                        moveTetromino(0, 1);
                    }
                    mEvents = EVENT_NONE;
                }

                // Check if it's time to move downwards the falling tetromino
                if (currentTime - lastFallTime >= delay)
                {
                    moveTetromino(0, 1);
                    lastFallTime = currentTime;
                }
            }
            systemTime = currentTime;
        }
        // Draw game state
        mPlatform.renderGame();
    }

    // This event is called when the falling tetromino is moved
    private function onTetrominoMoved():Void
    {
        var y:Int = 1;
        // Calculate number of cells where shadow tetromino would be
        while (!checkCollision(0, y))
        {
            ++y;
        }
        shadowGap = y - 1;

        stateChanged = true;
    }

    // Process a key down event
    public function onEventStart(command:Int):Void
    {
        switch (command)
        {
            case EVENT_MOVE_DOWN:
                mEvents |= EVENT_MOVE_DOWN;
                mDelayDown = DAS_DELAY_TIMER;
            case EVENT_ROTATE_CW:
                mEvents |= EVENT_ROTATE_CW;
                mDelayRotation = ROTATION_AUTOREPEAT_DELAY;
            case EVENT_MOVE_LEFT:
                mEvents |= EVENT_MOVE_LEFT;
                mDelayLeft = DAS_DELAY_TIMER;
            case EVENT_MOVE_RIGHT:
                mEvents |= EVENT_MOVE_RIGHT;
                mDelayRight = DAS_DELAY_TIMER;
            case EVENT_DROP, EVENT_RESTART, EVENT_PAUSE, EVENT_SHOW_NEXT, EVENT_SHOW_SHADOW:
                mEvents |= command;
        }
    }

    // Process a key up event
    public function onEventEnd(command:Int):Void
    {
        switch (command)
        {
            case EVENT_MOVE_DOWN:
                mDelayDown = -1;
            case EVENT_MOVE_LEFT:
                mDelayLeft = -1;
            case EVENT_MOVE_RIGHT:
                mDelayRight = -1;
            case EVENT_ROTATE_CW:
                mDelayRotation = -1;
        }
    }
}

// Data structure that holds information about our tetromino blocks
//
class StcTetromino
{

    public var cells:Array<Array<Int>>;
    public var x:Int;
    public var y:Int;
    public var size:Int;
    public var type:Int;

    public function new()
    {
        cells = Game.initMatrixCells(4, 4, Game.EMPTY_CELL);
    }
}

// Statistic data
//
class StcStats
{
    public var score:Float;        // user score for current game
    public var lines:Int;          // total number of lines cleared
    public var totalPieces:Int;    // total number of tetrominoes used
    public var pieces:Array<Int>;  // number of tetrominoes per type
    public var level:Int;          // current game level

    public function new()
    {
        score = 0;
        lines = 0;
        totalPieces = 0;
        level = 0;
        pieces = new Array();
        for (i in 0 ... Game.TETROMINO_TYPES)
        {
            pieces[i] = 0;
        }
    }
}
