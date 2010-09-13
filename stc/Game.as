/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   A simple tetris clone.                                                   */
/*                                                                            */
/*   Copyright (c) 2010 Laurens Rodriguez Oscanoa.                            */
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

package stc {

import flash.display.MovieClip;

// Compiled with Flex 3.2.0.
[SWF(backgroundColor="#FFFFFF", frameRate="60", width="480", height="272")]

/* Core gameplay class */
public class Game extends MovieClip {

    /* Application name */
    private static const GAME_NAME:String = "STC: simple tetris clone (AS3)";

    /* Playfield size (in tiles) */
    public static const BOARD_WIDTH:int = 10;
    public static const BOARD_HEIGHT:int = 22;

    /* Initial time delay (in milliseconds) between falling moves */
    private static const INIT_DELAY_FALL:int = 1000;

    /* Score points given by filled rows
     * http://www.tetrisconcept.com/wiki/index.php/Scoring */
    private static const SCORE_1_FILLED_ROW:int = 400;
    private static const SCORE_2_FILLED_ROW:int = 1000;
    private static const SCORE_3_FILLED_ROW:int = 3000;
    private static const SCORE_4_FILLED_ROW:int = 12000;

    /* User gets more score if he uses hard drop. (from SCORE_2_FILLED_ROW) */
    private static const SCORE_DROP_FACTOR:Number = 0.05;
    private static const SCORE_DROP_WITH_SHADOW_FACTOR:Number = 0.01;

    /* User gets points every time he accelerates downfall (from SCORE_2_FILLED_ROW) */
    private static const SCORE_MOVE_DOWN_FACTOR:Number = 0.001;

    /* Number of filled rows required to increase the game level */
    private static const FILLED_ROWS_FOR_LEVEL_UP:int = 10;

    /* The falling delay is multiplied by this factor with every level up */
    private static const DELAY_FACTOR_FOR_LEVEL_UP:Number = 0.9;

    /* Game error codes */
    private static const GAME_ERROR_NONE:int   = 0;     /* Everything is OK, oh wonders!      */
    private static const GAME_ERROR_ASSERT:int = -100;  /* Something went very very wrong...  */

    /* Game events */
    public static const EVENT_NONE:int        = 0;
    public static const EVENT_MOVE_DOWN:int   = 1 << 1;
    public static const EVENT_MOVE_LEFT:int   = 1 << 2;
    public static const EVENT_MOVE_RIGHT:int  = 1 << 3;
    public static const EVENT_ROTATE_CW:int   = 1 << 4;    /* rotate clockwise         */
    public static const EVENT_ROTATE_CCW:int  = 1 << 5;    /* rotate counter-clockwise */
    public static const EVENT_DROP:int        = 1 << 6;
    public static const EVENT_PAUSE:int       = 1 << 7;
    public static const EVENT_RESTART:int     = 1 << 8;
    public static const EVENT_SHOW_NEXT:int   = 1 << 9;    /* toggle show next tetromino */
    public static const EVENT_SHOW_SHADOW:int = 1 << 10;   /* toggle show shadow */

    /* Number of tetromino types */
    public static const TETROMINO_TYPES:int = 7;

    /* Tetromino definitions (used as indexes: must be between 0 - [TETROMINO_TYPES - 1])
     * http://www.tetrisconcept.com/wiki/index.php/Tetromino */
    /*
        ....
        ****
        ....
        ....
    */
    public static const TETROMINO_I:int = 0;
    /*
        **..
        **..
        ....
        ....
    */
    public static const TETROMINO_O:int = 1;
    /*
        .*..
        ***.
        ....
        ....
    */
    public static const TETROMINO_T:int = 2;
    /*
        .**.
        **..
        ....
        ....
    */
    public static const TETROMINO_S:int = 3;
    /*
        **..
        .**.
        ....
        ....
    */
    public static const TETROMINO_Z:int = 4;
    /*
        *...
        ***.
        ....
        ....
    */
    public static const TETROMINO_J:int = 5;
    /*
        ..*.
        ***.
        ....
        ....
    */
    public static const TETROMINO_L:int = 6;

    /* Tetromino color indexes (must be between 0 - TETROMINO_TYPES) */
    public static const COLOR_CYAN:int   = 1;
    public static const COLOR_RED:int    = 2;
    public static const COLOR_BLUE:int   = 3;
    public static const COLOR_ORANGE:int = 4;
    public static const COLOR_GREEN:int  = 5;
    public static const COLOR_YELLOW:int = 6;
    public static const COLOR_PURPLE:int = 7;
    public static const COLOR_WHITE:int  = 0;    /* Used for effects (if any) */

    /* This value used for empty tiles */
    public static const EMPTY_CELL:int = -1;

    /* Matrix that holds the cells (tilemap) */
    public var map:Array;

    public var nextBlock:StcTetromino;      /* next tetromino               */
    public var fallingBlock:StcTetromino;   /* current falling tetromino    */
    public var errorCode:int;               /* game error code              */
    public var systemTime:Number;           /* system time in miliseconds   */
    public var delay:int;                   /* delay time for falling tetrominoes      */
    public var isOver:Boolean;          /* true if the game is over, false otherwise   */
    public var isPaused:Boolean;        /* true if the game is paused, false otherwise */
    public var showPreview:Boolean;     /* true if we must show preview tetromino      */
    public var showShadow:Boolean;      /* true if we must show ghost shadow           */
    public var lastFallTime:Number;     /* last time game moved the falling tetromino  */
    public var stateChanged:Boolean;    /* true if game state changed, false otherwise */
    public var scoreChanged:Boolean;    /* true if game score changed, false otherwise */

    public var shadowGap:int;    /* height gap between shadow and falling tetromino */

    /* Game events are stored in bits in this variable.
     * must be cleared to EVENT_NONE after being used. */
    public var events:int;

    /* Platform implementation */
    public var platform:Platform;

    /* Statistic data */
    public var stats:StcStats;

    /* Class constructor */
    public function Game() {
        platform = new Platform(this);
        startGame();
    }

    /* Set matrix elements to indicated value */
    private function setMatrixCells(matrix:Array, value:int):void {
        for (var i:int = 0; i < matrix.length; ++i) {
            for (var j:int = 0; j < matrix[i].length; ++j) {
                matrix[i][j] = value;
            }
        }
    }

    /* Returns initialized matrix with elements set to indicated value */
    public static function initMatrixCells(width:int, height:int, value:int):Array {
        var matrix:Array = new Array();
        for (var i:int = 0; i < width; ++i) {
            matrix.push(new Array());
            for (var j:int = 0; j < height; ++j) {
                matrix[i][j] = value;
            }
        }
        return matrix;
    }

    /* Initialize tetromino cells for every tipe of tetromino */
    private function setTetromino(indexTetromino:int, tetromino:StcTetromino):void {

        /* Initialize tetromino cells to empty cells */
        setMatrixCells(tetromino.cells, EMPTY_CELL);

        /* Almost all the blocks have size 3 */
        tetromino.size = 3;

        /* Initial configuration from: http://www.tetrisconcept.com/wiki/index.php/SRS */
        switch (indexTetromino) {
        case TETROMINO_I:
            tetromino.cells[0][1] = COLOR_CYAN;
            tetromino.cells[1][1] = COLOR_CYAN;
            tetromino.cells[2][1] = COLOR_CYAN;
            tetromino.cells[3][1] = COLOR_CYAN;
            tetromino.size = 4;
            break;
        case TETROMINO_O:
            tetromino.cells[0][0] = COLOR_YELLOW;
            tetromino.cells[0][1] = COLOR_YELLOW;
            tetromino.cells[1][0] = COLOR_YELLOW;
            tetromino.cells[1][1] = COLOR_YELLOW;
            tetromino.size = 2;
            break;
        case TETROMINO_T:
            tetromino.cells[0][1] = COLOR_PURPLE;
            tetromino.cells[1][0] = COLOR_PURPLE;
            tetromino.cells[1][1] = COLOR_PURPLE;
            tetromino.cells[2][1] = COLOR_PURPLE;
            break;
        case TETROMINO_S:
            tetromino.cells[0][1] = COLOR_GREEN;
            tetromino.cells[1][0] = COLOR_GREEN;
            tetromino.cells[1][1] = COLOR_GREEN;
            tetromino.cells[2][0] = COLOR_GREEN;
            break;
        case TETROMINO_Z:
            tetromino.cells[0][0] = COLOR_RED;
            tetromino.cells[1][0] = COLOR_RED;
            tetromino.cells[1][1] = COLOR_RED;
            tetromino.cells[2][1] = COLOR_RED;
            break;
        case TETROMINO_J:
            tetromino.cells[0][0] = COLOR_BLUE;
            tetromino.cells[0][1] = COLOR_BLUE;
            tetromino.cells[1][1] = COLOR_BLUE;
            tetromino.cells[2][1] = COLOR_BLUE;
            break;
        case TETROMINO_L:
            tetromino.cells[0][1] = COLOR_ORANGE;
            tetromino.cells[1][1] = COLOR_ORANGE;
            tetromino.cells[2][0] = COLOR_ORANGE;
            tetromino.cells[2][1] = COLOR_ORANGE;
            break;
        }
        tetromino.type = indexTetromino;
    }

    /*  Start a new game */
    private function startGame():void {

        /* Initialize game data */
        nextBlock = new StcTetromino();
        fallingBlock = new StcTetromino();
        errorCode = GAME_ERROR_NONE;
        systemTime = platform.getSystemTime();
        lastFallTime = systemTime;
        isOver = false;
        isPaused = false;
        showPreview = true;
        events = EVENT_NONE;
        delay = INIT_DELAY_FALL;
        showShadow = true;

        /* Initialize game statistics */
        stats = new StcStats();

        /* Initialize game tile map */
        map = initMatrixCells(BOARD_WIDTH, BOARD_HEIGHT, EMPTY_CELL);

        /* Initialize falling tetromino */
        setTetromino(TETROMINO_TYPES * Math.random(), fallingBlock);
        fallingBlock.x = (BOARD_WIDTH - fallingBlock.size) / 2;
        fallingBlock.y = 0;

        /* Initialize preview tetromino */
        setTetromino(TETROMINO_TYPES * Math.random(), nextBlock);

        /* Initialize events */
        onTetrominoMoved();
        scoreChanged = true;
    }

    /*
     * Rotate falling tetromino. If there are no collisions when the
     * tetromino is rotated this modifies the tetromino's cell buffer.
     */
    private function rotateTetromino(clockwise:int):void {
        var i:int, j:int;
        var rotated:Array;  /* temporary array to hold rotated cells */

        /* If TETROMINO_O is falling return immediately */
        if (fallingBlock.type == TETROMINO_O) {
            return; /* rotation doesn't require any changes */
        }

        /* Initialize rotated cells to blank */
        rotated = initMatrixCells(4, 4, EMPTY_CELL);

        /* Copy rotated cells to the temporary array */
        for (i = 0; i < fallingBlock.size; ++i) {
            for (j = 0; j < fallingBlock.size; ++j) {
                if (clockwise) {
                    rotated[fallingBlock.size - j - 1][i] = fallingBlock.cells[i][j];
                } else {
                    rotated[j][fallingBlock.size - i - 1] = fallingBlock.cells[i][j];
                }
            }
        }

        var wallDisplace:int = 0;

        /* Check collision with left wall */
        if (fallingBlock.x < 0) {
            for (i = 0; (wallDisplace == 0) && (i < -fallingBlock.x); ++i) {
                for (j = 0; j < fallingBlock.size; ++j) {
                    if (rotated[i][j] != EMPTY_CELL) {
                        wallDisplace = i - fallingBlock.x;
                        break;
                    }
                }
            }
        }
        /* Or check collision with right wall */
        else if (fallingBlock.x > BOARD_WIDTH - fallingBlock.size) {
            i = fallingBlock.size - 1;
            for (; (wallDisplace == 0) && (i >= BOARD_WIDTH - fallingBlock.x); --i) {
                for (j = 0; j < fallingBlock.size; ++j) {
                    if (rotated[i][j] != EMPTY_CELL) {
                        wallDisplace = -fallingBlock.x - i + BOARD_WIDTH - 1;
                        break;
                    }
                }
            }
        }

        /* Check collision with board floor and other cells on board */
        for (i = 0; i < fallingBlock.size; ++i) {
            for (j = 0; j < fallingBlock.size; ++j) {
                if (rotated[i][j] != EMPTY_CELL) {
                    /* Check collision with bottom border of the map */
                    if (fallingBlock.y + j >= BOARD_HEIGHT) {
                        return; /* there was collision therefore return */
                    }
                    /* Check collision with existing cells in the map */
                    if (map[i + fallingBlock.x + wallDisplace][j + fallingBlock.y] != EMPTY_CELL) {
                        return; /* there was collision therefore return */
                    }
                }
            }
        }
        /* Move the falling piece if there was wall collision and it's a legal move */
        if (wallDisplace != 0) {
            fallingBlock.x += wallDisplace;
        }

        /* There are no collisions, replace tetromino cells with rotated cells */
        for (i = 0; i < 4; ++i) {
            for (j = 0; j < 4; ++j) {
                fallingBlock.cells[i][j] = rotated[i][j];
            }
        }
        onTetrominoMoved();
    }

    /*
     * Check if tetromino will collide with something if it is moved in the requested direction.
     * If there are collisions returns true else returns false.
     */
    public function checkCollision(dx:int, dy:int):Boolean {
        var newx:int, newy:int, i:int, j:int;

        newx = fallingBlock.x + dx;
        newy = fallingBlock.y + dy;

        for (i = 0; i < fallingBlock.size; ++i) {
            for (j = 0; j < fallingBlock.size; ++j) {
                if (fallingBlock.cells[i][j] != EMPTY_CELL) {
                    /* Check the tetromino would be inside the left, right and bottom borders */
                    if ((newx + i < 0) || (newx + i >= BOARD_WIDTH)
                        || (newy + j >= BOARD_HEIGHT)) {
                        return true;
                    }
                    /* Check the tetromino won't collide with existing cells in the map */
                    if (map[newx + i][newy + j] != EMPTY_CELL) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    /* Game scoring: http://www.tetrisconcept.com/wiki/index.php/Scoring */
    private function onFilledRows(filledRows:int):void {
        /* Update total number of filled rows */
        stats.lines += filledRows;

        /* Increase score accordingly to the number of filled rows */
        switch (filledRows) {
        case 1:
            stats.score += (SCORE_1_FILLED_ROW * (stats.level + 1));
            break;
        case 2:
            stats.score += (SCORE_2_FILLED_ROW * (stats.level + 1));
            break;
        case 3:
            stats.score += (SCORE_3_FILLED_ROW * (stats.level + 1));
            break;
        case 4:
            stats.score += (SCORE_4_FILLED_ROW * (stats.level + 1));
            break;
        default:
            errorCode = GAME_ERROR_ASSERT;    /* This can't happen */
        }
        /* Check if we need to update level */
        if (stats.lines >= FILLED_ROWS_FOR_LEVEL_UP * (stats.level + 1)) {
            stats.level++;

            /* Increase speed for falling tetrominoes */
            delay *= DELAY_FACTOR_FOR_LEVEL_UP;
        }
    }

    /*
     * Move tetromino in the direction especified by (x, y) (in tile units)
     * This function detects if there are filled rows or if the move
     * lands a falling tetromino, also checks for game over condition.
     */
    private function moveTetromino(x:int, y:int):void {
        var i:int, j:int, hasFullRow:Boolean, numFilledRows:int;

        /* Check if the move would create a collision */
        if (checkCollision(x, y)) {
            /* In case of collision check if move was downwards (y == 1) */
            if (y == 1) {
                /* Check if collision occurs when the falling
                 * tetromino is on the 1st or 2nd row */
                if (fallingBlock.y <= 1) {
                    isOver = true;   /* if this happens the game is over */
                    platform.onGameOver(isOver);
                }
                else {
                    /* The falling tetromino has reached the bottom,
                     * so we copy their cells to the board map */
                    for (i = 0; i < fallingBlock.size; ++i) {
                        for (j = 0; j < fallingBlock.size; ++j) {
                            if (fallingBlock.cells[i][j] != EMPTY_CELL) {
                                map[fallingBlock.x + i][fallingBlock.y + j]
                                        = fallingBlock.cells[i][j];
                            }
                        }
                    }

                    /* Check if the landing tetromino has created full rows */
                    numFilledRows = 0;
                    for (j = 1; j < BOARD_HEIGHT; ++j) {
                        hasFullRow = true;
                        for (i = 0; i < BOARD_WIDTH; ++i) {
                            if (map[i][j] == EMPTY_CELL) {
                                hasFullRow = false;
                                break;
                            }
                        }
                        /* If we found a full row we need to remove that row from the map
                         * we do that by just moving all the above rows one row below */
                        if (hasFullRow) {
                            for (x = 0; x < BOARD_WIDTH; ++x) {
                                for (y = j; y > 0; --y) {
                                    map[x][y] = map[x][y - 1];
                                }
                            }
                            numFilledRows++;    /* increase filled row counter */
                        }
                    }

                    /* Update game statistics */
                    if (numFilledRows) {
                        onFilledRows(numFilledRows);
                    }
                    stats.totalPieces++;
                    stats.pieces[fallingBlock.type]++;
                    scoreChanged = true;

                    /* Use preview tetromino as falling tetromino.
                     * Copy preview tetromino for falling tetromino */
                    for (i = 0; i < 4; ++i) {
                        for (j = 0; j < 4; ++j) {
                            fallingBlock.cells[i][j] = nextBlock.cells[i][j];
                        }
                    }
                    fallingBlock.size = nextBlock.size;
                    fallingBlock.type = nextBlock.type;

                    /* Reset position */
                    fallingBlock.y = 0;
                    fallingBlock.x = (BOARD_WIDTH - fallingBlock.size) / 2;
                    onTetrominoMoved();

                    /* Create next preview tetromino */
                    setTetromino(TETROMINO_TYPES * Math.random(), nextBlock);
                }
            }
            /* Nothing changes */
            return;
        }
        else {
            /* There are no collisions, just move the tetromino */
            fallingBlock.x += x;
            fallingBlock.y += y;
        }
        onTetrominoMoved();
    }

    /* Hard drop */
    private function dropTetromino():void {
        moveTetromino(0, shadowGap);
        moveTetromino(0, 1); /* Force lock */

        /* Update score */
        if (showShadow) {
            stats.score += SCORE_DROP_WITH_SHADOW_FACTOR * (SCORE_2_FILLED_ROW * (stats.level + 1));
        }
        else {
            stats.score += SCORE_DROP_FACTOR * (SCORE_2_FILLED_ROW * (stats.level + 1));
        }
        scoreChanged = true;
    }

    /* Main function game called every frame */
    public function gameUpdate():void {
        var sysTime:Number;

        /* Update game state */
        if (isOver) {
            if (events & EVENT_RESTART) {
                isOver = false;
                platform.onGameOver(isOver);
                startGame();
            }
        }
        else {
            sysTime = platform.getSystemTime();

            /* Always handle pause event */
            if (events & EVENT_PAUSE) {
                isPaused = !isPaused;
                events = EVENT_NONE;
                platform.onGamePaused(isPaused);
            }

            /* Check if the game is paused */
            if (isPaused) {
                /* We achieve the effect of pausing the game
                 * adding the last frame duration to lastFallTime */
                lastFallTime += (sysTime - systemTime);
            }
            else {
                if (events != EVENT_NONE) {
                    if (events & EVENT_SHOW_NEXT) {
                        showPreview = !showPreview;
                        stateChanged = true;
                    }
                    if (events & EVENT_SHOW_SHADOW) {
                        showShadow = !showShadow;
                        stateChanged = true;
                    }
                    if (events & EVENT_DROP) {
                        dropTetromino();
                    }
                    if (events & EVENT_ROTATE_CW) {
                        rotateTetromino(1);
                    }
                    if (events & EVENT_MOVE_RIGHT) {
                        moveTetromino(1, 0);
                    }
                    else if (events & EVENT_MOVE_LEFT) {
                        moveTetromino(-1, 0);
                    }
                    if (events & EVENT_MOVE_DOWN) {
                        /* Update score if the user accelerates downfall */
                        stats.score += SCORE_MOVE_DOWN_FACTOR * (SCORE_2_FILLED_ROW * (stats.level + 1));
                        scoreChanged = true;

                        moveTetromino(0, 1);
                    }
                    events = EVENT_NONE;
                }
                /* Check if it's time to move downwards the falling tetromino */
                if (sysTime - lastFallTime >= delay) {
                    moveTetromino(0, 1);
                    lastFallTime = sysTime;
                }
            }
            systemTime = sysTime;
        }
        /* Draw game state */
        platform.renderGame();
    }

    /* This event is called when the falling tetromino is moved */
    private function onTetrominoMoved():void {
        var y:int = 0;
        /* Calculate number of cells where shadow tetromino would be */
        while (!checkCollision(0, ++y));
        shadowGap = y - 1;

        stateChanged = true;
    }
}
}

/* AS3 doesn't support inner classes, also below class
 * declarations can't be put before the package definition. */
import stc.Game;

/*
 * Data structure that holds information about our tetromino blocks
 */
class StcTetromino {
    public var cells:Array;
    public var x:int;
    public var y:int;
    public var size:int;
    public var type:int;
    public function StcTetromino() {
        cells = Game.initMatrixCells(4, 4, Game.EMPTY_CELL);
    }
}

/*
 * Statistic data
 */
class StcStats {
    public var score:Number;       /* user score for current game      */
    public var lines:int;          /* total number of lines cleared    */
    public var totalPieces:int;    /* total number of tetrominoes used */
    public var pieces:Array;       /* number of tetrominoes per type   */
    public var level:int;          /* current game level               */
    public function StcStats() {
        score = 0;
        lines = 0;
        totalPieces = 0;
        level = 0;
        pieces = new Array();
        for (var i:int = 0; i < Game.TETROMINO_TYPES; ++i) {
            pieces[i] = 0;
        }
    }
}
