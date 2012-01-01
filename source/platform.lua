-- ========================================================================== --
--   Platform implementation.                                                 --
--   Copyright (c) 2011 Laurens Rodriguez Oscanoa.                            --
-- -------------------------------------------------------------------------- --

-- Screen size 
local SCREEN_WIDTH  = 480;
local SCREEN_HEIGHT = 272;

-- Size of square tile 
local TILE_SIZE = 12;

-- Board up-left corner coordinates 
local BOARD_X = 180;
local BOARD_Y = 4;

-- Preview tetromino position 
local PREVIEW_X = 112;
local PREVIEW_Y = 210;

-- Score position and length on screen 
local SCORE_X      = 72;
local SCORE_Y      = 52;
local SCORE_LENGTH = 10;

-- Lines position and length on screen 
local LINES_X      = 108;
local LINES_Y      = 34;
local LINES_LENGTH = 5;

-- Level position and length on screen 
local LEVEL_X      = 108;
local LEVEL_Y      = 16;
local LEVEL_LENGTH = 5;

-- Tetromino subtotals position 
local TETROMINO_X   = 425;
local TETROMINO_L_Y = 53;
local TETROMINO_I_Y = 77;
local TETROMINO_T_Y = 101;
local TETROMINO_S_Y = 125;
local TETROMINO_Z_Y = 149;
local TETROMINO_O_Y = 173;
local TETROMINO_J_Y = 197;

-- Size of subtotals 
local TETROMINO_LENGTH = 5;

-- Tetromino total position 
local PIECES_X      = 418;
local PIECES_Y      = 221;
local PIECES_LENGTH = 6;

-- Size of number 
local NUMBER_WIDTH  = 7;
local NUMBER_HEIGHT = 9;
    
Platform = { 
    m_bmpBackground = nil;
    m_bmpBlocks = nil;
    m_bmpNumbers = nil;    
    
    m_blocks = nil;
    m_numbers = nil;

    m_musicLoop = nil;
    m_musicIntro = nil;
    m_musicMute = nil;
};
    
-- Initializes platform.
function Platform:init()
    -- Initialize random generator
    math.randomseed(os.time());

    -- Load images.
    self.m_bmpBackground = love.graphics.newImage("back.png");
    
    self.m_bmpBlocks = love.graphics.newImage("blocks.png");
    self.m_bmpBlocks:setFilter("nearest", "nearest");    
    local w = self.m_bmpBlocks:getWidth();
    local h = self.m_bmpBlocks:getHeight();

    -- Load music.
	self.m_musicIntro = love.audio.newSource("stc_theme_intro.ogg");
	self.m_musicIntro:setVolume(0.5);
	self.m_musicIntro:play();
	self.m_musicLoop = love.audio.newSource("stc_theme_loop.ogg", "stream");
	self.m_musicLoop:setLooping(true);
	self.m_musicLoop:setVolume(0.5);
	m_musicMute = false;

    -- Create quads for blocks
    self.m_blocks = {};
    for shadow = 0, 1 do
        self.m_blocks[shadow] = {};
        for color = 0, Game.COLORS - 1 do
            self.m_blocks[shadow][color] = love.graphics.newQuad(TILE_SIZE * color, (TILE_SIZE + 1) * shadow,
                                                                 TILE_SIZE + 1, TILE_SIZE + 1, w, h);
        end
    end
    
    self.m_bmpNumbers = love.graphics.newImage("numbers.png");
    self.m_bmpNumbers:setFilter("nearest", "nearest");
    w = self.m_bmpNumbers:getWidth();
    h = self.m_bmpNumbers:getHeight();
    
    -- Create quads for numbers
    self.m_numbers = {};
    for color = 0, Game.COLORS - 1 do
        self.m_numbers[color] = {};
        for digit = 0, 9 do
            self.m_numbers[color][digit] = love.graphics.newQuad(NUMBER_WIDTH * digit, NUMBER_HEIGHT * color,
                                                                 NUMBER_WIDTH, NUMBER_HEIGHT, w, h);
        end
    end
end

-- Process events and notify game.
function Platform:onKeyDown(key)
    if (key == "escape") then
        love.event.push("quit");
    end
    if ((key == "left") or (key == "a")) then
        Game:onEventStart(Game.Event.MOVE_LEFT);
    end
    if ((key == "right") or (key == "d")) then
        Game:onEventStart(Game.Event.MOVE_RIGHT);
    end
    if ((key == "down") or (key == "s")) then
        Game:onEventStart(Game.Event.MOVE_DOWN);
    end
    if ((key == "up") or (key == "w")) then
        Game:onEventStart(Game.Event.ROTATE_CW);
    end
    if (key == " ") then
        Game:onEventStart(Game.Event.DROP);
    end
    if (key == "f5") then
        Game:onEventStart(Game.Event.RESTART);
    end
    if (key == "f1") then
        Game:onEventStart(Game.Event.PAUSE);
    end
    if (key == "f2") then
        Game:onEventStart(Game.Event.SHOW_NEXT);
    end
    if (key == "f3") then
        Game:onEventStart(Game.Event.SHOW_SHADOW);
    end
end

function Platform:onKeyUp(key)
    if ((key == "left") or (key == "a")) then
        Game:onEventEnd(Game.Event.MOVE_LEFT);
    end
    if ((key == "right") or (key == "d")) then
        Game:onEventEnd(Game.Event.MOVE_RIGHT);
    end
    if ((key == "down") or (key == "s")) then
        Game:onEventEnd(Game.Event.MOVE_DOWN);
    end
    if ((key == "up") or (key == "w")) then
        Game:onEventEnd(Game.Event.ROTATE_CW);
    end
    if (key == "f4") then
		if (self.m_musicMute) then
			if (self.m_musicIntro) then
				self.m_musicIntro:resume();
			else
				self.m_musicLoop:resume();
			end
		else
			if (self.m_musicIntro) then
				self.m_musicIntro:pause();
			else
				self.m_musicLoop:pause();
			end
		end
		self.m_musicMute = not self.m_musicMute;
    end
end

-- Draw a tile from a tetromino
function Platform:drawTile(x, y, tile, shadow)
    love.graphics.drawq(self.m_bmpBlocks, self.m_blocks[shadow][tile], x, y);
end

-- Draw a number on the given position
function Platform:drawNumber(x, y, number, length, color)
    local pos = 0;
    repeat
        love.graphics.drawq(self.m_bmpNumbers, self.m_numbers[color][number % 10],
                            x + NUMBER_WIDTH * (length - pos), y);
        number = math.floor(number / 10);
        pos = pos + 1;
    until (pos >= length);
end

-- Render the state of the game using platform functions.
function Platform:renderGame()
    -- Draw background
    love.graphics.draw(self.m_bmpBackground, 0, 0);

    -- Draw preview block
    if Game:showPreview() then
        for i = 0, Game.TETROMINO_SIZE - 1 do
            for j = 0, Game.TETROMINO_SIZE - 1 do
                if (Game:nextBlock().cells[i][j] ~= Game.Cell.EMPTY) then
                    Platform:drawTile(PREVIEW_X + TILE_SIZE * i,
                                      PREVIEW_Y + TILE_SIZE * j,
                                      Game:nextBlock().cells[i][j], 0);
                end
            end
        end
    end

    -- Draw shadow tetromino
    if (Game:showShadow() and Game:shadowGap() > 0) then
        for i = 0, Game.TETROMINO_SIZE - 1 do
            for j = 0, Game.TETROMINO_SIZE - 1 do
                if (Game:fallingBlock().cells[i][j] ~= Game.Cell.EMPTY) then
                    Platform:drawTile(BOARD_X + (TILE_SIZE * (Game:fallingBlock().x + i)),
                                      BOARD_Y + (TILE_SIZE * (Game:fallingBlock().y + Game:shadowGap() + j)),
                                      Game:fallingBlock().cells[i][j], 1);
                end
            end
        end
    end

    -- Draw the cells in the board
    for i = 0, Game.BOARD_TILEMAP_WIDTH - 1 do
        for j = 0, Game.BOARD_TILEMAP_HEIGHT - 1 do
            if (Game:getCell(i, j) ~= Game.Cell.EMPTY) then
                Platform:drawTile(BOARD_X + (TILE_SIZE * i),
                                  BOARD_Y + (TILE_SIZE * j),
                                  Game:getCell(i, j), 0);
            end
        end
    end

    -- Draw falling tetromino
    for i = 0, Game.TETROMINO_SIZE - 1 do
        for j = 0, Game.TETROMINO_SIZE - 1 do
            if (Game:fallingBlock().cells[i][j] ~= Game.Cell.EMPTY) then
                Platform:drawTile(BOARD_X + TILE_SIZE * (Game:fallingBlock().x + i),
                                  BOARD_Y + TILE_SIZE * (Game:fallingBlock().y + j),
                                  Game:fallingBlock().cells[i][j], 0);
            end
        end
    end
    
    -- Draw game statistic data
    if (not Game:isPaused()) then
        Platform:drawNumber(LEVEL_X, LEVEL_Y, Game:stats().level, LEVEL_LENGTH, Game.Cell.WHITE);
        Platform:drawNumber(LINES_X, LINES_Y, Game:stats().lines, LINES_LENGTH, Game.Cell.WHITE);
        Platform:drawNumber(SCORE_X, SCORE_Y, Game:stats().score, SCORE_LENGTH, Game.Cell.WHITE);

        Platform:drawNumber(TETROMINO_X, TETROMINO_L_Y, Game:stats().pieces[Game.TetrominoType.L], TETROMINO_LENGTH, Game.Cell.ORANGE);
        Platform:drawNumber(TETROMINO_X, TETROMINO_I_Y, Game:stats().pieces[Game.TetrominoType.I], TETROMINO_LENGTH, Game.Cell.CYAN);
        Platform:drawNumber(TETROMINO_X, TETROMINO_T_Y, Game:stats().pieces[Game.TetrominoType.T], TETROMINO_LENGTH, Game.Cell.PURPLE);
        Platform:drawNumber(TETROMINO_X, TETROMINO_S_Y, Game:stats().pieces[Game.TetrominoType.S], TETROMINO_LENGTH, Game.Cell.GREEN);
        Platform:drawNumber(TETROMINO_X, TETROMINO_Z_Y, Game:stats().pieces[Game.TetrominoType.Z], TETROMINO_LENGTH, Game.Cell.RED);
        Platform:drawNumber(TETROMINO_X, TETROMINO_O_Y, Game:stats().pieces[Game.TetrominoType.O], TETROMINO_LENGTH, Game.Cell.YELLOW);
        Platform:drawNumber(TETROMINO_X, TETROMINO_J_Y, Game:stats().pieces[Game.TetrominoType.J], TETROMINO_LENGTH, Game.Cell.BLUE);

        Platform:drawNumber(PIECES_X, PIECES_Y, Game:stats().totalPieces, PIECES_LENGTH, Game.Cell.WHITE);
    end

	-- Adding music loop check here for convenience.
	if (self.m_musicIntro) then
		if (self.m_musicIntro:isStopped()) then
			self.m_musicIntro = nil;
			self.m_musicLoop:play();
		end
	end
end

function Platform:getSystemTime()
    return math.floor(1000 * love.timer.getTime());
end

function Platform:random()
    return math.random(1000000000);
end
