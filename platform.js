/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   A simple tetris clone.                                                   */
/*                                                                            */
/*   Copyright (c) 2012 Laurens Rodriguez Oscanoa.                            */
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
var Stc;
(function (Stc) {
    // Clear resources used by platform
    // Process events and notify game
    // Render the state of the game
    // Return the current system time in milliseconds
    // Return a random positive integer number
    // Data structure that holds information about our tetromino blocks.
    var StcTetromino = (function () {
        function StcTetromino() {
            this.cells = [];
            for(var k = 0; k < Game.TETROMINO_SIZE; ++k) {
                this.cells[k] = [];
            }
        }
        return StcTetromino;
    })();
    Stc.StcTetromino = StcTetromino;    
    // Datas structure for statistical data
    var StcStatics = (function () {
        function StcStatics() {
            this.pieces = [];
        }
        return StcStatics;
    })();
    Stc.StcStatics = StcStatics;    
    var Game = (function () {
        function Game() {
            this.m_map = [];
            for(var k = 0; k < Game.BOARD_TILEMAP_WIDTH; ++k) {
                this.m_map[k] = [];
            }
            this.m_fallingBlock = new StcTetromino();
            this.m_nextBlock = new StcTetromino();
        }
        // Initialize the game. The error code (if any) is saved in [m_errorCode].
                Game.BOARD_TILEMAP_WIDTH = 10;
        Game.BOARD_TILEMAP_HEIGHT = 22;
        Game.INIT_DELAY_FALL = 1000;
        Game.SCORE_1_FILLED_ROW = 400;
        Game.SCORE_2_FILLED_ROW = 1000;
        Game.SCORE_3_FILLED_ROW = 3000;
        Game.SCORE_4_FILLED_ROW = 12000;
        Game.SCORE_MOVE_DOWN_DIVISOR = 1000;
        Game.SCORE_DROP_DIVISOR = 20;
        Game.SCORE_DROP_WITH_SHADOW_DIVISOR = 100;
        Game.FILLED_ROWS_FOR_LEVEL_UP = 10;
        Game.DELAY_FACTOR_FOR_LEVEL_UP = 9;
        Game.DELAY_DIVISOR_FOR_LEVEL_UP = 10;
        Game.DAS_DELAY_TIMER = 200;
        Game.DAS_MOVE_TIMER = 40;
        Game.ROTATION_AUTOREPEAT_DELAY = 375;
        Game.ROTATION_AUTOREPEAT_TIMER = 200;
        Game.ERROR_NONE = 0;
        Game.ERROR_PLAYER_QUITS = 1;
        Game.ERROR_NO_MEMORY = -1;
        Game.ERROR_NO_VIDEO = -2;
        Game.ERROR_NO_IMAGES = -3;
        Game.ERROR_PLATFORM = -4;
        Game.ERROR_ASSERT = -100;
        Game.EVENT_NONE = 0;
        Game.EVENT_MOVE_DOWN = 1;
        Game.EVENT_MOVE_LEFT = 1 << 1;
        Game.EVENT_MOVE_RIGHT = 1 << 2;
        Game.EVENT_ROTATE_CW = 1 << 3;
        Game.EVENT_ROTATE_CCW = 1 << 4;
        Game.EVENT_DROP = 1 << 5;
        Game.EVENT_PAUSE = 1 << 6;
        Game.EVENT_RESTART = 1 << 7;
        Game.EVENT_SHOW_NEXT = 1 << 8;
        Game.EVENT_SHOW_SHADOW = 1 << 9;
        Game.EVENT_QUIT = 1 << 10;
        Game.TETROMINO_SIZE = 4;
        Game.TETROMINO_TYPES = 7;
        Game.TETROMINO_I = 0;
        Game.TETROMINO_O = 1;
        Game.TETROMINO_T = 2;
        Game.TETROMINO_S = 3;
        Game.TETROMINO_Z = 4;
        Game.TETROMINO_J = 5;
        Game.TETROMINO_L = 6;
        Game.COLOR_CYAN = 1;
        Game.COLOR_RED = 2;
        Game.COLOR_BLUE = 3;
        Game.COLOR_ORANGE = 4;
        Game.COLOR_GREEN = 5;
        Game.COLOR_YELLOW = 6;
        Game.COLOR_PURPLE = 7;
        Game.COLOR_WHITE = 0;
        Game.EMPTY_CELL = -1;
        Game.prototype.onChangeProcessed = // The platform must call this method after processing a changed state
        function () {
            this.m_stateChanged = false;
        }// Return true if the game state has changed, false otherwise
        ;
        Game.prototype.hasChanged = function () {
            return this.m_stateChanged;
        }// Return true if the game state has changed, false otherwise
        ;
        Game.prototype.setChanged = function (changed) {
            this.m_stateChanged = changed;
        }// Return the cell at the specified position
        ;
        Game.prototype.getCell = function (column, row) {
            return this.m_map[column][row];
        }// Return a reference to the game statistic data
        ;
        Game.prototype.stats = function () {
            return this.m_stats;
        }// Return current falling tetromino
        ;
        Game.prototype.fallingBlock = function () {
            return this.m_fallingBlock;
        }// Return next tetromino
        ;
        Game.prototype.nextBlock = function () {
            return this.m_nextBlock;
        }// Return current error code
        ;
        Game.prototype.errorCode = function () {
            return this.m_errorCode;
        }// Return true if the game is paused, false otherwise
        ;
        Game.prototype.isPaused = function () {
            return this.m_isPaused;
        }// Return true if the game has finished, false otherwise
        ;
        Game.prototype.isOver = function () {
            return this.m_isOver;
        }// Return true if we must show preview tetromino
        ;
        Game.prototype.showPreview = function () {
            return this.m_showPreview;
        }// Return true if we must show ghost shadow
        ;
        Game.prototype.showShadow = function () {
            return this.m_showShadow;
        }// Return height gap between shadow and falling tetromino
        ;
        Game.prototype.shadowGap = function () {
            return this.m_shadowGap;
        };
        Game.prototype.init = function (targetPlatform) {
            // Store platform reference and start it
            this.m_platform = targetPlatform;
            // Initialize platform
            this.m_errorCode = this.m_platform.init(this);
            if(this.m_errorCode == Game.ERROR_NONE) {
                // If everything is OK start the game
                this.start();
            }
        };
        Game.prototype.end = // Free used resources
        function () {
            this.m_platform.end();
        }// Main function game called every frame
        ;
        Game.prototype.update = function () {
            // Read player input
            this.m_platform.processEvents();
            // Update game state
            if(this.m_isOver) {
                if((this.m_events & Game.EVENT_RESTART) != 0) {
                    this.m_isOver = false;
                    this.start();
                }
            } else {
                // Always handle restart event
                if((this.m_events & Game.EVENT_RESTART) != 0) {
                    this.start();
                    return;
                }
                var currentTime = this.m_platform.getSystemTime();
                // Process delayed autoshift
                var timeDelta = currentTime - this.m_systemTime;
                if(this.m_delayDown > 0) {
                    this.m_delayDown -= timeDelta;
                    if(this.m_delayDown <= 0) {
                        this.m_delayDown = Game.DAS_MOVE_TIMER;
                        this.m_events |= Game.EVENT_MOVE_DOWN;
                    }
                }
                if(this.m_delayLeft > 0) {
                    this.m_delayLeft -= timeDelta;
                    if(this.m_delayLeft <= 0) {
                        this.m_delayLeft = Game.DAS_MOVE_TIMER;
                        this.m_events |= Game.EVENT_MOVE_LEFT;
                    }
                } else {
                    if(this.m_delayRight > 0) {
                        this.m_delayRight -= timeDelta;
                        if(this.m_delayRight <= 0) {
                            this.m_delayRight = Game.DAS_MOVE_TIMER;
                            this.m_events |= Game.EVENT_MOVE_RIGHT;
                        }
                    }
                }
                if(this.m_delayRotation > 0) {
                    this.m_delayRotation -= timeDelta;
                    if(this.m_delayRotation <= 0) {
                        this.m_delayRotation = Game.ROTATION_AUTOREPEAT_TIMER;
                        this.m_events |= Game.EVENT_ROTATE_CW;
                    }
                }
                // Always handle pause event
                if((this.m_events & Game.EVENT_PAUSE) != 0) {
                    this.m_isPaused = !this.m_isPaused;
                    this.m_events = Game.EVENT_NONE;
                }
                // Check if the game is paused
                if(this.m_isPaused) {
                    // We achieve the effect of pausing the game
                    // adding the last frame duration to lastFallTime
                    this.m_lastFallTime += (currentTime - this.m_systemTime);
                } else {
                    if(this.m_events != Game.EVENT_NONE) {
                        if((this.m_events & Game.EVENT_SHOW_NEXT) != 0) {
                            this.m_showPreview = !this.m_showPreview;
                            this.m_stateChanged = true;
                        }
                        if((this.m_events & Game.EVENT_SHOW_SHADOW) != 0) {
                            this.m_showShadow = !this.m_showShadow;
                            this.m_stateChanged = true;
                        }
                        if((this.m_events & Game.EVENT_DROP) != 0) {
                            this.dropTetromino();
                        }
                        if((this.m_events & Game.EVENT_ROTATE_CW) != 0) {
                            this.rotateTetromino(true);
                        }
                        if((this.m_events & Game.EVENT_MOVE_RIGHT) != 0) {
                            this.moveTetromino(1, 0);
                        } else {
                            if((this.m_events & Game.EVENT_MOVE_LEFT) != 0) {
                                this.moveTetromino(-1, 0);
                            }
                        }
                        if((this.m_events & Game.EVENT_MOVE_DOWN) != 0) {
                            // Update score if the player accelerates downfall
                            this.m_stats.score += Math.floor(Game.SCORE_2_FILLED_ROW * (this.m_stats.level + 1) / Game.SCORE_MOVE_DOWN_DIVISOR);
                            this.moveTetromino(0, 1);
                        }
                        this.m_events = Game.EVENT_NONE;
                    }
                    // Check if it's time to move downwards the falling tetromino
                    if(currentTime - this.m_lastFallTime >= this.m_fallingDelay) {
                        this.moveTetromino(0, 1);
                        this.m_lastFallTime = currentTime;
                    }
                }
                // Save current time for next game update
                this.m_systemTime = currentTime;
            }
            // Draw game state
            this.m_platform.renderGame();
        }// Process a key down event
        ;
        Game.prototype.onEventStart = function (command) {
            switch(command) {
                case Game.EVENT_QUIT: {
                    this.m_errorCode = Game.ERROR_PLAYER_QUITS;
                    break;

                }
                case Game.EVENT_MOVE_DOWN: {
                    this.m_events |= Game.EVENT_MOVE_DOWN;
                    this.m_delayDown = Game.DAS_DELAY_TIMER;
                    break;

                }
                case Game.EVENT_ROTATE_CW: {
                    this.m_events |= Game.EVENT_ROTATE_CW;
                    this.m_delayRotation = Game.ROTATION_AUTOREPEAT_DELAY;
                    break;

                }
                case Game.EVENT_MOVE_LEFT: {
                    this.m_events |= Game.EVENT_MOVE_LEFT;
                    this.m_delayLeft = Game.DAS_DELAY_TIMER;
                    break;

                }
                case Game.EVENT_MOVE_RIGHT: {
                    this.m_events |= Game.EVENT_MOVE_RIGHT;
                    this.m_delayRight = Game.DAS_DELAY_TIMER;
                    break;

                }
                case Game.EVENT_DROP:
                case Game.EVENT_RESTART:
                case Game.EVENT_PAUSE:
                case Game.EVENT_SHOW_NEXT:
                case Game.EVENT_SHOW_SHADOW: {
                    this.m_events |= command;
                    break;

                }
            }
        }// Process a key up event
        ;
        Game.prototype.onEventEnd = function (command) {
            switch(command) {
                case Game.EVENT_MOVE_DOWN: {
                    this.m_delayDown = -1;
                    break;

                }
                case Game.EVENT_MOVE_LEFT: {
                    this.m_delayLeft = -1;
                    break;

                }
                case Game.EVENT_MOVE_RIGHT: {
                    this.m_delayRight = -1;
                    break;

                }
                case Game.EVENT_ROTATE_CW: {
                    this.m_delayRotation = -1;
                    break;

                }
            }
        }// Set matrix elements to indicated value
        ;
        Game.prototype.setMatrixCells = function (matrix, width, height, value) {
            for(var i = 0; i < width; ++i) {
                for(var j = 0; j < height; ++j) {
                    matrix[i][j] = value;
                }
            }
        }// Initialize tetromino cells for every type of tetromino
        ;
        Game.prototype.setTetromino = function (indexTetromino, tetromino) {
            // Initialize tetromino cells to empty cells
            this.setMatrixCells(tetromino.cells, Game.TETROMINO_SIZE, Game.TETROMINO_SIZE, Game.EMPTY_CELL);
            // Almost all the blocks have size 3
            tetromino.size = Game.TETROMINO_SIZE - 1;
            // Initial configuration from:http://tetris.wikia.com/wiki/SRS
            switch(indexTetromino) {
                case Game.TETROMINO_I: {
                    tetromino.cells[0][1] = Game.COLOR_CYAN;
                    tetromino.cells[1][1] = Game.COLOR_CYAN;
                    tetromino.cells[2][1] = Game.COLOR_CYAN;
                    tetromino.cells[3][1] = Game.COLOR_CYAN;
                    tetromino.size = Game.TETROMINO_SIZE;
                    break;

                }
                case Game.TETROMINO_O: {
                    tetromino.cells[0][0] = Game.COLOR_YELLOW;
                    tetromino.cells[0][1] = Game.COLOR_YELLOW;
                    tetromino.cells[1][0] = Game.COLOR_YELLOW;
                    tetromino.cells[1][1] = Game.COLOR_YELLOW;
                    tetromino.size = Game.TETROMINO_SIZE - 2;
                    break;

                }
                case Game.TETROMINO_T: {
                    tetromino.cells[0][1] = Game.COLOR_PURPLE;
                    tetromino.cells[1][0] = Game.COLOR_PURPLE;
                    tetromino.cells[1][1] = Game.COLOR_PURPLE;
                    tetromino.cells[2][1] = Game.COLOR_PURPLE;
                    break;

                }
                case Game.TETROMINO_S: {
                    tetromino.cells[0][1] = Game.COLOR_GREEN;
                    tetromino.cells[1][0] = Game.COLOR_GREEN;
                    tetromino.cells[1][1] = Game.COLOR_GREEN;
                    tetromino.cells[2][0] = Game.COLOR_GREEN;
                    break;

                }
                case Game.TETROMINO_Z: {
                    tetromino.cells[0][0] = Game.COLOR_RED;
                    tetromino.cells[1][0] = Game.COLOR_RED;
                    tetromino.cells[1][1] = Game.COLOR_RED;
                    tetromino.cells[2][1] = Game.COLOR_RED;
                    break;

                }
                case Game.TETROMINO_J: {
                    tetromino.cells[0][0] = Game.COLOR_BLUE;
                    tetromino.cells[0][1] = Game.COLOR_BLUE;
                    tetromino.cells[1][1] = Game.COLOR_BLUE;
                    tetromino.cells[2][1] = Game.COLOR_BLUE;
                    break;

                }
                case Game.TETROMINO_L: {
                    tetromino.cells[0][1] = Game.COLOR_ORANGE;
                    tetromino.cells[1][1] = Game.COLOR_ORANGE;
                    tetromino.cells[2][0] = Game.COLOR_ORANGE;
                    tetromino.cells[2][1] = Game.COLOR_ORANGE;
                    break;

                }
            }
            tetromino.type = indexTetromino;
        }// Start a new game
        ;
        Game.prototype.start = function () {
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
            for(var i = 0; i < Game.TETROMINO_TYPES; ++i) {
                this.m_stats.pieces[i] = 0;
            }
            // Initialize game tile map
            this.setMatrixCells(this.m_map, Game.BOARD_TILEMAP_WIDTH, Game.BOARD_TILEMAP_HEIGHT, Game.EMPTY_CELL);
            // Initialize falling tetromino
            this.setTetromino(this.m_platform.random() % Game.TETROMINO_TYPES, this.m_fallingBlock);
            this.m_fallingBlock.x = Math.floor((Game.BOARD_TILEMAP_WIDTH - this.m_fallingBlock.size) / 2);
            this.m_fallingBlock.y = 0;
            // Initialize preview tetromino
            this.setTetromino(this.m_platform.random() % Game.TETROMINO_TYPES, this.m_nextBlock);
            // Initialize events
            this.onTetrominoMoved();
            // Initialize delayed autoshift
            this.m_delayLeft = -1;
            this.m_delayRight = -1;
            this.m_delayDown = -1;
            this.m_delayRotation = -1;
        }// Rotate falling tetromino. If there are no collisions when the
        // tetromino is rotated this modifies the tetromino's cell buffer.
        ;
        Game.prototype.rotateTetromino = function (clockwise) {
            var i, j;
            var rotated = [];// temporary array to hold rotated cells
            
            for(var k = 0; k < Game.TETROMINO_SIZE; ++k) {
                rotated[k] = [];
            }
            // If TETROMINO_O is falling return immediately
            if(this.m_fallingBlock.type == Game.TETROMINO_O) {
                return;// rotation doesn't require any changes
                
            }
            // Initialize rotated cells to blank
            this.setMatrixCells(rotated, Game.TETROMINO_SIZE, Game.TETROMINO_SIZE, Game.EMPTY_CELL);
            // Copy rotated cells to the temporary array
            for(i = 0; i < this.m_fallingBlock.size; ++i) {
                for(j = 0; j < this.m_fallingBlock.size; ++j) {
                    if(clockwise) {
                        rotated[this.m_fallingBlock.size - j - 1][i] = this.m_fallingBlock.cells[i][j];
                    } else {
                        rotated[j][this.m_fallingBlock.size - i - 1] = this.m_fallingBlock.cells[i][j];
                    }
                }
            }
            var wallDisplace = 0;
            // Check collision with left wall
            if(this.m_fallingBlock.x < 0) {
                for(i = 0; (wallDisplace == 0) && (i < -this.m_fallingBlock.x); ++i) {
                    for(j = 0; j < this.m_fallingBlock.size; ++j) {
                        if(rotated[i][j] != Game.EMPTY_CELL) {
                            wallDisplace = i - this.m_fallingBlock.x;
                            break;
                        }
                    }
                }
            } else {
                // Or check collision with right wall
                if(this.m_fallingBlock.x > Game.BOARD_TILEMAP_WIDTH - this.m_fallingBlock.size) {
                    i = this.m_fallingBlock.size - 1;
                    for(; (wallDisplace == 0) && (i >= Game.BOARD_TILEMAP_WIDTH - this.m_fallingBlock.x); --i) {
                        for(j = 0; j < this.m_fallingBlock.size; ++j) {
                            if(rotated[i][j] != Game.EMPTY_CELL) {
                                wallDisplace = -this.m_fallingBlock.x - i + Game.BOARD_TILEMAP_WIDTH - 1;
                                break;
                            }
                        }
                    }
                }
            }
            // Check collision with board floor and other cells on board
            for(i = 0; i < this.m_fallingBlock.size; ++i) {
                for(j = 0; j < this.m_fallingBlock.size; ++j) {
                    if(rotated[i][j] != Game.EMPTY_CELL) {
                        // Check collision with bottom border of the map
                        if(this.m_fallingBlock.y + j >= Game.BOARD_TILEMAP_HEIGHT) {
                            return;// there was collision therefore return
                            
                        }
                        // Check collision with existing cells in the map
                        if(this.m_map[i + this.m_fallingBlock.x + wallDisplace][j + this.m_fallingBlock.y] != Game.EMPTY_CELL) {
                            return;// there was collision therefore return
                            
                        }
                    }
                }
            }
            // Move the falling piece if there was wall collision and it's a legal move
            if(wallDisplace != 0) {
                this.m_fallingBlock.x += wallDisplace;
            }
            // There are no collisions, replace tetromino cells with rotated cells
            for(i = 0; i < Game.TETROMINO_SIZE; ++i) {
                for(j = 0; j < Game.TETROMINO_SIZE; ++j) {
                    this.m_fallingBlock.cells[i][j] = rotated[i][j];
                }
            }
            this.onTetrominoMoved();
        }// Check if tetromino will collide with something if it is moved in the requested direction.
        // If there are collisions returns 1 else returns 0.
        ;
        Game.prototype.checkCollision = function (dx, dy) {
            var newx = this.m_fallingBlock.x + dx;
            var newy = this.m_fallingBlock.y + dy;
            for(var i = 0; i < this.m_fallingBlock.size; ++i) {
                for(var j = 0; j < this.m_fallingBlock.size; ++j) {
                    if(this.m_fallingBlock.cells[i][j] != Game.EMPTY_CELL) {
                        // Check the tetromino would be inside the left, right and bottom borders
                        if((newx + i < 0) || (newx + i >= Game.BOARD_TILEMAP_WIDTH) || (newy + j >= Game.BOARD_TILEMAP_HEIGHT)) {
                            return true;
                        }
                        // Check the tetromino won't collide with existing cells in the map
                        if(this.m_map[newx + i][newy + j] != Game.EMPTY_CELL) {
                            return true;
                        }
                    }
                }
            }
            return false;
        }// Game scoring:http://tetris.wikia.com/wiki/Scoring
        ;
        Game.prototype.onFilledRows = function (filledRows) {
            // Update total number of filled rows
            this.m_stats.lines += filledRows;
            // Increase score accordingly to the number of filled rows
            switch(filledRows) {
                case 1: {
                    this.m_stats.score += (Game.SCORE_1_FILLED_ROW * (this.m_stats.level + 1));
                    break;

                }
                case 2: {
                    this.m_stats.score += (Game.SCORE_2_FILLED_ROW * (this.m_stats.level + 1));
                    break;

                }
                case 3: {
                    this.m_stats.score += (Game.SCORE_3_FILLED_ROW * (this.m_stats.level + 1));
                    break;

                }
                case 4: {
                    this.m_stats.score += (Game.SCORE_4_FILLED_ROW * (this.m_stats.level + 1));
                    break;

                }
                default: {
                    // This shouldn't happen, but if happens kill the game
                    this.m_errorCode = Game.ERROR_ASSERT;

                }
            }
            // Check if we need to update the level
            if(this.m_stats.lines >= Game.FILLED_ROWS_FOR_LEVEL_UP * (this.m_stats.level + 1)) {
                this.m_stats.level++;
                // Increase speed for falling tetrominoes
                this.m_fallingDelay = Math.floor(Game.DELAY_FACTOR_FOR_LEVEL_UP * this.m_fallingDelay / Game.DELAY_DIVISOR_FOR_LEVEL_UP);
            }
        }// Move tetromino in the direction specified by (x, y) (in tile units)
        // This function detects if there are filled rows or if the move
        // lands a falling tetromino, also checks for game over condition.
        ;
        Game.prototype.moveTetromino = function (x, y) {
            var i, j;
            // Check if the move would create a collision
            if(this.checkCollision(x, y)) {
                // In case of collision check if move was downwards (y == 1)
                if(y == 1) {
                    // Check if collision occurs when the falling
                    // tetromino is on the 1st or 2nd row
                    if(this.m_fallingBlock.y <= 1) {
                        this.m_isOver = true// if this happens the game is over
                        ;
                    } else {
                        // The falling tetromino has reached the bottom,
                        // so we copy their cells to the board map
                        for(i = 0; i < this.m_fallingBlock.size; ++i) {
                            for(j = 0; j < this.m_fallingBlock.size; ++j) {
                                if(this.m_fallingBlock.cells[i][j] != Game.EMPTY_CELL) {
                                    this.m_map[this.m_fallingBlock.x + i][this.m_fallingBlock.y + j] = this.m_fallingBlock.cells[i][j];
                                }
                            }
                        }
                        // Check if the landing tetromino has created full rows
                        var numFilledRows = 0;
                        for(j = 1; j < Game.BOARD_TILEMAP_HEIGHT; ++j) {
                            var hasFullRow = true;
                            for(i = 0; i < Game.BOARD_TILEMAP_WIDTH; ++i) {
                                if(this.m_map[i][j] == Game.EMPTY_CELL) {
                                    hasFullRow = false;
                                    break;
                                }
                            }
                            // If we found a full row we need to remove that row from the map
                            // we do that by just moving all the above rows one row below
                            if(hasFullRow) {
                                for(x = 0; x < Game.BOARD_TILEMAP_WIDTH; ++x) {
                                    for(y = j; y > 0; --y) {
                                        this.m_map[x][y] = this.m_map[x][y - 1];
                                    }
                                }
                                numFilledRows++// increase filled row counter
                                ;
                            }
                        }
                        // Update game statistics
                        if(numFilledRows > 0) {
                            this.onFilledRows(numFilledRows);
                        }
                        this.m_stats.totalPieces++;
                        this.m_stats.pieces[this.m_fallingBlock.type]++;
                        // Use preview tetromino as falling tetromino.
                        // Copy preview tetromino for falling tetromino
                        for(i = 0; i < Game.TETROMINO_SIZE; ++i) {
                            for(j = 0; j < Game.TETROMINO_SIZE; ++j) {
                                this.m_fallingBlock.cells[i][j] = this.m_nextBlock.cells[i][j];
                            }
                        }
                        this.m_fallingBlock.size = this.m_nextBlock.size;
                        this.m_fallingBlock.type = this.m_nextBlock.type;
                        // Reset position
                        this.m_fallingBlock.y = 0;
                        this.m_fallingBlock.x = Math.floor((Game.BOARD_TILEMAP_WIDTH - this.m_fallingBlock.size) / 2);
                        this.onTetrominoMoved();
                        // Create next preview tetromino
                        this.setTetromino(this.m_platform.random() % Game.TETROMINO_TYPES, this.m_nextBlock);
                    }
                }
            } else {
                // There are no collisions, just move the tetromino
                this.m_fallingBlock.x += x;
                this.m_fallingBlock.y += y;
            }
            this.onTetrominoMoved();
        }// Hard drop
        ;
        Game.prototype.dropTetromino = function () {
            // Shadow has already calculated the landing position.
            this.m_fallingBlock.y += this.m_shadowGap;
            // Force lock.
            this.moveTetromino(0, 1);
            // Update score
            if(this.m_showShadow) {
                this.m_stats.score += Math.floor(Game.SCORE_2_FILLED_ROW * (this.m_stats.level + 1) / Game.SCORE_DROP_WITH_SHADOW_DIVISOR);
            } else {
                this.m_stats.score += Math.floor(Game.SCORE_2_FILLED_ROW * (this.m_stats.level + 1) / Game.SCORE_DROP_DIVISOR);
            }
        }// This event is called when the falling tetromino is moved
        ;
        Game.prototype.onTetrominoMoved = function () {
            var y = 0;
            // Calculate number of cells where shadow tetromino would be
            while(!this.checkCollision(0, ++y)) {
                ; ;
            }
            this.m_shadowGap = y - 1;
            this.m_stateChanged = true;
        }// Game events are stored in bits in this variable.
        // It must be cleared to EVENT_NONE after being used.
        ;
        return Game;
    })();
    Stc.Game = Game;    
    var PlatformHTML5 = (function () {
        function PlatformHTML5(image) {
            this.m_image = image;
            // Create background layer.
            var canvasBack = document.createElement("canvas");
            canvasBack.width = 480;
            canvasBack.height = 320;
            canvasBack.style.position = "absolute";
            canvasBack.style.left = "0";
            canvasBack.style.top = "0";
            canvasBack.style.zIndex = "0";
            document.body.appendChild(canvasBack);
            // Draw background
            var context = canvasBack.getContext('2d');
            context.drawImage(this.m_image, 0, PlatformHTML5.TEXTURE_SIZE - PlatformHTML5.SCREEN_HEIGHT, PlatformHTML5.SCREEN_WIDTH, PlatformHTML5.SCREEN_HEIGHT, 0, 0, PlatformHTML5.SCREEN_WIDTH, PlatformHTML5.SCREEN_HEIGHT);
            // Create stats layer.
            var canvasStats = document.createElement("canvas");
            canvasStats.width = 480;
            canvasStats.height = 320;
            canvasStats.style.position = "absolute";
            canvasStats.style.left = "0";
            canvasStats.style.top = "0";
            canvasStats.style.zIndex = "1";
            document.body.appendChild(canvasStats);
            this.m_canvasStats = canvasStats.getContext('2d');
            // Create game layer.
            var canvas = document.createElement("canvas");
            canvas.width = 480;
            canvas.height = 320;
            canvas.style.position = "absolute";
            canvas.style.left = "0";
            canvas.style.top = "0";
            canvas.style.zIndex = "2";
            document.body.appendChild(canvas);
            this.m_canvas = canvas.getContext('2d');
            // Register events.
            var myself = this;
function handlerKeyDown(event) {
                myself.onKeyDown(event);
            }
            window.addEventListener('keydown', handlerKeyDown, false);
function handlerKeyUp(event) {
                myself.onKeyUp(event);
            }
            window.addEventListener('keyup', handlerKeyUp, false);
function handlerTouchDown(event) {
                myself.onTouchStart(event);
            }
            canvas.onmousedown = handlerTouchDown;
            canvas.ontouchstart = handlerTouchDown;
function handlerTouchEnd(event) {
                myself.onTouchEnd(event);
            }
            canvas.onmouseup = handlerTouchEnd;
            canvas.ontouchend = handlerTouchEnd;
        }
        PlatformHTML5.SCREEN_WIDTH = 480;
        PlatformHTML5.SCREEN_HEIGHT = 320;
        PlatformHTML5.TILE_SIZE = 12;
        PlatformHTML5.BOARD_X = 180;
        PlatformHTML5.BOARD_Y = 28;
        PlatformHTML5.PREVIEW_X = 112;
        PlatformHTML5.PREVIEW_Y = 232;
        PlatformHTML5.SCORE_X = 72;
        PlatformHTML5.SCORE_Y = 86;
        PlatformHTML5.SCORE_LENGTH = 10;
        PlatformHTML5.LINES_X = 108;
        PlatformHTML5.LINES_Y = 68;
        PlatformHTML5.LINES_LENGTH = 5;
        PlatformHTML5.LEVEL_X = 108;
        PlatformHTML5.LEVEL_Y = 50;
        PlatformHTML5.LEVEL_LENGTH = 5;
        PlatformHTML5.TETROMINO_X = 425;
        PlatformHTML5.TETROMINO_L_Y = 79;
        PlatformHTML5.TETROMINO_I_Y = 102;
        PlatformHTML5.TETROMINO_T_Y = 126;
        PlatformHTML5.TETROMINO_S_Y = 150;
        PlatformHTML5.TETROMINO_Z_Y = 174;
        PlatformHTML5.TETROMINO_O_Y = 198;
        PlatformHTML5.TETROMINO_J_Y = 222;
        PlatformHTML5.TETROMINO_LENGTH = 5;
        PlatformHTML5.PIECES_X = 418;
        PlatformHTML5.PIECES_Y = 246;
        PlatformHTML5.PIECES_LENGTH = 6;
        PlatformHTML5.NUMBER_WIDTH = 7;
        PlatformHTML5.NUMBER_HEIGHT = 9;
        PlatformHTML5.TEXTURE_SIZE = 512;
        PlatformHTML5.FPS = 35;
        PlatformHTML5.TY_1 = 50;
        PlatformHTML5.TY_2 = 270;
        PlatformHTML5.TY_DOWN = 70;
        PlatformHTML5.TY_DROP = 250;
        PlatformHTML5.TX_1 = 160;
        PlatformHTML5.TX_2 = 320;
        PlatformHTML5.KEY_A = 65;
        PlatformHTML5.KEY_W = 87;
        PlatformHTML5.KEY_S = 83;
        PlatformHTML5.KEY_D = 68;
        PlatformHTML5.KEY_SPACE = 32;
        PlatformHTML5.KEY_LEFT = 37;
        PlatformHTML5.KEY_RIGHT = 39;
        PlatformHTML5.KEY_UP = 38;
        PlatformHTML5.KEY_DOWN = 40;
        PlatformHTML5.prototype.showOverlay = function (text) {
            this.m_canvas.globalAlpha = 0.4;
            this.m_canvas.fillStyle = "rgb(0, 0, 0)";
            this.m_canvas.fillRect(0, 0, PlatformHTML5.SCREEN_WIDTH, PlatformHTML5.SCREEN_HEIGHT);
            this.m_canvas.globalAlpha = 1;
            this.m_canvas.fillStyle = "white";
            this.m_canvas.font = "20px monospace";
            var textWidth = this.m_canvas.measureText(text).width;
            this.m_canvas.fillText(text, (PlatformHTML5.SCREEN_WIDTH - textWidth) / 2, PlatformHTML5.SCREEN_HEIGHT / 2);
        };
        PlatformHTML5.prototype.onTouchStart = function (event) {
            var tx = event.layerX;
            var ty = event.layerY;
            if(tx < PlatformHTML5.TX_1) {
                if(ty < PlatformHTML5.TY_1) {
                    this.m_game.onEventStart(Game.EVENT_RESTART);
                } else {
                    if(ty < PlatformHTML5.TY_2) {
                        this.m_game.onEventStart(Game.EVENT_MOVE_LEFT);
                    } else {
                        this.m_game.onEventStart(Game.EVENT_SHOW_NEXT);
                    }
                }
            } else {
                if(tx < PlatformHTML5.TX_2) {
                    if(ty > PlatformHTML5.TY_DROP) {
                        this.m_game.onEventStart(Game.EVENT_DROP);
                    } else {
                        if(ty > PlatformHTML5.TY_DOWN) {
                            this.m_game.onEventStart(Game.EVENT_MOVE_DOWN);
                        } else {
                            this.m_game.onEventStart(Game.EVENT_ROTATE_CW);
                        }
                    }
                } else {
                    if(ty < PlatformHTML5.TY_1) {
                        if(!this.m_game.isOver()) {
                            if(!this.m_game.isPaused()) {
                                this.showOverlay("Game is paused");
                            } else {
                                // Force redraw.
                                this.m_game.setChanged(true);
                                this.renderGame();
                            }
                            this.m_game.onEventStart(Game.EVENT_PAUSE);
                        }
                    } else {
                        if(ty < PlatformHTML5.TY_2) {
                            this.m_game.onEventStart(Game.EVENT_MOVE_RIGHT);
                        } else {
                            this.m_game.onEventStart(Game.EVENT_SHOW_SHADOW);
                        }
                    }
                }
            }
            console.info("-- touchStart:" + tx + " " + ty);
        };
        PlatformHTML5.prototype.onTouchEnd = function (event) {
            this.m_game.onEventEnd(Game.EVENT_MOVE_LEFT);
            this.m_game.onEventEnd(Game.EVENT_MOVE_RIGHT);
            this.m_game.onEventEnd(Game.EVENT_MOVE_DOWN);
            this.m_game.onEventEnd(Game.EVENT_ROTATE_CW);
        };
        PlatformHTML5.prototype.onKeyDown = function (event) {
            var key = (event.which) ? event.which : event.keyCode;
            switch(key) {
                case PlatformHTML5.KEY_A:
                case PlatformHTML5.KEY_LEFT: {
                    this.m_game.onEventStart(Game.EVENT_MOVE_LEFT);
                    break;

                }
                case PlatformHTML5.KEY_D:
                case PlatformHTML5.KEY_RIGHT: {
                    this.m_game.onEventStart(Game.EVENT_MOVE_RIGHT);
                    break;

                }
                case PlatformHTML5.KEY_W:
                case PlatformHTML5.KEY_UP: {
                    this.m_game.onEventStart(Game.EVENT_ROTATE_CW);
                    break;

                }
                case PlatformHTML5.KEY_S:
                case PlatformHTML5.KEY_DOWN: {
                    this.m_game.onEventStart(Game.EVENT_MOVE_DOWN);
                    break;

                }
                case PlatformHTML5.KEY_SPACE: {
                    this.m_game.onEventStart(Game.EVENT_DROP);
                    break;

                }
            }
        };
        PlatformHTML5.prototype.onKeyUp = function (event) {
            var key = (event.which) ? event.which : event.keyCode;
            switch(key) {
                case PlatformHTML5.KEY_LEFT: {
                    this.m_game.onEventEnd(Game.EVENT_MOVE_LEFT);
                    break;

                }
                case PlatformHTML5.KEY_RIGHT: {
                    this.m_game.onEventEnd(Game.EVENT_MOVE_RIGHT);
                    break;

                }
                case PlatformHTML5.KEY_UP: {
                    this.m_game.onEventEnd(Game.EVENT_ROTATE_CW);
                    break;

                }
                case PlatformHTML5.KEY_DOWN: {
                    this.m_game.onEventEnd(Game.EVENT_MOVE_DOWN);
                    break;

                }
            }
        }// Initializes platform
        ;
        PlatformHTML5.prototype.init = function (game) {
            this.m_game = game;
            return Game.ERROR_NONE;
        }// Clear resources used by platform
        ;
        PlatformHTML5.prototype.end = function () {
            this.m_game = null;
        }// Process events and notify game
        ;
        PlatformHTML5.prototype.processEvents = function () {
            // Events are handled by document handlers, nothing to do here.
                    }// Render the state of the game
        ;
        PlatformHTML5.prototype.renderGame = function () {
            var i, j;
            // Check if the game state has changed, if so redraw
            if(this.m_game.hasChanged()) {
                // Clear canvas.
                this.m_canvas.clearRect(0, 0, PlatformHTML5.SCREEN_WIDTH, PlatformHTML5.SCREEN_HEIGHT);
                // Draw preview block
                if(this.m_game.showPreview()) {
                    for(i = 0; i < Game.TETROMINO_SIZE; ++i) {
                        for(j = 0; j < Game.TETROMINO_SIZE; ++j) {
                            if(this.m_game.nextBlock().cells[i][j] != Game.EMPTY_CELL) {
                                this.drawTile(PlatformHTML5.PREVIEW_X + (PlatformHTML5.TILE_SIZE * i), PlatformHTML5.PREVIEW_Y + (PlatformHTML5.TILE_SIZE * j), this.m_game.nextBlock().cells[i][j], false);
                            }
                        }
                    }
                }
                // Draw shadow tetromino
                if(this.m_game.showShadow() && this.m_game.shadowGap() > 0) {
                    for(i = 0; i < Game.TETROMINO_SIZE; ++i) {
                        for(j = 0; j < Game.TETROMINO_SIZE; ++j) {
                            if(this.m_game.fallingBlock().cells[i][j] != Game.EMPTY_CELL) {
                                this.drawTile(PlatformHTML5.BOARD_X + (PlatformHTML5.TILE_SIZE * (this.m_game.fallingBlock().x + i)), PlatformHTML5.BOARD_Y + (PlatformHTML5.TILE_SIZE * (this.m_game.fallingBlock().y + this.m_game.shadowGap() + j)), this.m_game.fallingBlock().cells[i][j], true);
                            }
                        }
                    }
                }
                // Draw the cells in the board
                for(i = 0; i < Game.BOARD_TILEMAP_WIDTH; ++i) {
                    for(j = 0; j < Game.BOARD_TILEMAP_HEIGHT; ++j) {
                        if(this.m_game.getCell(i, j) != Game.EMPTY_CELL) {
                            this.drawTile(PlatformHTML5.BOARD_X + (PlatformHTML5.TILE_SIZE * i), PlatformHTML5.BOARD_Y + (PlatformHTML5.TILE_SIZE * j), this.m_game.getCell(i, j), false);
                        }
                    }
                }
                // Draw falling tetromino
                for(i = 0; i < Game.TETROMINO_SIZE; ++i) {
                    for(j = 0; j < Game.TETROMINO_SIZE; ++j) {
                        if(this.m_game.fallingBlock().cells[i][j] != Game.EMPTY_CELL) {
                            this.drawTile(PlatformHTML5.BOARD_X + (PlatformHTML5.TILE_SIZE * (this.m_game.fallingBlock().x + i)), PlatformHTML5.BOARD_Y + (PlatformHTML5.TILE_SIZE * (this.m_game.fallingBlock().y + j)), this.m_game.fallingBlock().cells[i][j], false);
                        }
                    }
                }
                // Draw game statistic data
                if(!this.m_game.isPaused()) {
                    // Clear stats canvas.
                    this.m_canvasStats.clearRect(0, 0, PlatformHTML5.SCREEN_WIDTH, PlatformHTML5.SCREEN_HEIGHT);
                    this.drawNumber(PlatformHTML5.LEVEL_X, PlatformHTML5.LEVEL_Y, this.m_game.stats().level, PlatformHTML5.LEVEL_LENGTH, Game.COLOR_WHITE);
                    this.drawNumber(PlatformHTML5.LINES_X, PlatformHTML5.LINES_Y, this.m_game.stats().lines, PlatformHTML5.LINES_LENGTH, Game.COLOR_WHITE);
                    this.drawNumber(PlatformHTML5.SCORE_X, PlatformHTML5.SCORE_Y, this.m_game.stats().score, PlatformHTML5.SCORE_LENGTH, Game.COLOR_WHITE);
                    this.drawNumber(PlatformHTML5.TETROMINO_X, PlatformHTML5.TETROMINO_L_Y, this.m_game.stats().pieces[Game.TETROMINO_L], PlatformHTML5.TETROMINO_LENGTH, Game.COLOR_ORANGE);
                    this.drawNumber(PlatformHTML5.TETROMINO_X, PlatformHTML5.TETROMINO_I_Y, this.m_game.stats().pieces[Game.TETROMINO_I], PlatformHTML5.TETROMINO_LENGTH, Game.COLOR_CYAN);
                    this.drawNumber(PlatformHTML5.TETROMINO_X, PlatformHTML5.TETROMINO_T_Y, this.m_game.stats().pieces[Game.TETROMINO_T], PlatformHTML5.TETROMINO_LENGTH, Game.COLOR_PURPLE);
                    this.drawNumber(PlatformHTML5.TETROMINO_X, PlatformHTML5.TETROMINO_S_Y, this.m_game.stats().pieces[Game.TETROMINO_S], PlatformHTML5.TETROMINO_LENGTH, Game.COLOR_GREEN);
                    this.drawNumber(PlatformHTML5.TETROMINO_X, PlatformHTML5.TETROMINO_Z_Y, this.m_game.stats().pieces[Game.TETROMINO_Z], PlatformHTML5.TETROMINO_LENGTH, Game.COLOR_RED);
                    this.drawNumber(PlatformHTML5.TETROMINO_X, PlatformHTML5.TETROMINO_O_Y, this.m_game.stats().pieces[Game.TETROMINO_O], PlatformHTML5.TETROMINO_LENGTH, Game.COLOR_YELLOW);
                    this.drawNumber(PlatformHTML5.TETROMINO_X, PlatformHTML5.TETROMINO_J_Y, this.m_game.stats().pieces[Game.TETROMINO_J], PlatformHTML5.TETROMINO_LENGTH, Game.COLOR_BLUE);
                    this.drawNumber(PlatformHTML5.PIECES_X, PlatformHTML5.PIECES_Y, this.m_game.stats().totalPieces, PlatformHTML5.PIECES_LENGTH, Game.COLOR_WHITE);
                }
                if(this.m_game.isOver()) {
                    this.showOverlay("Game is over");
                }
                // Inform the game that we are done with the changed state
                this.m_game.onChangeProcessed();
            }
        }// Return the current system time in milliseconds
        ;
        PlatformHTML5.prototype.getSystemTime = function () {
            return Date.now();
        }// Return a random positive integer number
        ;
        PlatformHTML5.prototype.random = function () {
            // JavaScript maximum integer number is 2^53 = 9007199254740992.
            return Math.floor(9007199254740992 * Math.random());
        };
        PlatformHTML5.prototype.drawTile = function (x, y, tile, shadow) {
            this.m_canvas.drawImage(this.m_image, PlatformHTML5.TILE_SIZE * (shadow ? Game.TETROMINO_TYPES + tile + 1 : tile), 0, PlatformHTML5.TILE_SIZE, PlatformHTML5.TILE_SIZE, x, y, PlatformHTML5.TILE_SIZE, PlatformHTML5.TILE_SIZE);
        };
        PlatformHTML5.prototype.drawNumber = function (x, y, value, length, color) {
            var pos = 0;
            do {
                this.m_canvasStats.drawImage(this.m_image, PlatformHTML5.NUMBER_WIDTH * (value % 10), 1 + PlatformHTML5.TILE_SIZE + PlatformHTML5.NUMBER_HEIGHT * color, PlatformHTML5.NUMBER_WIDTH, PlatformHTML5.NUMBER_HEIGHT, x + PlatformHTML5.NUMBER_WIDTH * (length - pos), y, PlatformHTML5.NUMBER_WIDTH, PlatformHTML5.NUMBER_HEIGHT);
                value = Math.floor(value / 10);
            }while(++pos < length)
        };
        return PlatformHTML5;
    })();
    Stc.PlatformHTML5 = PlatformHTML5;    
})(Stc || (Stc = {}));
window.onload = function () {
    // [iPhone] hiding Safari user interface components
    // http://stackoverflow.com/questions/273671/hiding-safari-user-interface-components-on-iphone
    setTimeout(function () {
        window.scrollTo(0, 1);
    }, 0);
    // Start loading the image with all game elements.
    var image = new Image();
    image.src = "stc_sprites.png";
    // Start game after image has loaded.
    image.onload = function () {
        var platform = new Stc.PlatformHTML5(image);
        var game = new Stc.Game();
        game.init(platform);
        function update() {
            game.update();
        }
        setInterval(update, 1000 / Stc.PlatformHTML5.FPS);
    };
};
