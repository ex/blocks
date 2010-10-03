/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Game logic implementation.                                               */
/*                                                                            */
/*   Copyright (c) 2010 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

package com.ex.stc;

public class Game {
	
    /* Enable shadow piece:						*/
    /* http://tetris.wikia.com/wiki/Ghost_piece */
    public static final boolean STC_SHOW_GHOST_PIECE = true;

    /* Enable wall kick:					    */
    /* http://tetris.wikia.com/wiki/Wall_kick   */
    public static final boolean STC_WALL_KICK_ENABLED = true;

    /* Enable auto-rotation of the falling piece */
    public static final boolean STC_AUTO_ROTATION = true;

    /* Application name */
    private static final String GAME_NAME = "STC: simple tetris clone (J2ME)";

    /* Playfield size (in tiles) */
    public static final int BOARD_WIDTH = 10;
    public static final int BOARD_HEIGHT = 22;

    /* Initial time delay (in milliseconds) between falling moves */
    private static final int INIT_DELAY_FALL = 1000;

    /* Score points given by filled rows (we use the original NES * 10)
     * http://tetris.wikia.com/wiki/Scoring */
    private static final int SCORE_1_FILLED_ROW = 400;
    private static final int SCORE_2_FILLED_ROW = 1000;
    private static final int SCORE_3_FILLED_ROW = 3000;
    private static final int SCORE_4_FILLED_ROW = 12000;

    /* User gets more score if he uses hard drop. (factor of SCORE_2_FILLED_ROW) */
    private static final double SCORE_DROP_FACTOR = 0.05;
    private static final double SCORE_DROP_WITH_SHADOW_FACTOR = 0.01;

    /* User gets points every time he accelerates downfall (factor of SCORE_2_FILLED_ROW) */
    private static final double SCORE_MOVE_DOWN_FACTOR = 0.001;

    /* Number of filled rows required to increase the game level */
    private static final int FILLED_ROWS_FOR_LEVEL_UP = 10;

    /* The falling delay is multiplied by this factor with every level up */
    private static final double DELAY_FACTOR_FOR_LEVEL_UP = 0.9;

    /*
     * Game error codes
     */
    public static final int GAME_ERROR_NONE       = 0;     /* Everything is OK, oh wonders!      */
    public static final int GAME_ERROR_USER_QUITS = 1;     /* The user quits (bored?), our fail  */
    public static final int GAME_ERROR_NO_MEMORY  = -1;    /* Not enough memory                  */
    public static final int GAME_ERROR_NO_VIDEO   = -2;    /* Video system was not initialized   */
    public static final int GAME_ERROR_NO_IMAGES  = -3;    /* Problem loading the image files    */
    public static final int GAME_ERROR_ASSERT     = -100;  /* Something went very very wrong...  */

    /*
     * Game events
     */
    public static final int EVENT_NONE        = 0;
    public static final int EVENT_MOVE_DOWN   = 1 << 1;
    public static final int EVENT_MOVE_LEFT   = 1 << 2;
    public static final int EVENT_MOVE_RIGHT  = 1 << 3;
    public static final int EVENT_ROTATE_CW   = 1 << 4;    /* rotate clockwise         */
    public static final int EVENT_ROTATE_CCW  = 1 << 5;    /* rotate counter-clockwise */
    public static final int EVENT_DROP        = 1 << 6;
    public static final int EVENT_PAUSE       = 1 << 7;
    public static final int EVENT_RESTART     = 1 << 8;
    public static final int EVENT_SHOW_NEXT   = 1 << 9;    /* toggle show next tetromino */
    public static final int EVENT_SHOW_SHADOW = 1 << 10;   /* toggle show shadow */

    /* Number of tetromino types */
    private static final int TETROMINO_TYPES = 7;

    /* We are going to store the tetromino cells in a square matrix */
    /* of this size (this is the size of the biggest tetromino)     */
    public static final int TETROMINO_SIZE = 4;

    /* Tetromino definitions (used as indexes: must be between 0 - [TETROMINO_TYPES - 1])
     * http://tetris.wikia.com/wiki/Tetromino */
    /*
        ....
        ****
        ....
        ....
    */
    public static final int TETROMINO_I = 0;
    /*
        **..
        **..
        ....
        ....
    */
    public static final int TETROMINO_O = 1;
    /*
        .*..
        ***.
        ....
        ....
    */
    public static final int TETROMINO_T = 2;
    /*
        .**.
        **..
        ....
        ....
    */
    public static final int TETROMINO_S = 3;
    /*
        **..
        .**.
        ....
        ....
    */
    public static final int TETROMINO_Z = 4;
    /*
        *...
        ***.
        ....
        ....
    */
    public static final int TETROMINO_J = 5;
    /*
        ..*.
        ***.
        ....
        ....
    */
    public static final int TETROMINO_L = 6;

    /* Tetromino color indexes (must be between 0 - TETROMINO_TYPES) */
    public static final int COLOR_CYAN   = 1;
    public static final int COLOR_RED    = 2;
    public static final int COLOR_BLUE   = 3;
    public static final int COLOR_ORANGE = 4;
    public static final int COLOR_GREEN  = 5;
    public static final int COLOR_YELLOW = 6;
    public static final int COLOR_PURPLE = 7;
    public static final int COLOR_WHITE  = 0;	/* Used for effects (if any) */

    /* This value used for empty tiles */
    public static final int EMPTY_CELL = -1;

    /* Data structure that holds information about our tetromino blocks. */
    public class StcTetromino {
        /*
         *  Tetromino buffer: [x][y]
         *  +---- x
         *  |
         *  |
         *  y
         */
        int[][] cells = new int[TETROMINO_SIZE][TETROMINO_SIZE];
        int x;
        int y;
        int size;
        int type;
    };

    // Statistic data.
    public class Stats {
        long score;         /* user score for current game      */
        int lines;          /* total number of lines cleared    */
        int totalPieces;    /* total number of tetrominoes used */
        int level;          /* current game level               */
        int[] pieces = new int[TETROMINO_TYPES];    /* number of tetrominoes per type */
    };
    public Stats stats;

    /*
     * Matrix that holds the cells (tilemap)
     */
    public int[][] map = new int[BOARD_WIDTH][BOARD_HEIGHT];

    /*
     * Game events are stored in bits in this variable.
     * It must be cleared to EVENT_NONE after being used.
     */
    public int events;

    public StcTetromino nextBlock;     /* next tetromino               */
    public StcTetromino fallingBlock;  /* current falling tetromino    */
    public int errorCode;              /* game error code              */
    public boolean isPaused;    /* true if the game is paused, false otherwise */
    public boolean showPreview; /* true if we must show preview tetromino      */
    public boolean showShadow;  /* true if we must show ghost shadow           */
    public int shadowGap;   /* height gap between shadow and falling tetromino */
    public boolean stateChanged;    /* true if game state changed, 0 otherwise */

    private Platform platform;          /* platform interface           */
    private long systemTime;            /* system time in milliseconds  */
    private int delay;          /* delay time for falling tetrominoes   */
    private boolean isOver;     /* true if the game is over, false otherwise      */
    private long lastFallTime;  /* last time the game moved the falling tetromino */


    /* Initializes the game, if there are no problems returns GAME_ERROR_NONE.*/
    public void init(Platform targetPlatform) {
        stats = new Stats();
        nextBlock = new StcTetromino();
        fallingBlock = new StcTetromino();

        platform = targetPlatform;
        start();
    }

    /* Free used resources */    
    public void end() {
        platform = null;
    }
    
    /* Main function game called every frame */
    public void update() {
        /* Update game state */
        if (isOver) {
            if ((events & EVENT_RESTART) != 0) {
                isOver = false;
                start();
            }
        }
        else {
            long sysTime = platform.getSystemTime();
            
            /* Always handle pause event */
            if ((events & EVENT_PAUSE) != 0) {
                isPaused = !isPaused;
                events = EVENT_NONE;
            }

            /* Check if the game is paused */
            if (isPaused) {
                /* We achieve the effect of pausing the game
                 * adding the last frame duration to lastFallTime */
                lastFallTime += (sysTime - systemTime);
            }
            else {
                if (events != EVENT_NONE) {
                    if ((events & EVENT_SHOW_NEXT) != 0) {
                        showPreview = !showPreview;
                        stateChanged = true;
                    }
                    if (STC_SHOW_GHOST_PIECE) {
	                if ((events & EVENT_SHOW_SHADOW) != 0) {
	                    showShadow = !showShadow;
	                    stateChanged = true;
	                }
                    }
                    if ((events & EVENT_DROP) != 0) {
                        dropTetromino();
                    }
                    if ((events & EVENT_ROTATE_CW) != 0) {
                        rotateTetromino(true);
                    }
                    if ((events & EVENT_MOVE_RIGHT) != 0) {
                        moveTetromino(1, 0);
                    }
                    else if ((events & EVENT_MOVE_LEFT) != 0) {
                        moveTetromino(-1, 0);
                    }
                    if ((events & EVENT_MOVE_DOWN) != 0) {
                        /* Update score if the user accelerates downfall */
                        stats.score += (long)(SCORE_MOVE_DOWN_FACTOR * (SCORE_2_FILLED_ROW * (stats.level + 1)));

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
    }

    /* Set matrix elements to indicated value */    
    private void setMatrixCells(int[][] matrix, int width, int height, int value) {
        for (int i = 0; i < width; ++i) {
            for (int j = 0; j < height; ++j) {
                matrix[i][j] = value;
            }
        }
    }
    
    /* Initialize tetromino cells for every type of tetromino */    
    private void setTetromino(int indexTetromino, StcTetromino tetromino) {

        /* Initialize tetromino cells to empty cells */
        setMatrixCells(tetromino.cells, TETROMINO_SIZE, TETROMINO_SIZE, EMPTY_CELL);

        /* Almost all the blocks have size 3 */
        tetromino.size = TETROMINO_SIZE - 1;

        /* Initial configuration from: http://tetris.wikia.com/wiki/SRS */
        switch (indexTetromino) {
        case TETROMINO_I:
            tetromino.cells[0][1] = COLOR_CYAN;
            tetromino.cells[1][1] = COLOR_CYAN;
            tetromino.cells[2][1] = COLOR_CYAN;
            tetromino.cells[3][1] = COLOR_CYAN;
            tetromino.size = TETROMINO_SIZE;
            break;
        case TETROMINO_O:
            tetromino.cells[0][0] = COLOR_YELLOW;
            tetromino.cells[0][1] = COLOR_YELLOW;
            tetromino.cells[1][0] = COLOR_YELLOW;
            tetromino.cells[1][1] = COLOR_YELLOW;
            tetromino.size = TETROMINO_SIZE - 2;
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

    /* Start a new game */    
    private void start() {

        /* Initialize game data */
        errorCode = GAME_ERROR_NONE;
        systemTime = platform.getSystemTime();
        lastFallTime = systemTime;
        isOver = false;
        isPaused = false;
        showPreview = true;
        events = EVENT_NONE;
        delay = INIT_DELAY_FALL;
        if (STC_SHOW_GHOST_PIECE) {
            showShadow = true;
        }

        /* Initialize game statistics */
        stats.score = 0;
        stats.lines = 0;
        stats.totalPieces = 0;
        stats.level = 0;
        for (int i = 0; i < TETROMINO_TYPES; ++i) {
            stats.pieces[i] = 0;
        }

        /* Initialize random generator */
        platform.seedRandom(systemTime);

        /* Initialize game tile map */
        setMatrixCells(map, BOARD_WIDTH, BOARD_HEIGHT, EMPTY_CELL);

        /* Initialize falling tetromino */
        setTetromino(platform.random() % TETROMINO_TYPES, fallingBlock);
        fallingBlock.x = (BOARD_WIDTH - fallingBlock.size) / 2;
        fallingBlock.y = 0;

        /* Initialize preview tetromino */
        setTetromino(platform.random() % TETROMINO_TYPES, nextBlock);

        /* Initialize events */
        onTetrominoMoved();
    }
    
    /*
     * Rotate falling tetromino. If there are no collisions when the 
     * tetromino is rotated this modifies the tetromino's cell buffer.
     */
    private void rotateTetromino(boolean clockwise) {
        int i, j;
        int[][] rotated = new int[TETROMINO_SIZE][TETROMINO_SIZE];  /* temporary array to hold rotated cells */

        /* If TETROMINO_O is falling return immediately */
        if (fallingBlock.type == TETROMINO_O) {
            return; /* rotation doesn't require any changes */
        }

        /* Initialize rotated cells to blank */
        setMatrixCells(rotated, TETROMINO_SIZE, TETROMINO_SIZE, EMPTY_CELL);

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
        
		if (STC_WALL_KICK_ENABLED) {
	        int wallDisplace = 0;
	
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
		}
		else {
	        /* Check collision of the temporary array */
	        for (i = 0; i < fallingBlock.size; ++i) {
	            for (j = 0; j < fallingBlock.size; ++j) {
	                if (rotated[i][j] != EMPTY_CELL) {
	                    /* Check collision with left, right or bottom borders of the map */
	                    if ((fallingBlock.x + i < 0) || (fallingBlock.x + i >= BOARD_WIDTH)
	                            || (fallingBlock.y + j >= BOARD_HEIGHT)) {
	                        return; /* there was collision therefore return */
	                    }
	                    /* Check collision with existing cells in the map */
	                    if (map[i + fallingBlock.x][j + fallingBlock.y] != EMPTY_CELL) {
	                        return; /* there was collision therefore return */
	                    }
	                }
	            }
	        }
		}
        /* There are no collisions, replace tetromino cells with rotated cells */
        for (i = 0; i < TETROMINO_SIZE; ++i) {
            for (j = 0; j < TETROMINO_SIZE; ++j) {
                fallingBlock.cells[i][j] = rotated[i][j];
            }
        }
        onTetrominoMoved();
    }

    /*
     * Check if tetromino will collide with something if it is moved in the requested direction.
     * If there are collisions returns true else returns false.
     */
    private boolean checkCollision(int dx, int dy) {

        int newx = fallingBlock.x + dx;
        int newy = fallingBlock.y + dy;

        for (int i = 0; i < fallingBlock.size; ++i) {
            for (int j = 0; j < fallingBlock.size; ++j) {
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
    
    /* Game scoring: http://tetris.wikia.com/wiki/Scoring */    
    private void onFilledRows(int filledRows) {
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
            delay = (int)(delay * DELAY_FACTOR_FOR_LEVEL_UP);
        }
    }
    
    /*
     * Move tetromino in the direction specified by (x, y) (in tile units)
     * This function detects if there are filled rows or if the move 
     * lands a falling tetromino, also checks for game over condition.
     */
    private void moveTetromino(int x, int y) {
        int i, j;
        
        /* Check if the move would create a collision */
        if (checkCollision(x, y)) {
            /* In case of collision check if move was downwards (y == 1) */
            if (y == 1) {
                /* Check if collision occurs when the falling 
                 * tetromino is on the 1st or 2nd row */
                if (fallingBlock.y <= 1) {
                    isOver = true;   /* if this happens the game is over */
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
                    int numFilledRows = 0;
                    for (j = 1; j < BOARD_HEIGHT; ++j) {
                        boolean hasFullRow = true;
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
                    if (numFilledRows > 0) {
                        onFilledRows(numFilledRows);
                    }
                    stats.totalPieces++;
                    stats.pieces[fallingBlock.type]++;
                    
                    /* Use preview tetromino as falling tetromino.
                     * Copy preview tetromino for falling tetromino */
                    for (i = 0; i < TETROMINO_SIZE; ++i) {
                        for (j = 0; j < TETROMINO_SIZE; ++j) {
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
                    setTetromino(platform.random() % TETROMINO_TYPES, nextBlock);
                }
            }
        }
        else {
            /* There are no collisions, just move the tetromino */
            fallingBlock.x += x;
            fallingBlock.y += y;
        }
        onTetrominoMoved();
    }

    /* Hard drop */    
    private void dropTetromino() {
    	if (STC_SHOW_GHOST_PIECE) {
	        moveTetromino(0, shadowGap);
	        moveTetromino(0, 1); /* Force lock */
	
	        /* Update score */
	        if (showShadow) {
	            stats.score += (long)(SCORE_DROP_WITH_SHADOW_FACTOR * SCORE_2_FILLED_ROW * (stats.level + 1));
	        }
	        else {
	            stats.score += (long)(SCORE_DROP_FACTOR * SCORE_2_FILLED_ROW * (stats.level + 1));
	        }
    	}
    	else {
	        int y = 0;
	        /* Calculate number of cells to drop */
	        while (!checkCollision(0, ++y));
	        moveTetromino(0, y - 1);
	        moveTetromino(0, 1); /* Force lock */
	
	        /* Update score */
	        stats.score += (long)(SCORE_DROP_FACTOR * SCORE_2_FILLED_ROW * (stats.level + 1));
    	}
    }

    /* This event is called when the falling tetromino is moved */    
    private void onTetrominoMoved() {
    	if (STC_SHOW_GHOST_PIECE) {
            int y = 0;
	    /* Calculate number of cells where shadow tetromino would be */
	    while (!checkCollision(0, ++y));
	    shadowGap = y - 1;
    	}
        stateChanged = true;
    }
}
