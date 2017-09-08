/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   A simple tetris clone.                                                   */
/*                                                                            */
/*   Copyright (c) 2015 Laurens Rodriguez Oscanoa.                            */
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

module Stc {

    export interface Platform {
        // Initializes platform
        init( game: Game ): number;

        // Clear resources used by platform
        end(): void;

        // Process events and notify game
        processEvents(): void;

        // Render the state of the game
        renderGame(): void;

        // Return the current system time in milliseconds
        getSystemTime(): number;

        // Return a random positive integer number
        random(): number;
    }

    // Data structure that holds information about our tetromino blocks.
    export class StcTetromino {
        //
        //  Tetromino buffer:[x][y]
        //  +---- x
        //  |
        //  |
        //  y
        //
        public cells: number[][];

        // Position (x, y)
        public x: number;
        public y: number;

        public size: number;
        public type: number;

        constructor() {
            this.cells = [];
            for ( var k = 0; k < Game.TETROMINO_SIZE; ++k ) {
                this.cells[k] = [];
            }
        }
    }

    // Datas structure for statistical data
    export class StcStatics {
        // User score for current game
        public score: number;
        // Total number of lines cleared
        public lines: number;
        // Total number of tetrominoes used                  
        public totalPieces: number;
        // Current game level
        public level: number;
        // Number of tetrominoes per type
        public pieces: number[];

        constructor() {
            this.pieces = [];
        }
    }

    export class Game {
        //--------------------------------------------------------------------------
        // Game configuration.
        // Edit this section to change the sizes, scores or pace of the game.

        // Playfield size (in tiles)
        public static BOARD_TILEMAP_WIDTH = 10;
        public static BOARD_TILEMAP_HEIGHT = 22;

        // Initial time delay (in milliseconds) between falling moves.
        public static INIT_DELAY_FALL = 1000;

        // Score points given by filled rows (we use the original NES * 10)
        // http://tetris.wikia.com/wiki/Scoring
        public static SCORE_1_FILLED_ROW = 400;
        public static SCORE_2_FILLED_ROW = 1000;
        public static SCORE_3_FILLED_ROW = 3000;
        public static SCORE_4_FILLED_ROW = 12000;

        // The player gets points every time he accelerates downfall.
        // The added points are equal to SCORE_2_FILLED_ROW divided by this value.
        public static SCORE_MOVE_DOWN_DIVISOR = 1000;

        // The player gets points every time he does a hard drop.
        // The added points are equal to SCORE_2_FILLED_ROW divided by these
        // values. If the player is not using the shadow he gets more points.
        public static SCORE_DROP_DIVISOR = 20;
        public static SCORE_DROP_WITH_SHADOW_DIVISOR = 100;

        // Number of filled rows required to increase the game level.
        public static FILLED_ROWS_FOR_LEVEL_UP = 10;

        // The falling delay is multiplied and divided by
        // these factors with every level up.
        public static DELAY_FACTOR_FOR_LEVEL_UP = 9;
        public static DELAY_DIVISOR_FOR_LEVEL_UP = 10;

        // Delayed autoshift initial delay.
        public static DAS_DELAY_TIMER = 200;

        // Delayed autoshift timer for left and right moves.
        public static DAS_MOVE_TIMER = 40;

        // Rotation auto-repeat delay.
        public static ROTATION_AUTOREPEAT_DELAY = 375;

        // Rotation autorepeat timer.
        public static ROTATION_AUTOREPEAT_TIMER = 200;

        //--------------------------------------------------------------------------
        // Game constants. You likely don't need to change this section unless
        // you're changing the gameplay.

        // Error codes
        public static ERROR_NONE = 0;           // Everything is OK, oh wonders!
        public static ERROR_PLAYER_QUITS = 1;   // The user quits, our fail
        public static ERROR_NO_MEMORY = -1;     // Not enough memory
        public static ERROR_NO_VIDEO = -2;      // Video system was not initialized
        public static ERROR_NO_IMAGES = -3;     // Problem loading the image files
        public static ERROR_PLATFORM = -4;      // Problem creating platform
        public static ERROR_ASSERT = -100;      // Something went very very wrong...

        // Game events
        public static EVENT_NONE = 0;
        public static EVENT_MOVE_DOWN = 1;
        public static EVENT_MOVE_LEFT = 1 << 1;
        public static EVENT_MOVE_RIGHT = 1 << 2;
        public static EVENT_ROTATE_CW = 1 << 3;   // rotate clockwise
        public static EVENT_ROTATE_CCW = 1 << 4;  // rotate counter-clockwise
        public static EVENT_DROP = 1 << 5;
        public static EVENT_PAUSE = 1 << 6;
        public static EVENT_RESTART = 1 << 7;
        public static EVENT_SHOW_NEXT = 1 << 8;   // toggle show next tetromino
        public static EVENT_SHOW_SHADOW = 1 << 9; // toggle show shadow
        public static EVENT_QUIT = 1 << 10;       // finish the game

        // We are going to store the tetromino cells in a square matrix
        // of this size (this is the size of the biggest tetromino)
        public static TETROMINO_SIZE = 4;

        // Number of tetromino types.
        public static TETROMINO_TYPES = 7;

        // Tetromino definitions.
        // They are indexes and must be between:0 - [TETROMINO_TYPES - 1]
        // http://tetris.wikia.com/wiki/Tetromino
        // Initial cell disposition is commented below.

        //
        //              ....
        //              ####
        //              ....
        //              ....
        //
        public static TETROMINO_I = 0;
        //
        //              ##..
        //              ##..
        //              ....
        //              ....
        //
        public static TETROMINO_O = 1;
        //
        //              .#..
        //              ###.
        //              ....
        //              ....
        //
        public static TETROMINO_T = 2;
        //
        //              .##.
        //              ##..
        //              ....
        //              ....
        //
        public static TETROMINO_S = 3;
        //
        //              ##..
        //              .##.
        //              ....
        //              ....
        //
        public static TETROMINO_Z = 4;
        //
        //              #...
        //              ###.
        //              ....
        //              ....
        //
        public static TETROMINO_J = 5;
        //
        //              ..#.
        //              ###.
        //              ....
        //              ....
        //
        public static TETROMINO_L = 6;

        // Color indexes
        public static COLOR_CYAN = 1;
        public static COLOR_RED = 2;
        public static COLOR_BLUE = 3;
        public static COLOR_ORANGE = 4;
        public static COLOR_GREEN = 5;
        public static COLOR_YELLOW = 6;
        public static COLOR_PURPLE = 7;
        public static COLOR_WHITE = 0;  // Used for effects (if any)

        // This value used for empty tiles
        public static EMPTY_CELL = -1;

        // The platform must call this method after processing a changed state
        public onChangeProcessed() { this.m_stateChanged = false; }

        // Return true if the game state has changed, false otherwise
        public hasChanged() { return this.m_stateChanged; }

        // Return true if the game state has changed, false otherwise
        public setChanged( changed: boolean ) { this.m_stateChanged = changed; }

        // Return the cell at the specified position
        public getCell( column: number, row: number ) { return this.m_map[column][row]; }

        // Return a reference to the game statistic data
        public stats() { return this.m_stats; }

        // Return current falling tetromino
        public fallingBlock() { return this.m_fallingBlock; }

        // Return next tetromino
        public nextBlock() { return this.m_nextBlock; }

        // Return current error code
        public errorCode() { return this.m_errorCode; }

        // Return true if the game is paused, false otherwise
        public isPaused() { return this.m_isPaused; }

        // Return true if the game has finished, false otherwise
        public isOver() { return this.m_isOver; }

        // Return true if we must show preview tetromino
        public showPreview() { return this.m_showPreview; }

        // Return true if we must show ghost shadow
        public showShadow() { return this.m_showShadow; }

        // Return height gap between shadow and falling tetromino
        public shadowGap() { return this.m_shadowGap; }

        constructor() {
            this.m_map = [];
            for ( var k = 0; k < Game.BOARD_TILEMAP_WIDTH; ++k ) {
                this.m_map[k] = [];
            }
            this.m_fallingBlock = new StcTetromino();
            this.m_nextBlock = new StcTetromino();
        }

        // Initialize the game. The error code (if any) is saved in [m_errorCode].
        public init( targetPlatform: Platform ) {

            // Store platform reference and start it
            this.m_platform = targetPlatform;

            // Initialize platform
            this.m_errorCode = this.m_platform.init( this );

            if ( this.m_errorCode == Game.ERROR_NONE ) {
                // If everything is OK start the game
                this.start();
            }
        }

        // Free used resources
        public end() {
            this.m_platform.end();
        }

        // Main function game called every frame
        public update() {
            // Read player input
            this.m_platform.processEvents();

            // Update game state
            if ( this.m_isOver ) {
                if ( ( this.m_events & Game.EVENT_RESTART ) != 0 ) {
                    this.m_isOver = false;
                    this.start();
                }
            }
            else {
                // Always handle restart event
                if ( ( this.m_events & Game.EVENT_RESTART ) != 0 ) {
                    this.start();
                    return;
                }

                var currentTime = this.m_platform.getSystemTime();

                // Process delayed autoshift
                var timeDelta = currentTime - this.m_systemTime;
                if ( this.m_delayDown > 0 ) {
                    this.m_delayDown -= timeDelta;
                    if ( this.m_delayDown <= 0 ) {
                        this.m_delayDown = Game.DAS_MOVE_TIMER;
                        this.m_events |= Game.EVENT_MOVE_DOWN;
                    }
                }
                if ( this.m_delayLeft > 0 ) {
                    this.m_delayLeft -= timeDelta;
                    if ( this.m_delayLeft <= 0 ) {
                        this.m_delayLeft = Game.DAS_MOVE_TIMER;
                        this.m_events |= Game.EVENT_MOVE_LEFT;
                    }
                }
                else if ( this.m_delayRight > 0 ) {
                    this.m_delayRight -= timeDelta;
                    if ( this.m_delayRight <= 0 ) {
                        this.m_delayRight = Game.DAS_MOVE_TIMER;
                        this.m_events |= Game.EVENT_MOVE_RIGHT;
                    }
                }

                if ( this.m_delayRotation > 0 ) {
                    this.m_delayRotation -= timeDelta;
                    if ( this.m_delayRotation <= 0 ) {
                        this.m_delayRotation = Game.ROTATION_AUTOREPEAT_TIMER;
                        this.m_events |= Game.EVENT_ROTATE_CW;
                    }
                }

                // Always handle pause event
                if ( ( this.m_events & Game.EVENT_PAUSE ) != 0 ) {
                    this.m_isPaused = !this.m_isPaused;
                    this.m_events = Game.EVENT_NONE;
                }

                // Check if the game is paused
                if ( this.m_isPaused ) {
                    // We achieve the effect of pausing the game
                    // adding the last frame duration to lastFallTime
                    this.m_lastFallTime += ( currentTime - this.m_systemTime );
                }
                else {
                    if ( this.m_events != Game.EVENT_NONE ) {
                        if ( ( this.m_events & Game.EVENT_SHOW_NEXT ) != 0 ) {
                            this.m_showPreview = !this.m_showPreview;
                            this.m_stateChanged = true;
                        }

                        if ( ( this.m_events & Game.EVENT_SHOW_SHADOW ) != 0 ) {
                            this.m_showShadow = !this.m_showShadow;
                            this.m_stateChanged = true;
                        }

                        if ( ( this.m_events & Game.EVENT_DROP ) != 0 ) {
                            this.dropTetromino();
                        }

                        if ( ( this.m_events & Game.EVENT_ROTATE_CW ) != 0 ) {
                            this.rotateTetromino( true );
                        }

                        if ( ( this.m_events & Game.EVENT_MOVE_RIGHT ) != 0 ) {
                            this.moveTetromino( 1, 0 );
                        }
                        else if ( ( this.m_events & Game.EVENT_MOVE_LEFT ) != 0 ) {
                            this.moveTetromino( -1, 0 );
                        }

                        if ( ( this.m_events & Game.EVENT_MOVE_DOWN ) != 0 ) {
                            // Update score if the player accelerates downfall
                            this.m_stats.score += Math.floor( Game.SCORE_2_FILLED_ROW * ( this.m_stats.level + 1 )
                                / Game.SCORE_MOVE_DOWN_DIVISOR );

                            this.moveTetromino( 0, 1 );
                        }
                        this.m_events = Game.EVENT_NONE;
                    }

                    // Check if it's time to move downwards the falling tetromino
                    if ( currentTime - this.m_lastFallTime >= this.m_fallingDelay ) {
                        this.moveTetromino( 0, 1 );
                        this.m_lastFallTime = currentTime;
                    }
                }
                // Save current time for next game update
                this.m_systemTime = currentTime;
            }
            // Draw game state
            this.m_platform.renderGame();
        }

        // Process a key down event
        public onEventStart( command: number ) {
            switch ( command ) {
                case Game.EVENT_QUIT:
                    this.m_errorCode = Game.ERROR_PLAYER_QUITS;
                    break;
                case Game.EVENT_MOVE_DOWN:
                    this.m_events |= Game.EVENT_MOVE_DOWN;
                    this.m_delayDown = Game.DAS_DELAY_TIMER;
                    break;
                case Game.EVENT_ROTATE_CW:
                    this.m_events |= Game.EVENT_ROTATE_CW;
                    this.m_delayRotation = Game.ROTATION_AUTOREPEAT_DELAY;
                    break;
                case Game.EVENT_MOVE_LEFT:
                    this.m_events |= Game.EVENT_MOVE_LEFT;
                    this.m_delayLeft = Game.DAS_DELAY_TIMER;
                    break;
                case Game.EVENT_MOVE_RIGHT:
                    this.m_events |= Game.EVENT_MOVE_RIGHT;
                    this.m_delayRight = Game.DAS_DELAY_TIMER;
                    break;
                case Game.EVENT_DROP:       // Fall through
                case Game.EVENT_RESTART:    // Fall through
                case Game.EVENT_PAUSE:      // Fall through
                case Game.EVENT_SHOW_NEXT:  // Fall through
                case Game.EVENT_SHOW_SHADOW:
                    this.m_events |= command;
                    break;
            }
        }

        // Process a key up event
        public onEventEnd( command: number ) {
            switch ( command ) {
                case Game.EVENT_MOVE_DOWN:
                    this.m_delayDown = -1;
                    break;
                case Game.EVENT_MOVE_LEFT:
                    this.m_delayLeft = -1;
                    break;
                case Game.EVENT_MOVE_RIGHT:
                    this.m_delayRight = -1;
                    break;
                case Game.EVENT_ROTATE_CW:
                    this.m_delayRotation = -1;
                    break;
            }
        }

        // Set matrix elements to indicated value
        private setMatrixCells( matrix, width: number, height: number, value: number ) {
            for ( var i = 0; i < width; ++i ) {
                for ( var j = 0; j < height; ++j ) {
                    matrix[i][j] = value;
                }
            }
        }

        // Initialize tetromino cells for every type of tetromino
        private setTetromino( indexTetromino: number, tetromino: StcTetromino ) {
            // Initialize tetromino cells to empty cells
            this.setMatrixCells( tetromino.cells, Game.TETROMINO_SIZE, Game.TETROMINO_SIZE, Game.EMPTY_CELL );

            // Almost all the blocks have size 3
            tetromino.size = Game.TETROMINO_SIZE - 1;

            // Initial configuration from:http://tetris.wikia.com/wiki/SRS
            switch ( indexTetromino ) {
                case Game.TETROMINO_I:
                    tetromino.cells[0][1] = Game.COLOR_CYAN;
                    tetromino.cells[1][1] = Game.COLOR_CYAN;
                    tetromino.cells[2][1] = Game.COLOR_CYAN;
                    tetromino.cells[3][1] = Game.COLOR_CYAN;
                    tetromino.size = Game.TETROMINO_SIZE;
                    break;
                case Game.TETROMINO_O:
                    tetromino.cells[0][0] = Game.COLOR_YELLOW;
                    tetromino.cells[0][1] = Game.COLOR_YELLOW;
                    tetromino.cells[1][0] = Game.COLOR_YELLOW;
                    tetromino.cells[1][1] = Game.COLOR_YELLOW;
                    tetromino.size = Game.TETROMINO_SIZE - 2;
                    break;
                case Game.TETROMINO_T:
                    tetromino.cells[0][1] = Game.COLOR_PURPLE;
                    tetromino.cells[1][0] = Game.COLOR_PURPLE;
                    tetromino.cells[1][1] = Game.COLOR_PURPLE;
                    tetromino.cells[2][1] = Game.COLOR_PURPLE;
                    break;
                case Game.TETROMINO_S:
                    tetromino.cells[0][1] = Game.COLOR_GREEN;
                    tetromino.cells[1][0] = Game.COLOR_GREEN;
                    tetromino.cells[1][1] = Game.COLOR_GREEN;
                    tetromino.cells[2][0] = Game.COLOR_GREEN;
                    break;
                case Game.TETROMINO_Z:
                    tetromino.cells[0][0] = Game.COLOR_RED;
                    tetromino.cells[1][0] = Game.COLOR_RED;
                    tetromino.cells[1][1] = Game.COLOR_RED;
                    tetromino.cells[2][1] = Game.COLOR_RED;
                    break;
                case Game.TETROMINO_J:
                    tetromino.cells[0][0] = Game.COLOR_BLUE;
                    tetromino.cells[0][1] = Game.COLOR_BLUE;
                    tetromino.cells[1][1] = Game.COLOR_BLUE;
                    tetromino.cells[2][1] = Game.COLOR_BLUE;
                    break;
                case Game.TETROMINO_L:
                    tetromino.cells[0][1] = Game.COLOR_ORANGE;
                    tetromino.cells[1][1] = Game.COLOR_ORANGE;
                    tetromino.cells[2][0] = Game.COLOR_ORANGE;
                    tetromino.cells[2][1] = Game.COLOR_ORANGE;
                    break;
            }
            tetromino.type = indexTetromino;
        }

        // Start a new game
        private start() {

            // Initialize game data
            this.m_errorCode = Game.ERROR_NONE;
            this.m_systemTime = this.m_platform.getSystemTime();
            this.m_lastFallTime = this.m_systemTime;
            this.m_isOver = false;
            this.m_isPaused = false;
            this.m_showPreview = true;
            this.m_events = Game.EVENT_NONE;
            this.m_fallingDelay = Game.INIT_DELAY_FALL;
            this.m_showShadow = true;

            // Initialize game statistics
            this.m_stats = new StcStatics();
            this.m_stats.score = 0;
            this.m_stats.lines = 0;
            this.m_stats.totalPieces = 0;
            this.m_stats.level = 0;
            for ( var i = 0; i < Game.TETROMINO_TYPES; ++i ) {
                this.m_stats.pieces[i] = 0;
            }

            // Initialize game tile map
            this.setMatrixCells( this.m_map, Game.BOARD_TILEMAP_WIDTH, Game.BOARD_TILEMAP_HEIGHT, Game.EMPTY_CELL );

            // Initialize falling tetromino
            this.setTetromino( this.m_platform.random() % Game.TETROMINO_TYPES, this.m_fallingBlock );
            this.m_fallingBlock.x = Math.floor( ( Game.BOARD_TILEMAP_WIDTH - this.m_fallingBlock.size ) / 2 );
            this.m_fallingBlock.y = 0;

            // Initialize preview tetromino
            this.setTetromino( this.m_platform.random() % Game.TETROMINO_TYPES, this.m_nextBlock );

            // Initialize events
            this.onTetrominoMoved();

            // Initialize delayed autoshift
            this.m_delayLeft = -1;
            this.m_delayRight = -1;
            this.m_delayDown = -1;
            this.m_delayRotation = -1;
        }

        // Rotate falling tetromino. If there are no collisions when the
        // tetromino is rotated this modifies the tetromino's cell buffer.
        private rotateTetromino( clockwise: boolean ) {
            var i: number, j: number;
            var rotated = []; // temporary array to hold rotated cells
            for ( var k = 0; k < Game.TETROMINO_SIZE; ++k ) {
                rotated[k] = [];
            }

            // If TETROMINO_O is falling return immediately
            if ( this.m_fallingBlock.type == Game.TETROMINO_O ) {
                return; // rotation doesn't require any changes
            }

            // Initialize rotated cells to blank
            this.setMatrixCells( rotated, Game.TETROMINO_SIZE, Game.TETROMINO_SIZE, Game.EMPTY_CELL );

            // Copy rotated cells to the temporary array
            for ( i = 0; i < this.m_fallingBlock.size; ++i ) {
                for ( j = 0; j < this.m_fallingBlock.size; ++j ) {
                    if ( clockwise ) {
                        rotated[this.m_fallingBlock.size - j - 1][i] = this.m_fallingBlock.cells[i][j];
                    }
                    else {
                        rotated[j][this.m_fallingBlock.size - i - 1] = this.m_fallingBlock.cells[i][j];
                    }
                }
            }

            var wallDisplace = 0;

            // Check collision with left wall
            if ( this.m_fallingBlock.x < 0 ) {
                for ( i = 0; ( wallDisplace == 0 ) && ( i < -this.m_fallingBlock.x ); ++i ) {
                    for ( j = 0; j < this.m_fallingBlock.size; ++j ) {
                        if ( rotated[i][j] != Game.EMPTY_CELL ) {
                            wallDisplace = i - this.m_fallingBlock.x;
                            break;
                        }
                    }
                }
            }
            // Or check collision with right wall
            else if ( this.m_fallingBlock.x > Game.BOARD_TILEMAP_WIDTH - this.m_fallingBlock.size ) {
                i = this.m_fallingBlock.size - 1;
                for ( ; ( wallDisplace == 0 ) && ( i >= Game.BOARD_TILEMAP_WIDTH - this.m_fallingBlock.x ); --i ) {
                    for ( j = 0; j < this.m_fallingBlock.size; ++j ) {
                        if ( rotated[i][j] != Game.EMPTY_CELL ) {
                            wallDisplace = -this.m_fallingBlock.x - i + Game.BOARD_TILEMAP_WIDTH - 1;
                            break;
                        }
                    }
                }
            }

            // Check collision with board floor and other cells on board
            for ( i = 0; i < this.m_fallingBlock.size; ++i ) {
                for ( j = 0; j < this.m_fallingBlock.size; ++j ) {
                    if ( rotated[i][j] != Game.EMPTY_CELL ) {
                        // Check collision with bottom border of the map
                        if ( this.m_fallingBlock.y + j >= Game.BOARD_TILEMAP_HEIGHT ) {
                            return; // there was collision therefore return
                        }

                        // Check collision with existing cells in the map
                        if ( this.m_map[i + this.m_fallingBlock.x + wallDisplace][j + this.m_fallingBlock.y] != Game.EMPTY_CELL ) {
                            return; // there was collision therefore return
                        }
                    }
                }
            }

            // Move the falling piece if there was wall collision and it's a legal move
            if ( wallDisplace != 0 ) {
                this.m_fallingBlock.x += wallDisplace;
            }

            // There are no collisions, replace tetromino cells with rotated cells
            for ( i = 0; i < Game.TETROMINO_SIZE; ++i ) {
                for ( j = 0; j < Game.TETROMINO_SIZE; ++j ) {
                    this.m_fallingBlock.cells[i][j] = rotated[i][j];
                }
            }
            this.onTetrominoMoved();
        }

        // Check if tetromino will collide with something if it is moved in the requested direction.
        // If there are collisions returns 1 else returns 0.
        private checkCollision( dx: number, dy: number ) {
            var newx = this.m_fallingBlock.x + dx;
            var newy = this.m_fallingBlock.y + dy;

            for ( var i = 0; i < this.m_fallingBlock.size; ++i ) {
                for ( var j = 0; j < this.m_fallingBlock.size; ++j ) {
                    if ( this.m_fallingBlock.cells[i][j] != Game.EMPTY_CELL ) {
                        // Check the tetromino would be inside the left, right and bottom borders
                        if ( ( newx + i < 0 ) || ( newx + i >= Game.BOARD_TILEMAP_WIDTH )
                            || ( newy + j >= Game.BOARD_TILEMAP_HEIGHT ) ) {
                            return true;
                        }

                        // Check the tetromino won't collide with existing cells in the map
                        if ( this.m_map[newx + i][newy + j] != Game.EMPTY_CELL ) {
                            return true;
                        }
                    }
                }
            }
            return false;
        }

        // Game scoring:http://tetris.wikia.com/wiki/Scoring
        private onFilledRows( filledRows: number ) {
            // Update total number of filled rows
            this.m_stats.lines += filledRows;

            // Increase score accordingly to the number of filled rows
            switch ( filledRows ) {
                case 1:
                    this.m_stats.score += ( Game.SCORE_1_FILLED_ROW * ( this.m_stats.level + 1 ) );
                    break;
                case 2:
                    this.m_stats.score += ( Game.SCORE_2_FILLED_ROW * ( this.m_stats.level + 1 ) );
                    break;
                case 3:
                    this.m_stats.score += ( Game.SCORE_3_FILLED_ROW * ( this.m_stats.level + 1 ) );
                    break;
                case 4:
                    this.m_stats.score += ( Game.SCORE_4_FILLED_ROW * ( this.m_stats.level + 1 ) );
                    break;
                default:
                    // This shouldn't happen, but if happens kill the game
                    this.m_errorCode = Game.ERROR_ASSERT;
            }

            // Check if we need to update the level
            if ( this.m_stats.lines >= Game.FILLED_ROWS_FOR_LEVEL_UP * ( this.m_stats.level + 1 ) ) {
                this.m_stats.level++;

                // Increase speed for falling tetrominoes
                this.m_fallingDelay = Math.floor( Game.DELAY_FACTOR_FOR_LEVEL_UP * this.m_fallingDelay
                    / Game.DELAY_DIVISOR_FOR_LEVEL_UP );
            }
        }

        // Move tetromino in the direction specified by (x, y) (in tile units)
        // This function detects if there are filled rows or if the move
        // lands a falling tetromino, also checks for game over condition.
        private moveTetromino( x: number, y: number ) {
            var i: number, j: number;

            // Check if the move would create a collision
            if ( this.checkCollision( x, y ) ) {
                // In case of collision check if move was downwards (y == 1)
                if ( y == 1 ) {
                    // Check if collision occurs when the falling
                    // tetromino is on the 1st or 2nd row
                    if ( this.m_fallingBlock.y <= 1 ) {
                        this.m_isOver = true; // if this happens the game is over
                    }
                    else {
                        // The falling tetromino has reached the bottom,
                        // so we copy their cells to the board map
                        for ( i = 0; i < this.m_fallingBlock.size; ++i ) {
                            for ( j = 0; j < this.m_fallingBlock.size; ++j ) {
                                if ( this.m_fallingBlock.cells[i][j] != Game.EMPTY_CELL ) {
                                    this.m_map[this.m_fallingBlock.x + i][this.m_fallingBlock.y + j]
                                    = this.m_fallingBlock.cells[i][j];
                                }
                            }
                        }

                        // Check if the landing tetromino has created full rows
                        var numFilledRows = 0;
                        for ( j = 1; j < Game.BOARD_TILEMAP_HEIGHT; ++j ) {
                            var hasFullRow: boolean = true;
                            for ( i = 0; i < Game.BOARD_TILEMAP_WIDTH; ++i ) {
                                if ( this.m_map[i][j] == Game.EMPTY_CELL ) {
                                    hasFullRow = false;
                                    break;
                                }
                            }

                            // If we found a full row we need to remove that row from the map
                            // we do that by just moving all the above rows one row below
                            if ( hasFullRow ) {
                                for ( x = 0; x < Game.BOARD_TILEMAP_WIDTH; ++x ) {
                                    for ( y = j; y > 0; --y ) {
                                        this.m_map[x][y] = this.m_map[x][y - 1];
                                    }
                                }
                                numFilledRows++; // increase filled row counter
                            }
                        }

                        // Update game statistics
                        if ( numFilledRows > 0 ) {
                            this.onFilledRows( numFilledRows );
                        }
                        this.m_stats.totalPieces++;
                        this.m_stats.pieces[this.m_fallingBlock.type]++;

                        // Use preview tetromino as falling tetromino.
                        // Copy preview tetromino for falling tetromino
                        for ( i = 0; i < Game.TETROMINO_SIZE; ++i ) {
                            for ( j = 0; j < Game.TETROMINO_SIZE; ++j ) {
                                this.m_fallingBlock.cells[i][j] = this.m_nextBlock.cells[i][j];
                            }
                        }
                        this.m_fallingBlock.size = this.m_nextBlock.size;
                        this.m_fallingBlock.type = this.m_nextBlock.type;

                        // Reset position
                        this.m_fallingBlock.y = 0;
                        this.m_fallingBlock.x = Math.floor( ( Game.BOARD_TILEMAP_WIDTH - this.m_fallingBlock.size ) / 2 );
                        this.onTetrominoMoved();

                        // Create next preview tetromino
                        this.setTetromino( this.m_platform.random() % Game.TETROMINO_TYPES, this.m_nextBlock );
                    }
                }
            }
            else {
                // There are no collisions, just move the tetromino
                this.m_fallingBlock.x += x;
                this.m_fallingBlock.y += y;
            }
            this.onTetrominoMoved();
        }

        // Hard drop
        private dropTetromino() {
            // Shadow has already calculated the landing position.
            this.m_fallingBlock.y += this.m_shadowGap;

            // Force lock.
            this.moveTetromino( 0, 1 );

            // Update score
            if ( this.m_showShadow ) {
                this.m_stats.score += Math.floor( Game.SCORE_2_FILLED_ROW * ( this.m_stats.level + 1 )
                    / Game.SCORE_DROP_WITH_SHADOW_DIVISOR );
            }
            else {
                this.m_stats.score += Math.floor( Game.SCORE_2_FILLED_ROW * ( this.m_stats.level + 1 )
                    / Game.SCORE_DROP_DIVISOR );
            }
        }

        // This event is called when the falling tetromino is moved
        private onTetrominoMoved() {
            var y = 0;
            // Calculate number of cells where shadow tetromino would be
            while ( !this.checkCollision( 0, ++y ) );
            this.m_shadowGap = y - 1;
            this.m_stateChanged = true;
        }

        // Game events are stored in bits in this variable.
        // It must be cleared to EVENT_NONE after being used.
        private m_events: number;

        // Matrix that holds the cells (tilemap)
        private m_map: number[][];

        private m_platform: Platform;         // platform interface
        private m_stats: StcStatics;          // statistic data
        private m_fallingBlock: StcTetromino; // current falling tetromino
        private m_nextBlock: StcTetromino;    // next tetromino

        private m_stateChanged: boolean; // true if game state has changed
        private m_errorCode: number;  // stores current error code
        private m_isPaused: boolean;     // true if the game is over
        private m_isOver: boolean;       // true if the game is over
        private m_showPreview: boolean;  // true if we must show the preview block

        private m_showShadow: boolean;   // true if we must show the shadow block
        private m_shadowGap: number;  // distance between falling block and shadow

        private m_systemTime: number;   // system time in milliseconds
        private m_fallingDelay: number; // delay time for falling tetrominoes
        private m_lastFallTime: number; // last time the falling tetromino dropped

        // For delayed autoshift:http://tetris.wikia.com/wiki/DAS
        private m_delayLeft: number;
        private m_delayRight: number;
        private m_delayDown: number;
        private m_delayRotation: number;
    }

    export class PlatformHTML5 implements Platform {
        // UI layout (quantities are expressed in pixels)

        // Screen size
        private static SCREEN_WIDTH = 480;
        private static SCREEN_HEIGHT = 320;

        // Size of square tile
        private static TILE_SIZE = 12;

        // Board up-left corner coordinates
        private static BOARD_X = 180;
        private static BOARD_Y = 28;

        // Preview tetromino position
        private static PREVIEW_X = 112;
        private static PREVIEW_Y = 232;

        // Score position and length on screen
        private static SCORE_X = 72;
        private static SCORE_Y = 86;
        private static SCORE_LENGTH = 10;

        // Lines position and length on screen
        private static LINES_X = 108;
        private static LINES_Y = 68;
        private static LINES_LENGTH = 5;

        // Level position and length on screen
        private static LEVEL_X = 108;
        private static LEVEL_Y = 50;
        private static LEVEL_LENGTH = 5;

        // Tetromino subtotals position
        private static TETROMINO_X = 425;
        private static TETROMINO_L_Y = 79;
        private static TETROMINO_I_Y = 102;
        private static TETROMINO_T_Y = 126;
        private static TETROMINO_S_Y = 150;
        private static TETROMINO_Z_Y = 174;
        private static TETROMINO_O_Y = 198;
        private static TETROMINO_J_Y = 222;

        // Size of subtotals
        private static TETROMINO_LENGTH = 5;

        // Tetromino total position
        private static PIECES_X = 418;
        private static PIECES_Y = 246;
        private static PIECES_LENGTH = 6;

        // Size of number
        private static NUMBER_WIDTH = 7;
        private static NUMBER_HEIGHT = 9;

        // Texture size of compounded images
        private static TEXTURE_SIZE = 512;

        // Frames per seconds
        public static FPS = 35;

        // Touch zones limits
        private static TY_1 = 50;
        private static TY_2 = 270;

        private static TY_DOWN = 70;
        private static TY_DROP = 250;

        private static TX_1 = 160;
        private static TX_2 = 320;

        private static KEY_A = 65;
        private static KEY_W = 87;
        private static KEY_S = 83;
        private static KEY_D = 68;

        private static KEY_SPACE = 32;

        private static KEY_LEFT = 37;
        private static KEY_RIGHT = 39;
        private static KEY_UP = 38;
        private static KEY_DOWN = 40;

        constructor( image: HTMLImageElement ) {

            // http://stackoverflow.com/questions/9038625/detect-if-device-is-ios
            this.m_isIOS = ( navigator.userAgent.match( /(iPad|iPhone|iPod)/i ) ? true : false );

            this.m_image = image;

            // Create background layer.
            var canvasBack = <HTMLCanvasElement>document.createElement( "canvas" );
            canvasBack.width = 480;
            canvasBack.height = 320;
            canvasBack.style.position = "absolute";
            canvasBack.style.left = "0";
            canvasBack.style.top = "0";
            canvasBack.style.zIndex = "0";
            document.body.appendChild( canvasBack );

            // Draw background
            var context = canvasBack.getContext( '2d' );
            context.drawImage( this.m_image, 0, PlatformHTML5.TEXTURE_SIZE - PlatformHTML5.SCREEN_HEIGHT,
                PlatformHTML5.SCREEN_WIDTH, PlatformHTML5.SCREEN_HEIGHT, 0, 0,
                PlatformHTML5.SCREEN_WIDTH, PlatformHTML5.SCREEN_HEIGHT );

            // Create stats layer.
            var canvasStats = <HTMLCanvasElement>document.createElement( "canvas" );
            canvasStats.width = 480;
            canvasStats.height = 320;
            canvasStats.style.position = "absolute";
            canvasStats.style.left = "0";
            canvasStats.style.top = "0";
            canvasStats.style.zIndex = "1";
            document.body.appendChild( canvasStats );
            this.m_canvasStats = canvasStats.getContext( '2d' );

            // Create game layer.
            var canvas = <HTMLCanvasElement>document.createElement( "canvas" );
            canvas.width = 480;
            canvas.height = 320;
            canvas.style.position = "absolute";
            canvas.style.left = "0";
            canvas.style.top = "0";
            canvas.style.zIndex = "2";
            document.body.appendChild( canvas );
            this.m_canvas = canvas.getContext( '2d' );

            // Register events.
            var myself = this;
            function handlerKeyDown( event ) {
                myself.onKeyDown( event );
            }
            window.addEventListener( 'keydown', handlerKeyDown, false );

            function handlerKeyUp( event ) {
                myself.onKeyUp( event );
            }
            window.addEventListener( 'keyup', handlerKeyUp, false );

            function handlerTouchDown( event ) {
                myself.onTouchStart( event );
            }
            if ( this.m_isIOS ) {
                canvas['ontouchstart'] = handlerTouchDown;
            }
            else {
                canvas.onmousedown = handlerTouchDown;
            }

            function handlerTouchEnd( event ) {
                myself.onTouchEnd( event );
            }
            if ( this.m_isIOS ) {
                canvas['ontouchend'] = handlerTouchEnd;
            }
            else {
                canvas.onmouseup = handlerTouchEnd;
            }
        }

        private showOverlay( text: string ): void {
            this.m_canvas.globalAlpha = 0.4;
            this.m_canvas.fillStyle = "rgb(0, 0, 0)";
            this.m_canvas.fillRect( 0, 0, PlatformHTML5.SCREEN_WIDTH, PlatformHTML5.SCREEN_HEIGHT );
            this.m_canvas.globalAlpha = 1;

            this.m_canvas.fillStyle = "white";
            this.m_canvas.font = "20px monospace";
            var textWidth = this.m_canvas.measureText( text ).width;
            this.m_canvas.fillText( text, ( PlatformHTML5.SCREEN_WIDTH - textWidth ) / 2, PlatformHTML5.SCREEN_HEIGHT / 2 );
        }

        private onTouchStart( event ): void {
            var tx = event.layerX;
            var ty = event.layerY;

            if ( tx < PlatformHTML5.TX_1 ) {
                if ( ty < PlatformHTML5.TY_1 ) {
                    this.m_game.onEventStart( Game.EVENT_RESTART );
                }
                else if ( ty < PlatformHTML5.TY_2 ) {
                    this.m_game.onEventStart( Game.EVENT_MOVE_LEFT );
                }
                else {
                    this.m_game.onEventStart( Game.EVENT_SHOW_NEXT );
                }
            }
            else if ( tx < PlatformHTML5.TX_2 ) {
                if ( ty > PlatformHTML5.TY_DROP ) {
                    this.m_game.onEventStart( Game.EVENT_DROP );
                }
                else if ( ty > PlatformHTML5.TY_DOWN ) {
                    this.m_game.onEventStart( Game.EVENT_MOVE_DOWN );
                }
                else {
                    this.m_game.onEventStart( Game.EVENT_ROTATE_CW );
                }
            }
            else {
                if ( ty < PlatformHTML5.TY_1 ) {
                    if ( !this.m_game.isOver() ) {
                        if ( !this.m_game.isPaused() ) {
                            this.showOverlay( "Game is paused" );
                        }
                        else {
                            // Force redraw.
                            this.m_game.setChanged( true );
                            this.renderGame();
                        }
                        this.m_game.onEventStart( Game.EVENT_PAUSE );
                    }
                }
                else if ( ty < PlatformHTML5.TY_2 ) {
                    this.m_game.onEventStart( Game.EVENT_MOVE_RIGHT );
                }
                else {
                    this.m_game.onEventStart( Game.EVENT_SHOW_SHADOW );
                }
            }
            console.info( "-- touchStart:" + tx + " " + ty );
        }

        private onTouchEnd( event ): void {
            this.m_game.onEventEnd( Game.EVENT_MOVE_LEFT );
            this.m_game.onEventEnd( Game.EVENT_MOVE_RIGHT );
            this.m_game.onEventEnd( Game.EVENT_MOVE_DOWN );
            this.m_game.onEventEnd( Game.EVENT_ROTATE_CW );
        }

        private onKeyDown( event ): void {
            var key = ( event.which ) ? event.which : event.keyCode;

            switch ( key ) {
                case PlatformHTML5.KEY_A:
                case PlatformHTML5.KEY_LEFT:
                    this.m_game.onEventStart( Game.EVENT_MOVE_LEFT );
                    break;
                case PlatformHTML5.KEY_D:
                case PlatformHTML5.KEY_RIGHT:
                    this.m_game.onEventStart( Game.EVENT_MOVE_RIGHT );
                    break;
                case PlatformHTML5.KEY_W:
                case PlatformHTML5.KEY_UP:
                    this.m_game.onEventStart( Game.EVENT_ROTATE_CW );
                    break;
                case PlatformHTML5.KEY_S:
                case PlatformHTML5.KEY_DOWN:
                    this.m_game.onEventStart( Game.EVENT_MOVE_DOWN );
                    break;
                case PlatformHTML5.KEY_SPACE:
                    this.m_game.onEventStart( Game.EVENT_DROP );
                    break;
            }
        }

        private onKeyUp( event ): void {
            var key = ( event.which ) ? event.which : event.keyCode;

            switch ( key ) {
                case PlatformHTML5.KEY_LEFT:
                    this.m_game.onEventEnd( Game.EVENT_MOVE_LEFT );
                    break;
                case PlatformHTML5.KEY_RIGHT:
                    this.m_game.onEventEnd( Game.EVENT_MOVE_RIGHT );
                    break;
                case PlatformHTML5.KEY_UP:
                    this.m_game.onEventEnd( Game.EVENT_ROTATE_CW );
                    break;
                case PlatformHTML5.KEY_DOWN:
                    this.m_game.onEventEnd( Game.EVENT_MOVE_DOWN );
                    break;
            }
        }

        // Initializes platform
        public init( game ): number {
            this.m_game = game;
            return Game.ERROR_NONE;
        }

        // Clear resources used by platform
        public end(): void {
            this.m_game = null;
        }

        // Process events and notify game
        public processEvents(): void {
            // Events are handled by document handlers, nothing to do here.
        }

        // Render the state of the game
        public renderGame(): void {
            var i: number, j: number;

            // Check if the game state has changed, if so redraw
            if ( this.m_game.hasChanged() ) {
                // Clear canvas.
                this.m_canvas.clearRect( 0, 0, PlatformHTML5.SCREEN_WIDTH, PlatformHTML5.SCREEN_HEIGHT );

                // Draw preview block
                if ( this.m_game.showPreview() ) {
                    for ( i = 0; i < Game.TETROMINO_SIZE; ++i ) {
                        for ( j = 0; j < Game.TETROMINO_SIZE; ++j ) {
                            if ( this.m_game.nextBlock().cells[i][j] != Game.EMPTY_CELL ) {
                                this.drawTile( PlatformHTML5.PREVIEW_X + ( PlatformHTML5.TILE_SIZE * i ),
                                    PlatformHTML5.PREVIEW_Y + ( PlatformHTML5.TILE_SIZE * j ),
                                    this.m_game.nextBlock().cells[i][j], false );
                            }
                        }
                    }
                }

                // Draw shadow tetromino
                if ( this.m_game.showShadow() && this.m_game.shadowGap() > 0 ) {
                    for ( i = 0; i < Game.TETROMINO_SIZE; ++i ) {
                        for ( j = 0; j < Game.TETROMINO_SIZE; ++j ) {
                            if ( this.m_game.fallingBlock().cells[i][j] != Game.EMPTY_CELL ) {
                                this.drawTile( PlatformHTML5.BOARD_X + ( PlatformHTML5.TILE_SIZE * ( this.m_game.fallingBlock().x + i ) ),
                                    PlatformHTML5.BOARD_Y + ( PlatformHTML5.TILE_SIZE * ( this.m_game.fallingBlock().y + this.m_game.shadowGap() + j ) ),
                                    this.m_game.fallingBlock().cells[i][j], true );
                            }
                        }
                    }
                }
                // Draw the cells in the board
                for ( i = 0; i < Game.BOARD_TILEMAP_WIDTH; ++i ) {
                    for ( j = 0; j < Game.BOARD_TILEMAP_HEIGHT; ++j ) {
                        if ( this.m_game.getCell( i, j ) != Game.EMPTY_CELL ) {
                            this.drawTile( PlatformHTML5.BOARD_X + ( PlatformHTML5.TILE_SIZE * i ),
                                PlatformHTML5.BOARD_Y + ( PlatformHTML5.TILE_SIZE * j ),
                                this.m_game.getCell( i, j ), false );
                        }
                    }
                }

                // Draw falling tetromino
                for ( i = 0; i < Game.TETROMINO_SIZE; ++i ) {
                    for ( j = 0; j < Game.TETROMINO_SIZE; ++j ) {
                        if ( this.m_game.fallingBlock().cells[i][j] != Game.EMPTY_CELL ) {
                            this.drawTile( PlatformHTML5.BOARD_X + ( PlatformHTML5.TILE_SIZE * ( this.m_game.fallingBlock().x + i ) ),
                                PlatformHTML5.BOARD_Y + ( PlatformHTML5.TILE_SIZE * ( this.m_game.fallingBlock().y + j ) ),
                                this.m_game.fallingBlock().cells[i][j], false );
                        }
                    }
                }

                // Draw game statistic data
                if ( !this.m_game.isPaused() ) {
                    // Clear stats canvas.
                    this.m_canvasStats.clearRect( 0, 0, PlatformHTML5.SCREEN_WIDTH, PlatformHTML5.SCREEN_HEIGHT );

                    this.drawNumber( PlatformHTML5.LEVEL_X, PlatformHTML5.LEVEL_Y, this.m_game.stats().level, PlatformHTML5.LEVEL_LENGTH, Game.COLOR_WHITE );
                    this.drawNumber( PlatformHTML5.LINES_X, PlatformHTML5.LINES_Y, this.m_game.stats().lines, PlatformHTML5.LINES_LENGTH, Game.COLOR_WHITE );
                    this.drawNumber( PlatformHTML5.SCORE_X, PlatformHTML5.SCORE_Y, this.m_game.stats().score, PlatformHTML5.SCORE_LENGTH, Game.COLOR_WHITE );

                    this.drawNumber( PlatformHTML5.TETROMINO_X, PlatformHTML5.TETROMINO_L_Y, this.m_game.stats().pieces[Game.TETROMINO_L], PlatformHTML5.TETROMINO_LENGTH, Game.COLOR_ORANGE );
                    this.drawNumber( PlatformHTML5.TETROMINO_X, PlatformHTML5.TETROMINO_I_Y, this.m_game.stats().pieces[Game.TETROMINO_I], PlatformHTML5.TETROMINO_LENGTH, Game.COLOR_CYAN );
                    this.drawNumber( PlatformHTML5.TETROMINO_X, PlatformHTML5.TETROMINO_T_Y, this.m_game.stats().pieces[Game.TETROMINO_T], PlatformHTML5.TETROMINO_LENGTH, Game.COLOR_PURPLE );
                    this.drawNumber( PlatformHTML5.TETROMINO_X, PlatformHTML5.TETROMINO_S_Y, this.m_game.stats().pieces[Game.TETROMINO_S], PlatformHTML5.TETROMINO_LENGTH, Game.COLOR_GREEN );
                    this.drawNumber( PlatformHTML5.TETROMINO_X, PlatformHTML5.TETROMINO_Z_Y, this.m_game.stats().pieces[Game.TETROMINO_Z], PlatformHTML5.TETROMINO_LENGTH, Game.COLOR_RED );
                    this.drawNumber( PlatformHTML5.TETROMINO_X, PlatformHTML5.TETROMINO_O_Y, this.m_game.stats().pieces[Game.TETROMINO_O], PlatformHTML5.TETROMINO_LENGTH, Game.COLOR_YELLOW );
                    this.drawNumber( PlatformHTML5.TETROMINO_X, PlatformHTML5.TETROMINO_J_Y, this.m_game.stats().pieces[Game.TETROMINO_J], PlatformHTML5.TETROMINO_LENGTH, Game.COLOR_BLUE );

                    this.drawNumber( PlatformHTML5.PIECES_X, PlatformHTML5.PIECES_Y, this.m_game.stats().totalPieces, PlatformHTML5.PIECES_LENGTH, Game.COLOR_WHITE );
                }

                if ( this.m_game.isOver() ) {
                    this.showOverlay( "Game is over" );
                }

                // Inform the game that we are done with the changed state
                this.m_game.onChangeProcessed();
            }

        }

        // Return the current system time in milliseconds
        public getSystemTime(): number {
            return Date.now();
        }

        // Return a random positive integer number
        public random(): number {
            // JavaScript maximum integer number is 2^53 = 9007199254740992.
            return Math.floor( 9007199254740992 * Math.random() );
        }

        private drawTile( x: number, y: number, tile: number, shadow: boolean ): void {
            this.m_canvas.drawImage( this.m_image,
                PlatformHTML5.TILE_SIZE * ( shadow ? Game.TETROMINO_TYPES + tile + 1 : tile ), 0,
                PlatformHTML5.TILE_SIZE, PlatformHTML5.TILE_SIZE, x, y,
                PlatformHTML5.TILE_SIZE, PlatformHTML5.TILE_SIZE );
        }

        private drawNumber( x: number, y: number, value: number, length: number, color: number ): void {
            var pos = 0;
            do {
                this.m_canvasStats.drawImage( this.m_image,
                    PlatformHTML5.NUMBER_WIDTH * ( value % 10 ),
                    1 + PlatformHTML5.TILE_SIZE + PlatformHTML5.NUMBER_HEIGHT * color,
                    PlatformHTML5.NUMBER_WIDTH, PlatformHTML5.NUMBER_HEIGHT,
                    x + PlatformHTML5.NUMBER_WIDTH * ( length - pos ), y,
                    PlatformHTML5.NUMBER_WIDTH, PlatformHTML5.NUMBER_HEIGHT );

                value = Math.floor( value / 10 );
            } while ( ++pos < length );
        }

        private m_game: Game;
        private m_timeStart: number;
        private m_canvasStats: any;
        private m_canvas: any;
        private m_image: HTMLImageElement;

        private m_isIOS: boolean;
    }
}

window.onload = () => {
    // [iPhone] hiding Safari user interface components
    // http://stackoverflow.com/questions/273671/hiding-safari-user-interface-components-on-iphone
    setTimeout( function() { window.scrollTo( 0, 1 ); }, 0 );

    // Start loading the image with all game elements.
    var image = new Image();
    image.src = "stc_sprites.png"

    // Start game after image has loaded.
    image.onload = function() {
        var platform = new Stc.PlatformHTML5( image );
        var game = new Stc.Game();
        game.init( platform );
        function update() {
            game.update();
        }
        setInterval( update, 1000 / Stc.PlatformHTML5.FPS );
    };
};
