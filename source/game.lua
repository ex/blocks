-- ========================================================================== --
--   Game logic implementation.                                               --
--   Copyright (c) 2011 Laurens Rodriguez Oscanoa.                            --
-- -------------------------------------------------------------------------- --

-- Initial time delay (in milliseconds) between falling moves.
local INIT_DELAY_FALL = 1000;

-- Score points given by filled rows (we use the original NES * 10)
-- http://tetris.wikia.com/wiki/Scoring
local SCORE_1_FILLED_ROW = 400;
local SCORE_2_FILLED_ROW = 1000;
local SCORE_3_FILLED_ROW = 3000;
local SCORE_4_FILLED_ROW = 12000;

-- The player gets points every time he accelerates downfall.
-- The added points are equal to SCORE_2_FILLED_ROW divided by this value.
local SCORE_MOVE_DOWN_DIVISOR = 1000;

-- The player gets points every time he does a hard drop.
-- The added points are equal to SCORE_2_FILLED_ROW divided by these
-- values. If the player is not using the shadow he gets more points.
local SCORE_DROP_DIVISOR             = 20;
local SCORE_DROP_WITH_SHADOW_DIVISOR = 100;

-- Number of filled rows required to increase the game level.
local FILLED_ROWS_FOR_LEVEL_UP = 10;

-- The falling delay is multiplied and divided by
-- these factors with every level up.
local DELAY_FACTOR_FOR_LEVEL_UP = 9;
local DELAY_DIVISOR_FOR_LEVEL_UP = 10;

-- Delayed autoshift initial delay.
local DAS_DELAY_TIMER = 200;

-- Delayed autoshift timer for left and right moves.
local DAS_MOVE_TIMER = 40;

-- Rotation auto-repeat delay.
local ROTATION_AUTOREPEAT_DELAY = 375;

-- Rotation autorepeat timer.
local ROTATION_AUTOREPEAT_TIMER = 200;

-- Number of tetromino types.
local TETROMINO_TYPES = 7;

Game = {
    -- Playfield size (in tiles).
    BOARD_TILEMAP_WIDTH  = 10;
    BOARD_TILEMAP_HEIGHT = 22;

    -- Error codes.
    Error = { 
        NONE         =  0,   -- Everything is OK, oh wonders!
        PLAYER_QUITS =  1,   -- The user quits, our fail
        NO_MEMORY    = -1,   -- Not enough memory
        NO_VIDEO     = -2,   -- Video system was not initialized
        NO_IMAGES    = -3,   -- Problem loading the image files
        PLATFORM     = -4,   -- Problem creating platform
        ASSERT       = -100  -- Something went very very wrong...
    }; 

    -- Game events.
    Event = {
        NONE        = 0,
        MOVE_DOWN   = 1,
        MOVE_LEFT   = 2,
        MOVE_RIGHT  = 4,
        ROTATE_CW   = 8,    -- rotate clockwise
        ROTATE_CCW  = 16,   -- rotate counter-clockwise
        DROP        = 32,
        PAUSE       = 64,
        RESTART     = 128,
        SHOW_NEXT   = 256,  -- toggle show next tetromino
        SHOW_SHADOW = 512,  -- toggle show shadow
        QUIT        = 1024  -- finish the game
    };

    -- We are going to store the tetromino cells in a square matrix
    -- of this size (this is the size of the biggest tetromino).
    TETROMINO_SIZE  = 4;

    -- Tetromino definitions.
    -- They are indexes and must be between: 0 - [TETROMINO_TYPES - 1]
    -- http://tetris.wikia.com/wiki/Tetromino
    -- Initial cell disposition is commented below.
    TetrominoType = { 
        --      ....
        --      ####
        --      ....
        --      ....
        I = 0,
        --      ##..
        --      ##..
        --      ....
        --      ....
        O = 1,
        --      .#..
        --      ###.
        --      ....
        --      ....
        T = 2,
        --      .##.
        --      ##..
        --      ....
        --      ....
        S = 3,
        --      ##..
        --      .##.
        --      ....
        --      ....
        Z = 4,
        --      #...
        --      ###.
        --      ....
        --      ....
        J = 5,
        --      ..#.
        --      ###.
        --      ....
        --      ....
        L = 6
    };

    -- Color indexes.
    Cell = {
        EMPTY  = -1, -- This value used for empty tiles.
        CYAN   = 1,
        RED    = 2,
        BLUE   = 3,
        ORANGE = 4,
        GREEN  = 5,
        YELLOW = 6,
        PURPLE = 7,
        WHITE  = 0  -- Used for effects (if any)
    };
    
    COLORS = 8;

    -- Create data structure that holds information about our tetromino blocks.
    createTetromino = function()
        local tetromino = {
            cells = {}; -- Tetromino buffer
            x = 0;
            y = 0;
            size = 0;
            type = nil;
        };
        return tetromino;
    end;

    -- Create data structure for statistical data.
    createStatics = function()
        local stats = {
            score = 0;       -- user score for current game
            lines = 0;       -- total number of lines cleared
            totalPieces = 0; -- total number of tetrominoes used
            level = 0;       -- current game level
            pieces = {};     -- number of tetrominoes per type
        };
        return stats;
    end;
    
    -- Game events are stored in bits in this variable.
    -- It must be cleared to Game.Event.NONE after being used.
    m_events = nil;

    -- Matrix that holds the cells (tilemap)
    m_map = nil;

    m_stats = nil;        -- statistic data
    m_fallingBlock = nil; -- current falling tetromino
    m_nextBlock = nil;    -- next tetromino

    m_stateChanged = nil;   -- true if game state has changed
    m_errorCode = nil;      -- stores current error code
    m_isPaused = nil;       -- true if the game is over
    m_isOver = nil;         -- true if the game is over
    m_showPreview = nil;    -- true if we must show the preview block

    m_showShadow = nil; -- true if we must show the shadow block
    m_shadowGap = nil;  -- distance between falling block and shadow

    m_systemTime = nil;   -- system time in milliseconds
    m_fallingDelay = nil; -- delay time for falling tetrominoes
    m_lastFallTime = nil; -- last time the falling tetromino dropped

    -- For delayed autoshift: http://tetris.wikia.com/wiki/DAS
    m_delayLeft = nil;
    m_delayRight = nil;
    m_delayDown = nil;

    m_delayRotation = nil;
};

-- The platform must call this method after processing a changed state.
function Game:onChangeProcessed()   self.m_stateChanged = false; end

-- Return the cell at the specified position.
function Game:getCell(column, row)  return self.m_map[column][row]; end

-- Return true if the game state has changed, false otherwise.
function Game:hasChanged()          return self.m_stateChanged; end

-- Return a reference to the game statistic data.
function Game:stats()               return self.m_stats; end

-- Return current falling tetromino.
function Game:fallingBlock()        return self.m_fallingBlock; end

-- Return next tetromino.
function Game:nextBlock()           return self.m_nextBlock; end

-- Return current error code.
function Game:errorCode()           return self.m_errorCode; end

-- Return true if the game is paused, false otherwise.
function Game:isPaused()            return self.m_isPaused; end

-- Return true if we must show preview tetromino.
function Game:showPreview()         return self.m_showPreview; end

-- Return true if we must show ghost shadow.
function Game:showShadow()          return self.m_showShadow; end

-- Return height gap between shadow and falling tetromino.
function Game:shadowGap()           return self.m_shadowGap; end

-- Set matrix elements to indicated value.
function Game:setMatrixCells(matrix, width, height, value)
	for i = 0, width - 1 do
        matrix[i] = {};
		for j = 0, height - 1 do
			matrix[i][j] = value;
		end
	end
end

-- Initialize tetromino cells for every type of tetromino.
function Game:setTetromino(indexTetromino, tetromino)
	-- Initialize tetromino cells to empty cells.
	Game:setMatrixCells(tetromino.cells, Game.TETROMINO_SIZE, Game.TETROMINO_SIZE, Game.Cell.EMPTY);

	-- Almost all the blocks have size 3.
	tetromino.size = Game.TETROMINO_SIZE - 1;

	-- Initial configuration from: http://tetris.wikia.com/wiki/SRS
	if indexTetromino == Game.TetrominoType.I then
        tetromino.cells[0][1] = Game.Cell.CYAN;
        tetromino.cells[1][1] = Game.Cell.CYAN;
        tetromino.cells[2][1] = Game.Cell.CYAN;
        tetromino.cells[3][1] = Game.Cell.CYAN;
        tetromino.size = Game.TETROMINO_SIZE;
    elseif indexTetromino == Game.TetrominoType.O then
        tetromino.cells[0][0] = Game.Cell.YELLOW;
        tetromino.cells[0][1] = Game.Cell.YELLOW;
        tetromino.cells[1][0] = Game.Cell.YELLOW;
        tetromino.cells[1][1] = Game.Cell.YELLOW;
        tetromino.size = Game.TETROMINO_SIZE - 2;
    elseif indexTetromino == Game.TetrominoType.T then
        tetromino.cells[0][1] = Game.Cell.PURPLE;
        tetromino.cells[1][0] = Game.Cell.PURPLE;
        tetromino.cells[1][1] = Game.Cell.PURPLE;
        tetromino.cells[2][1] = Game.Cell.PURPLE;
    elseif indexTetromino == Game.TetrominoType.S then
        tetromino.cells[0][1] = Game.Cell.GREEN;
        tetromino.cells[1][0] = Game.Cell.GREEN;
        tetromino.cells[1][1] = Game.Cell.GREEN;
        tetromino.cells[2][0] = Game.Cell.GREEN;
    elseif indexTetromino == Game.TetrominoType.Z then
        tetromino.cells[0][0] = Game.Cell.RED;
        tetromino.cells[1][0] = Game.Cell.RED;
        tetromino.cells[1][1] = Game.Cell.RED;
        tetromino.cells[2][1] = Game.Cell.RED;
    elseif indexTetromino == Game.TetrominoType.J then
        tetromino.cells[0][0] = Game.Cell.BLUE;
        tetromino.cells[0][1] = Game.Cell.BLUE;
        tetromino.cells[1][1] = Game.Cell.BLUE;
        tetromino.cells[2][1] = Game.Cell.BLUE;
    elseif indexTetromino == Game.TetrominoType.L then
        tetromino.cells[0][1] = Game.Cell.ORANGE;
        tetromino.cells[1][1] = Game.Cell.ORANGE;
        tetromino.cells[2][0] = Game.Cell.ORANGE;
        tetromino.cells[2][1] = Game.Cell.ORANGE;
	end
	tetromino.type = indexTetromino;
end

-- Start a new game.
function Game:start()
	-- Initialize game data.
    self.m_map = {};
	self.m_stats = Game:createStatics();
	self.m_fallingBlock = Game:createTetromino();
	self.m_nextBlock = Game:createTetromino();

	self.m_errorCode = Game.Error.NONE;
	self.m_systemTime = Platform:getSystemTime();
	self.m_lastFallTime = self.m_systemTime;
	self.m_isOver = false;
	self.m_isPaused = false;
	self.m_showPreview = true;
	self.m_events = Game.Event.NONE;
	self.m_fallingDelay = INIT_DELAY_FALL;
	self.m_showShadow = true;

	-- Initialize game statistics.
	for i = 0, TETROMINO_TYPES - 1 do
		self.m_stats.pieces[i] = 0;
	end

	-- Initialize game tile map.
	Game:setMatrixCells(self.m_map, Game.BOARD_TILEMAP_WIDTH, Game.BOARD_TILEMAP_HEIGHT, Game.Cell.EMPTY);

	-- Initialize falling tetromino.
	Game:setTetromino(Platform:random() % TETROMINO_TYPES, self.m_fallingBlock);
	self.m_fallingBlock.x = math.floor((Game.BOARD_TILEMAP_WIDTH - self.m_fallingBlock.size) / 2);
	self.m_fallingBlock.y = 0;

	-- Initialize preview tetromino.
	Game:setTetromino(Platform.random() % TETROMINO_TYPES, self.m_nextBlock);

	-- Initialize events.
	Game:onTetrominoMoved();

	-- Initialize delayed autoshift.
	self.m_delayLeft = -1;
	self.m_delayRight = -1;
	self.m_delayDown = -1;
	self.m_delayRotation = -1;
end

-- Initialize the game. The error code (if any) is saved in [mErrorcode].
function Game:init()
	-- Initialize platform.
	Platform:init();

    -- If everything is OK start the game.
    Game:start();
end

-- Rotate falling tetromino. If there are no collisions when the
-- tetromino is rotated this modifies the tetromino's cell buffer.
function Game:rotateTetromino(clockwise)
    local i; local j;

	-- Temporary array to hold rotated cells.
	local rotated = {};  

	-- If TETROMINO_O is falling return immediately.
	if (self.m_fallingBlock.type == Game.TetrominoType.O) then
		-- Rotation doesn't require any changes.
		return; 
	end

	-- Initialize rotated cells to blank.
	Game:setMatrixCells(rotated, Game.TETROMINO_SIZE, Game.TETROMINO_SIZE, Game.Cell.EMPTY);

	-- Copy rotated cells to the temporary array.
	for i = 0, self.m_fallingBlock.size - 1 do
		for j = 0, self.m_fallingBlock.size - 1 do
			if (clockwise) then
				rotated[self.m_fallingBlock.size - j - 1][i] = self.m_fallingBlock.cells[i][j];
			else
				rotated[j][self.m_fallingBlock.size - i - 1] = self.m_fallingBlock.cells[i][j];
			end
		end
	end

	local wallDisplace = 0;

	-- Check collision with left wall.
	if (self.m_fallingBlock.x < 0) then
        i = 0;
		while ((wallDisplace == 0) and (i < -self.m_fallingBlock.x)) do
			for j = 0, self.m_fallingBlock.size - 1 do
				if (rotated[i][j] ~= Game.Cell.EMPTY) then
					wallDisplace = i - self.m_fallingBlock.x;
					break;
				end
			end
            i = i + 1;
		end
	-- Or check collision with right wall.
	elseif (self.m_fallingBlock.x > Game.BOARD_TILEMAP_WIDTH - self.m_fallingBlock.size) then
		i = self.m_fallingBlock.size - 1;
		while ((wallDisplace == 0) and (i >= Game.BOARD_TILEMAP_WIDTH - self.m_fallingBlock.x)) do
			for j = 0, self.m_fallingBlock.size - 1 do
				if (rotated[i][j] ~= Game.Cell.EMPTY) then
					wallDisplace = -self.m_fallingBlock.x - i + Game.BOARD_TILEMAP_WIDTH - 1;
					break;
				end
			end
            i = i - 1;
        end
	end

	-- Check collision with board floor and other cells on board.
	for i = 0, self.m_fallingBlock.size - 1 do
		for j = 0, self.m_fallingBlock.size - 1 do
			if (rotated[i][j] ~= Game.Cell.EMPTY) then
				-- Check collision with bottom border of the map.
				if (self.m_fallingBlock.y + j >= Game.BOARD_TILEMAP_HEIGHT) then
					-- There is a collision therefore return.
					return; 
                end
				-- Check collision with existing cells in the map.
				if (self.m_map[i + self.m_fallingBlock.x + wallDisplace][j + self.m_fallingBlock.y] ~= Game.Cell.EMPTY) then
					-- There is a collision therefore return.
					return; 
				end
			end
		end
	end
	-- Move the falling piece if there was wall collision and it's a legal move.
	if (wallDisplace ~= 0) then
		self.m_fallingBlock.x = self.m_fallingBlock.x + wallDisplace;
	end

	-- There are no collisions, replace tetromino cells with rotated cells.
	for i = 0, Game.TETROMINO_SIZE - 1 do
		for j = 0, Game.TETROMINO_SIZE - 1 do
			self.m_fallingBlock.cells[i][j] = rotated[i][j];
		end
	end
	Game:onTetrominoMoved();
end

-- Check if tetromino will collide with something if it is moved in the requested direction.
-- If there are collisions returns 1 else returns 0.
function Game:checkCollision(dx, dy)

	local newx = self.m_fallingBlock.x + dx;
	local newy = self.m_fallingBlock.y + dy;

	for i = 0, self.m_fallingBlock.size - 1 do
		for j = 0, self.m_fallingBlock.size - 1 do
			if (self.m_fallingBlock.cells[i][j] ~= Game.Cell.EMPTY) then
				-- Check that tetromino would be inside the left, right and bottom borders.
				if ((newx + i < 0) or (newx + i >= Game.BOARD_TILEMAP_WIDTH)
								   or (newy + j >= Game.BOARD_TILEMAP_HEIGHT)) then
					return true;
				end
				-- Check that tetromino won't collide with existing cells in the map.
				if (self.m_map[newx + i][newy + j] ~= Game.Cell.EMPTY) then
					return true;
				end
			end
		end
	end
	return false;
end

-- Game scoring: http://tetris.wikia.com/wiki/Scoring
function Game:onFilledRows(filledRows)
    -- Update total number of filled rows.
	self.m_stats.lines = self.m_stats.lines + filledRows;

	-- Increase score accordingly to the number of filled rows.
	if (filledRows == 1) then
        self.m_stats.score = self.m_stats.score + (SCORE_1_FILLED_ROW * (self.m_stats.level + 1));
    elseif (filledRows == 2) then
        self.m_stats.score = self.m_stats.score + (SCORE_2_FILLED_ROW * (self.m_stats.level + 1));
    elseif (filledRows == 3) then
        self.m_stats.score = self.m_stats.score + (SCORE_3_FILLED_ROW * (self.m_stats.level + 1));
    elseif (filledRows == 4) then
        self.m_stats.score = self.m_stats.score + (SCORE_4_FILLED_ROW * (self.m_stats.level + 1));
    else
        -- This shouldn't happen, but if happens kill the game.
        self.m_errorCode = Game.Error.ASSERT;
	end

	-- Check if we need to update the level.
	if (self.m_stats.lines >= FILLED_ROWS_FOR_LEVEL_UP * (self.m_stats.level + 1)) then
		self.m_stats.level = self.m_stats.level + 1;

		-- Increase speed for falling tetrominoes.
		self.m_fallingDelay = math.floor(DELAY_FACTOR_FOR_LEVEL_UP * self.m_fallingDelay 
                                         / DELAY_DIVISOR_FOR_LEVEL_UP);
	end
end

-- Move tetromino in the direction specified by (x, y) (in tile units)
-- This function detects if there are filled rows or if the move
-- lands a falling tetromino, also checks for game over condition.
function Game:moveTetromino(x, y)
	local i; local j;

	-- Check if the move would create a collision.
	if (Game:checkCollision(x, y)) then
		-- In case of collision check if move was downwards (y == 1)
		if (y == 1) then
			-- Check if collision occurs when the falling
			-- tetromino is on the 1st or 2nd row.
			if (self.m_fallingBlock.y <= 1) then
				-- If this happens the game is over.
				self.m_isOver = true;   
			else
				-- The falling tetromino has reached the bottom,
				-- so we copy their cells to the board map.
				for i = 0, self.m_fallingBlock.size - 1 do
					for j = 0, self.m_fallingBlock.size - 1 do
						if (self.m_fallingBlock.cells[i][j] ~= Game.Cell.EMPTY) then
							self.m_map[self.m_fallingBlock.x + i][self.m_fallingBlock.y + j]
										= self.m_fallingBlock.cells[i][j];
						end
					end
				end

				-- Check if the landing tetromino has created full rows.
				local numFilledRows = 0;
				for j = 1, Game.BOARD_TILEMAP_HEIGHT - 1 do
					local hasFullRow = true;
					for i = 0, Game.BOARD_TILEMAP_WIDTH - 1 do
						if (self.m_map[i][j] == Game.Cell.EMPTY) then
							hasFullRow = false;
							break;
						end
					end
					-- If we found a full row we need to remove that row from the map
					-- we do that by just moving all the above rows one row below.
					if (hasFullRow) then
						for x = 0, Game.BOARD_TILEMAP_WIDTH - 1 do
							for y = j, 1, -1 do
								self.m_map[x][y] = self.m_map[x][y - 1];
							end
						end
						-- Increase filled row counter.
						numFilledRows = numFilledRows + 1;
					end
				end

				-- Update game statistics.
				if (numFilledRows > 0) then
					Game:onFilledRows(numFilledRows);
				end
				self.m_stats.totalPieces = self.m_stats.totalPieces + 1;
				self.m_stats.pieces[self.m_fallingBlock.type] 
                        = self.m_stats.pieces[self.m_fallingBlock.type] + 1;

				-- Use preview tetromino as falling tetromino.
				-- Copy preview tetromino for falling tetromino.
				for i = 0, Game.TETROMINO_SIZE - 1 do
					for j = 0, Game.TETROMINO_SIZE - 1 do
						self.m_fallingBlock.cells[i][j] = self.m_nextBlock.cells[i][j];
					end
				end
				self.m_fallingBlock.size = self.m_nextBlock.size;
				self.m_fallingBlock.type = self.m_nextBlock.type;

				-- Reset position.
				self.m_fallingBlock.y = 0;
				self.m_fallingBlock.x = math.floor((Game.BOARD_TILEMAP_WIDTH - self.m_fallingBlock.size) / 2);
				Game:onTetrominoMoved();

				-- Create next preview tetromino.
				Game:setTetromino(Platform:random() % TETROMINO_TYPES, self.m_nextBlock);
			end
		end
	else
		-- There are no collisions, just move the tetromino.
		self.m_fallingBlock.x = self.m_fallingBlock.x + x;
		self.m_fallingBlock.y = self.m_fallingBlock.y + y;
	end
	Game:onTetrominoMoved();
end

-- Hard drop.
function Game:dropTetromino()
    -- Shadow has already calculated the landing position.
    self.m_fallingBlock.y = self.m_fallingBlock.y + self.m_shadowGap;

    -- Force lock.
    Game:moveTetromino(0, 1); 

	-- Update score.
	if (self.m_showShadow) then
		self.m_stats.score = self.m_stats.score + (SCORE_2_FILLED_ROW * (self.m_stats.level + 1) 
                                                  / SCORE_DROP_WITH_SHADOW_DIVISOR);
	else 
		self.m_stats.score = self.m_stats.score + (SCORE_2_FILLED_ROW * (self.m_stats.level + 1)
                                                  / SCORE_DROP_DIVISOR);
	end
end

-- Main game function called every frame.
function Game:update()
	-- Update game state.
	if self.m_isOver then
		if isFlagSet(self.m_events, Game.Event.RESTART) then
			self.m_isOver = false;
			Game:start();
		end
	else
		local currentTime = Platform:getSystemTime();

		-- Process delayed autoshift.
		local timeDelta = currentTime - self.m_systemTime;
        
		if (self.m_delayDown > 0) then
			self.m_delayDown = self.m_delayDown - timeDelta;
			if (self.m_delayDown <= 0) then
				self.m_delayDown = DAS_MOVE_TIMER;
				self.m_events = setFlag(self.m_events, Game.Event.MOVE_DOWN);
			end
		end
		if (self.m_delayLeft > 0) then
			self.m_delayLeft = self.m_delayLeft - timeDelta;
			if (self.m_delayLeft <= 0) then
				self.m_delayLeft = DAS_MOVE_TIMER;
				self.m_events = setFlag(self.m_events, Game.Event.MOVE_LEFT);
			end
		elseif (self.m_delayRight > 0) then
			self.m_delayRight = self.m_delayRight - timeDelta;
			if (self.m_delayRight <= 0) then
				self.m_delayRight = DAS_MOVE_TIMER;
				self.m_events = setFlag(self.m_events, Game.Event.MOVE_RIGHT);
			end
		end
		if (self.m_delayRotation > 0) then
			self.m_delayRotation = self.m_delayRotation - timeDelta;
			if (self.m_delayRotation <= 0) then
				self.m_delayRotation = ROTATION_AUTOREPEAT_TIMER;
				self.m_events = setFlag(self.m_events, Game.Event.ROTATE_CW);
			end
		end

		-- Always handle pause event.
		if isFlagSet(self.m_events, Game.Event.PAUSE) then
			self.m_isPaused = not self.m_isPaused;
			self.m_events = Game.Event.NONE;
		end

		-- Check if the game is paused.
		if (self.m_isPaused) then
			-- We achieve the effect of pausing the game
			-- adding the last frame duration to lastFallTime.
			self.m_lastFallTime = self.m_lastFallTime + (currentTime - self.m_systemTime);
		else 
			if (self.m_events ~= Game.Event.NONE) then
				if isFlagSet(self.m_events, Game.Event.SHOW_NEXT) then
					self.m_showPreview = not self.m_showPreview;
					self.m_stateChanged = true;
				end
				if isFlagSet(self.m_events, Game.Event.SHOW_SHADOW) then
					self.m_showShadow = not self.m_showShadow;
					self.m_stateChanged = true;
				end
				if isFlagSet(self.m_events, Game.Event.DROP) then
					Game:dropTetromino();
				end
				if isFlagSet(self.m_events, Game.Event.ROTATE_CW) then
					Game:rotateTetromino(true);
				end

				if isFlagSet(self.m_events, Game.Event.MOVE_RIGHT) then
					Game:moveTetromino(1, 0);
				elseif isFlagSet(self.m_events, Game.Event.MOVE_LEFT) then
					Game:moveTetromino(-1, 0);
				end

				if isFlagSet(self.m_events, Game.Event.MOVE_DOWN) then
					-- Update score if the player accelerates downfall.
					self.m_stats.score = self.m_stats.score + (SCORE_2_FILLED_ROW * (self.m_stats.level + 1) 
                                                               / SCORE_MOVE_DOWN_DIVISOR);

					Game:moveTetromino(0, 1);
				end
				self.m_events = Game.Event.NONE;
			end
			-- Check if it's time to move downwards the falling tetromino.
			if (currentTime - self.m_lastFallTime >= self.m_fallingDelay) then
				Game:moveTetromino(0, 1);
				self.m_lastFallTime = currentTime;
			end
		end
		-- Save current time for next game update.
		self.m_systemTime = currentTime;
	end
end

-- This event is called when the falling tetromino is moved.
function Game:onTetrominoMoved()
	local y = 1;
	-- Calculate number of cells where shadow tetromino would be.
	while (not Game:checkCollision(0, y)) do
        y = y + 1;
    end
	self.m_shadowGap = y - 1;
	self.m_stateChanged = true;
end

-- Process a key down event.
function Game:onEventStart(command)
	if (command == Game.Event.QUIT) then
        self.m_errorCode = Error.PLAYER_QUITS;
    elseif (command == Game.Event.MOVE_DOWN) then
        self.m_events = setFlag(self.m_events, Game.Event.MOVE_DOWN);
        self.m_delayDown = DAS_DELAY_TIMER;
    elseif (command == Game.Event.ROTATE_CW) then
        self.m_events = setFlag(self.m_events, Game.Event.ROTATE_CW);
        self.m_delayRotation = ROTATION_AUTOREPEAT_DELAY;
    elseif (command == Game.Event.MOVE_LEFT) then
        self.m_events = setFlag(self.m_events, Game.Event.MOVE_LEFT);
        self.m_delayLeft = DAS_DELAY_TIMER;
    elseif (command == Game.Event.MOVE_RIGHT) then
        self.m_events = setFlag(self.m_events, Game.Event.MOVE_RIGHT);
        self.m_delayRight = DAS_DELAY_TIMER;
    elseif (command == Game.Event.DROP) 
           or (command == Game.Event.RESTART)
           or (command == Game.Event.PAUSE)
           or (command == Game.Event.SHOW_NEXT)
           or (command == Game.Event.SHOW_SHADOW) then
        self.m_events = setFlag(self.m_events, command);
	end
end

-- Process a key up event.
function Game:onEventEnd(command)
	if (command == Game.Event.MOVE_DOWN) then
        self.m_delayDown = -1;
    elseif (command == Game.Event.MOVE_LEFT) then
        self.m_delayLeft = -1;
    elseif (command == Game.Event.MOVE_RIGHT) then
        self.m_delayRight = -1;
    elseif (command == Game.Event.ROTATE_CW) then
        self.m_delayRotation = -1;
	end
end

-- Bit flags utility helpers.
function isFlagSet(set, flag)
    return (set % (2*flag) >= flag);
end

function setFlag(set, flag)
    if (set % (2*flag) >= flag) then
        return set;
    end
    return (set + flag);
end

function clearFlag(set, flag)
    if (set % (2*flag) >= flag) then
        return (set - flag);
    end
    return set;
end

