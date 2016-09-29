/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Game logic implementation.                                               */
/*                                                                            */
/*   Copyright (c) 2011 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */
#if DEBUG || STC_USE_DLL
#define STC_AUTO_ROTATION
#define STC_WALL_KICK_ENABLED

namespace Stc
{
public class Game
{
    //--------------------------------------------------------------------------
    // Game configuration.
    // Edit this section to change the sizes, scores or pace of the game.
    //--------------------------------------------------------------------------

    // Playfield size (in tiles).
    public const int BOARD_TILEMAP_WIDTH  = 10;
    public const int BOARD_TILEMAP_HEIGHT = 22;

    // Initial time delay (in milliseconds) between falling moves.
    private const int INIT_DELAY_FALL = 1000;

    // Score points given by filled rows (we use the original NES * 10)
    // http://tetris.wikia.com/wiki/Scoring
    private const int SCORE_1_FILLED_ROW = 400;
    private const int SCORE_2_FILLED_ROW = 1000;
    private const int SCORE_3_FILLED_ROW = 3000;
    private const int SCORE_4_FILLED_ROW = 12000;

    // The player gets points every time he accelerates downfall.
    // The added points are equal to SCORE_2_FILLED_ROW divided by this value.
    private const int SCORE_MOVE_DOWN_DIVISOR = 1000;

    // The player gets points every time he does a hard drop.
    // The added points are equal to SCORE_2_FILLED_ROW divided by these
    // values. If the player is not using the shadow he gets more points.
    private const int SCORE_DROP_DIVISOR             = 20;
    private const int SCORE_DROP_WITH_SHADOW_DIVISOR = 100;

    // Number of filled rows required to increase the game level.
    private const int FILLED_ROWS_FOR_LEVEL_UP = 10;

    // The falling delay is multiplied and divided by
    // these factors with every level up.
    private const int DELAY_FACTOR_FOR_LEVEL_UP = 9;
    private const int DELAY_DIVISOR_FOR_LEVEL_UP = 10;

    // Delayed autoshift initial delay.
    private const int DAS_DELAY_TIMER = 200;

    // Delayed autoshift timer for left and right moves.
    private const int DAS_MOVE_TIMER = 40;

#if STC_AUTO_ROTATION
    // Rotation auto-repeat delay.
    private const int ROTATION_AUTOREPEAT_DELAY = 375;

    // Rotation autorepeat timer.
    private const int ROTATION_AUTOREPEAT_TIMER = 200;
#endif

    //--------------------------------------------------------------------------
    // Game constants.
    // Don't need to change this section unless you're changing the gameplay.
    //--------------------------------------------------------------------------

    // Number of tetromino types.
    private const int TETROMINO_TYPES = 7;

    // Error codes.
    public enum Error
    { 
        NONE         =  0,   // Everything is OK, oh wonders!
        PLAYER_QUITS =  1,   // The user quits, our fail
        NO_MEMORY    = -1,   // Not enough memory
        NO_VIDEO     = -2,   // Video system was not initialized
        NO_IMAGES    = -3,   // Problem loading the image files
        PLATFORM     = -4,   // Problem creating platform
        ASSERT       = -100  // Something went very very wrong...
    }; 

    // Game events.
    public enum Event
    {
        NONE        = 0,
        MOVE_DOWN   = 1,
        MOVE_LEFT   = 1 << 1,
        MOVE_RIGHT  = 1 << 2,
        ROTATE_CW   = 1 << 3,  // rotate clockwise
        ROTATE_CCW  = 1 << 4,  // rotate counter-clockwise
        DROP        = 1 << 5,
        PAUSE       = 1 << 6,
        RESTART     = 1 << 7,
        SHOW_NEXT   = 1 << 8,  // toggle show next tetromino
        SHOW_SHADOW = 1 << 9,  // toggle show shadow
        QUIT        = 1 << 10  // finish the game
    };

    // We are going to store the tetromino cells in a square matrix
    // of this size (this is the size of the biggest tetromino).
    public const int TETROMINO_SIZE  = 4;

    // Tetromino definitions.
    // They are indexes and must be between: 0 - [TETROMINO_TYPES - 1]
    // http://tetris.wikia.com/wiki/Tetromino
    // Initial cell disposition is commented below.
    public enum TetrominoType
    { 
        //      ....
        //      ####
        //      ....
        //      ....
        I = 0,
        //
        //      ##..
        //      ##..
        //      ....
        //      ....
        O = 1,
        //      .#..
        //      ###.
        //      ....
        //      ....
        T = 2,
        //      .##.
        //      ##..
        //      ....
        //      ....
        S = 3,
        //      ##..
        //      .##.
        //      ....
        //      ....
        Z = 4,
        //      #...
        //      ###.
        //      ....
        //      ....
        J = 5,
        //      ..#.
        //      ###.
        //      ....
        //      ....
        L = 6
    };

    // Color indexes.
    public enum Cell
    {
        EMPTY  = -1, // This value used for empty tiles.
        CYAN   = 1,
        RED    = 2,
        BLUE   = 3,
        ORANGE = 4,
        GREEN  = 5,
        YELLOW = 6,
        PURPLE = 7,
        WHITE  = 0  // Used for effects (if any)
    };

    // Data structure that holds information about our tetromino blocks.
    public class StcTetromino
    {
        // Tetromino buffer: [x][y]
        //  +---- x
        //  |
        //  |
        //  y
        public Cell[,] cells;
        public int x;
        public int y;
        public int size;
        public TetrominoType type;

        public StcTetromino()
        {
            cells = new Cell[TETROMINO_SIZE, TETROMINO_SIZE];
        }
    };

    // Data structure for statistical data.
    public class StcStatics
    {
        public long score;      // user score for current game
        public int lines;       // total number of lines cleared
        public int totalPieces; // total number of tetrominoes used
        public int level;       // current game level
        public int[] pieces;    // number of tetrominoes per type

        public StcStatics()
        {
            pieces = new int[TETROMINO_TYPES]; // number of tetrominoes per type
        }
    };

    // The platform must call this method after processing a changed state.
    public void OnChangeProcessed()          { m_stateChanged = false; }

    // The platform must call this method after processing a tetramino move.
    public void OnMoveProcessed()            { m_stateMoved = false; }

    // Return the cell at the specified position.
    public Cell GetCell(int column, int row) { return m_map[column, row]; }

    // Return true if the game state has changed, false otherwise.
    public bool hasChanged                   { get { return m_stateChanged; } }

    // Return true if the falling tetromino has moved, false otherwise.
    public bool hasMove                      { get { return m_stateMoved; } }

    // Return a reference to the game statistic data.
    public StcStatics stats                  { get { return m_stats; } }

    // Return current falling tetromino.
    public StcTetromino fallingBlock         { get { return m_fallingBlock; } }

    // Return next tetromino.
    public StcTetromino nextBlock            { get { return m_nextBlock; } }

    // Return current error code.
    public Error errorCode                   { get { return m_errorCode; } }

    // Return true if the game is paused, false otherwise.
    public bool isPaused                     { get { return m_isPaused; } }

    // Return true if we must show preview tetromino.
    public bool showPreview                  { get { return m_showPreview; } }

    // Return true if we must show ghost shadow.
    public bool showShadow                   { get { return m_showShadow; } }

    // Return height gap between shadow and falling tetromino.
    public int shadowGap                     { get { return m_shadowGap; } }

    // Set matrix elements to indicated value.
    private void SetMatrixCells(Cell[,] matrix, int width, int height, Cell value)
    {
        for (int i = 0; i < width; ++i)
        {
            for (int j = 0; j < height; ++j)
            {
                matrix[i, j] = value;
            }
        }
    }

    // Initialize tetromino cells for every type of tetromino.
    private void SetTetromino(TetrominoType indexTetromino, StcTetromino tetromino)
    {
        // Initialize tetromino cells to empty cells.
        SetMatrixCells(tetromino.cells, TETROMINO_SIZE, TETROMINO_SIZE, Cell.EMPTY);

        // Almost all the blocks have size 3.
        tetromino.size = TETROMINO_SIZE - 1;

        // Initial configuration from: http://tetris.wikia.com/wiki/SRS
        switch (indexTetromino)
        {
            case TetrominoType.I:
                tetromino.cells[0, 1] = Cell.CYAN;
                tetromino.cells[1, 1] = Cell.CYAN;
                tetromino.cells[2, 1] = Cell.CYAN;
                tetromino.cells[3, 1] = Cell.CYAN;
                tetromino.size = TETROMINO_SIZE;
                break;
            case TetrominoType.O:
                tetromino.cells[0, 0] = Cell.YELLOW;
                tetromino.cells[0, 1] = Cell.YELLOW;
                tetromino.cells[1, 0] = Cell.YELLOW;
                tetromino.cells[1, 1] = Cell.YELLOW;
                tetromino.size = TETROMINO_SIZE - 2;
                break;
            case TetrominoType.T:
                tetromino.cells[0, 1] = Cell.PURPLE;
                tetromino.cells[1, 0] = Cell.PURPLE;
                tetromino.cells[1, 1] = Cell.PURPLE;
                tetromino.cells[2, 1] = Cell.PURPLE;
                break;
            case TetrominoType.S:
                tetromino.cells[0, 1] = Cell.GREEN;
                tetromino.cells[1, 0] = Cell.GREEN;
                tetromino.cells[1, 1] = Cell.GREEN;
                tetromino.cells[2, 0] = Cell.GREEN;
                break;
            case TetrominoType.Z:
                tetromino.cells[0, 0] = Cell.RED;
                tetromino.cells[1, 0] = Cell.RED;
                tetromino.cells[1, 1] = Cell.RED;
                tetromino.cells[2, 1] = Cell.RED;
                break;
            case TetrominoType.J:
                tetromino.cells[0, 0] = Cell.BLUE;
                tetromino.cells[0, 1] = Cell.BLUE;
                tetromino.cells[1, 1] = Cell.BLUE;
                tetromino.cells[2, 1] = Cell.BLUE;
                break;
            case TetrominoType.L:
                tetromino.cells[0, 1] = Cell.ORANGE;
                tetromino.cells[1, 1] = Cell.ORANGE;
                tetromino.cells[2, 0] = Cell.ORANGE;
                tetromino.cells[2, 1] = Cell.ORANGE;
                break;
        }
        tetromino.type = indexTetromino;
    }

    // Start a new game.
    private void Start()
    {
        // Initialize game data.
        m_map = new Cell[BOARD_TILEMAP_WIDTH, BOARD_TILEMAP_HEIGHT];
        m_stats = new StcStatics();
        m_fallingBlock = new StcTetromino();
        m_nextBlock = new StcTetromino();

        m_errorCode = Error.NONE;
        m_systemTime = m_platform.GetSystemTime();
        m_lastFallTime = m_systemTime;
        m_isOver = false;
        m_isPaused = false;
        m_showPreview = true;
        m_events = Event.NONE;
        m_fallingDelay = INIT_DELAY_FALL;
        m_showShadow = true;

        // Initialize game statistics.
        m_stats.score = 0;
        m_stats.lines = 0;
        m_stats.totalPieces = 0;
        m_stats.level = 0;
        for (int i = 0; i < TETROMINO_TYPES; ++i)
        {
            m_stats.pieces[i] = 0;
        }

        // Initialize random generator.
        m_platform.SeedRandom(m_systemTime);

        // Initialize game tile map.
        SetMatrixCells(m_map, BOARD_TILEMAP_WIDTH, BOARD_TILEMAP_HEIGHT, Cell.EMPTY);

        // Initialize falling tetromino.
        SetTetromino((TetrominoType)(m_platform.Random() % TETROMINO_TYPES), m_fallingBlock);
        m_fallingBlock.x = (BOARD_TILEMAP_WIDTH - m_fallingBlock.size) / 2;
        m_fallingBlock.y = 0;

        // Initialize preview tetromino.
        SetTetromino((TetrominoType)(m_platform.Random() % TETROMINO_TYPES), m_nextBlock);

        m_stateChanged = true;

        // Initialize events.
        OnTetrominoMoved();

        // Initialize delayed autoshift.
        m_delayLeft = -1;
        m_delayRight = -1;
        m_delayDown = -1;
#if STC_AUTO_ROTATION
        m_delayRotation = -1;
#endif
    }

    // Initialize the game. The error code (if any) is saved in [mErrorcode].
    public void Init(Platform targetPlatform)
    {
        // Store platform reference and start it.
        m_platform = targetPlatform;

        // Initialize platform.
        m_errorCode = m_platform.Init(this);

        if (m_errorCode == Error.NONE) {
            // If everything is OK start the game.
            Start();
        }
    }

    // Free used resources.
    private void End()
    {
        m_platform.End();
    }

    // Rotate falling tetromino. If there are no collisions when the
    // tetromino is rotated this modifies the tetromino's cell buffer.
    private void RotateTetromino(bool clockwise)
    {
        int i, j;
        // Temporary array to hold rotated cells.
        Cell[,] rotated = new Cell[TETROMINO_SIZE, TETROMINO_SIZE];  

        // If TETROMINO_O is falling return immediately.
        if (m_fallingBlock.type == TetrominoType.O)
        {
            // Rotation doesn't require any changes.
            return; 
        }

        // Initialize rotated cells to blank.
        SetMatrixCells(rotated, TETROMINO_SIZE, TETROMINO_SIZE, Cell.EMPTY);

        // Copy rotated cells to the temporary array.
        for (i = 0; i < m_fallingBlock.size; ++i)
        {
            for (j = 0; j < m_fallingBlock.size; ++j)
            {
                if (clockwise)
                {
                    rotated[m_fallingBlock.size - j - 1, i] = m_fallingBlock.cells[i, j];
                }
                else
                {
                    rotated[j, m_fallingBlock.size - i - 1] = m_fallingBlock.cells[i, j];
                }
            }
        }
#if STC_WALL_KICK_ENABLED
        int wallDisplace = 0;

        // Check collision with left wall.
        if (m_fallingBlock.x < 0)
        {
            for (i = 0; (wallDisplace == 0) && (i < -m_fallingBlock.x); ++i) 
            {
                for (j = 0; j < m_fallingBlock.size; ++j)
                {
                    if (rotated[i, j] != Cell.EMPTY)
                    {
                        wallDisplace = i - m_fallingBlock.x;
                        break;
                    }
                }
            }
        }
        // Or check collision with right wall.
        else if (m_fallingBlock.x > BOARD_TILEMAP_WIDTH - m_fallingBlock.size)
        {
            i = m_fallingBlock.size - 1;
            for (; (wallDisplace == 0) && (i >= BOARD_TILEMAP_WIDTH - m_fallingBlock.x); --i)
            {
                for (j = 0; j < m_fallingBlock.size; ++j)
                {
                    if (rotated[i, j] != Cell.EMPTY)
                    {
                        wallDisplace = -m_fallingBlock.x - i + BOARD_TILEMAP_WIDTH - 1;
                        break;
                    }
                }
            }
        }

        // Check collision with board floor and other cells on board.
        for (i = 0; i < m_fallingBlock.size; ++i) 
        {
            for (j = 0; j < m_fallingBlock.size; ++j)
            {
                if (rotated[i, j] != Cell.EMPTY)
                {
                    // Check collision with bottom border of the map.
                    if (m_fallingBlock.y + j >= BOARD_TILEMAP_HEIGHT) 
                    {
                        // There is a collision therefore return.
                        return; 
                    }
                    // Check collision with existing cells in the map.
                    if (m_map[i + m_fallingBlock.x + wallDisplace, j + m_fallingBlock.y] != Cell.EMPTY) 
                    {
                        // There is a collision therefore return.
                        return; 
                    }
                }
            }
        }
        // Move the falling piece if there was wall collision and it's a legal move.
        if (wallDisplace != 0)
        {
            m_fallingBlock.x += wallDisplace;
        }
#else
        // Check collision of the temporary array.
        for (i = 0; i < m_fallingBlock.size; ++i) 
        {
            for (j = 0; j < m_fallingBlock.size; ++j)
            {
                if (rotated[i, j] != Cell.EMPTY)
                {
                    // Check collision with left, right or bottom borders of the map.
                    if ((m_fallingBlock.x + i < 0) || (m_fallingBlock.x + i >= BOARD_TILEMAP_WIDTH)
                                                   || (m_fallingBlock.y + j >= BOARD_TILEMAP_HEIGHT))
                    {
                        // There is a collision therefore return.
                        return; 
                    }
                    // Check collision with existing cells in the map.
                    if (m_map[i + m_fallingBlock.x, j + m_fallingBlock.y] != Cell.EMPTY) 
                    {
                        // There is a collision therefore return.
                        return; 
                    }
                }
            }
        }
#endif
        // There are no collisions, replace tetromino cells with rotated cells.
        for (i = 0; i < TETROMINO_SIZE; ++i)
        {
            for (j = 0; j < TETROMINO_SIZE; ++j)
            {
                m_fallingBlock.cells[i, j] = rotated[i, j];
            }
        }
        OnTetrominoMoved();
    }

    // Check if tetromino will collide with something if it is moved in the requested direction.
    // If there are collisions returns 1 else returns 0.
    private bool CheckCollision(int dx, int dy)
    {
        int newx = m_fallingBlock.x + dx;
        int newy = m_fallingBlock.y + dy;

        for (int i = 0; i < m_fallingBlock.size; ++i)
        {
            for (int j = 0; j < m_fallingBlock.size; ++j)
            {
                if (m_fallingBlock.cells[i, j] != Cell.EMPTY)
                {
                    // Check that tetromino would be inside the left, right and bottom borders.
                    if ((newx + i < 0) || (newx + i >= BOARD_TILEMAP_WIDTH)
                                       || (newy + j >= BOARD_TILEMAP_HEIGHT))
                    {
                        return true;
                    }
                    // Check that tetromino won't collide with existing cells in the map.
                    if (m_map[newx + i, newy + j] != Cell.EMPTY) 
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    // Game scoring: http://tetris.wikia.com/wiki/Scoring
    private void OnFilledRows(int filledRows)
    {
        // Update total number of filled rows.
        m_stats.lines += filledRows;

        // Increase score accordingly to the number of filled rows.
        switch (filledRows)
        {
            case 1:
                m_stats.score += (SCORE_1_FILLED_ROW * (m_stats.level + 1));
                break;
            case 2:
                m_stats.score += (SCORE_2_FILLED_ROW * (m_stats.level + 1));
                break;
            case 3:
                m_stats.score += (SCORE_3_FILLED_ROW * (m_stats.level + 1));
                break;
            case 4:
                m_stats.score += (SCORE_4_FILLED_ROW * (m_stats.level + 1));
                break;
            default:
                // This shouldn't happen, but if happens kill the game.
                m_errorCode = Error.ASSERT;
                break;
        }

        // Check if we need to update the level.
        if (m_stats.lines >= FILLED_ROWS_FOR_LEVEL_UP * (m_stats.level + 1)) {
            m_stats.level++;

            // Increase speed for falling tetrominoes.
            m_fallingDelay = (int)(DELAY_FACTOR_FOR_LEVEL_UP * m_fallingDelay 
                                  / DELAY_DIVISOR_FOR_LEVEL_UP);
        }
    }

    // Move tetromino in the direction specified by (x, y) (in tile units)
    // This function detects if there are filled rows or if the move
    // lands a falling tetromino, also checks for game over condition.
    private void MoveTetromino(int x, int y)
    {
        int i, j;

        // Check if the move would create a collision.
        if (CheckCollision(x, y)) 
        {
            // In case of collision check if move was downwards (y == 1)
            if (y == 1)
            {
                // Check if collision occurs when the falling
                // tetromino is on the 1st or 2nd row.
                if (m_fallingBlock.y <= 1)
                {
                    // If this happens the game is over.
                    m_isOver = true;   
                }
                else
                {
                    // The falling tetromino has reached the bottom,
                    // so we copy their cells to the board map.
                    for (i = 0; i < m_fallingBlock.size; ++i) 
                    {
                        for (j = 0; j < m_fallingBlock.size; ++j) 
                        {
                            if (m_fallingBlock.cells[i, j] != Cell.EMPTY)
                            {
                                m_map[m_fallingBlock.x + i, m_fallingBlock.y + j]
                                            = m_fallingBlock.cells[i, j];
                            }
                        }
                    }

                    // Check if the landing tetromino has created full rows.
                    int numFilledRows = 0;
                    for (j = 1; j < BOARD_TILEMAP_HEIGHT; ++j)
                    {
                        bool hasFullRow = true;
                        for (i = 0; i < BOARD_TILEMAP_WIDTH; ++i)
                        {
                            if (m_map[i, j] == Cell.EMPTY)
                            {
                                hasFullRow = false;
                                break;
                            }
                        }
                        // If we found a full row we need to remove that row from the map
                        // we do that by just moving all the above rows one row below.
                        if (hasFullRow) 
                        {
                            for (x = 0; x < BOARD_TILEMAP_WIDTH; ++x)
                            {
                                for (y = j; y > 0; --y)
                                {
                                    m_map[x, y] = m_map[x, y - 1];
                                }
                            }
                            // Increase filled row counter.
                            numFilledRows++;    
                        }
                    }

                    // Update game statistics.
                    if (numFilledRows > 0) 
                    {
                        OnFilledRows(numFilledRows);
                    }
                    m_stats.totalPieces++;
                    m_stats.pieces[(int)m_fallingBlock.type]++;

                    // Use preview tetromino as falling tetromino.
                    // Copy preview tetromino for falling tetromino.
                    for (i = 0; i < TETROMINO_SIZE; ++i) 
                    {
                        for (j = 0; j < TETROMINO_SIZE; ++j)
                        {
                            m_fallingBlock.cells[i, j] = m_nextBlock.cells[i, j];
                        }
                    }
                    m_fallingBlock.size = m_nextBlock.size;
                    m_fallingBlock.type = m_nextBlock.type;

                    // Reset position.
                    m_fallingBlock.y = 0;
                    m_fallingBlock.x = (BOARD_TILEMAP_WIDTH - m_fallingBlock.size) / 2;
                    OnTetrominoMoved();

                    // Create next preview tetromino.
                    SetTetromino((TetrominoType)(m_platform.Random() % TETROMINO_TYPES), m_nextBlock);

                    m_stateChanged = true;
                }
            }
        }
        else
        {
            // There are no collisions, just move the tetromino.
            m_fallingBlock.x += x;
            m_fallingBlock.y += y;
        }
        OnTetrominoMoved();
    }

    // Hard drop.
    private void DropTetromino()
    {
        // Shadow has already calculated the landing position.
        m_fallingBlock.y += m_shadowGap;

        // Force lock.
        MoveTetromino(0, 1); 

        // Update score.
        if (m_showShadow)
        {
            m_stats.score += (long)(SCORE_2_FILLED_ROW * (m_stats.level + 1) 
                                   / SCORE_DROP_WITH_SHADOW_DIVISOR);
        }
        else 
        {
            m_stats.score += (long)(SCORE_2_FILLED_ROW * (m_stats.level + 1)
                                   / SCORE_DROP_DIVISOR);
        }
    }

    // This event is called when the falling tetromino is moved.
    private void OnTetrominoMoved()
    {
        int y = 0;
        // Calculate number of cells where shadow tetromino would be.
        while (!CheckCollision(0, ++y));
        m_shadowGap = y - 1;
        m_stateMoved = true;
    }

    // Main game function called every frame.
    public void Update()
    {
        // Read player input.
        m_platform.ProcessEvents();

        // Update game state.
        if (m_isOver) 
        {
            if ((m_events & Event.RESTART) != 0)
            {
                m_isOver = false;
                Start();
            }
        }
        else
        {
            long currentTime = m_platform.GetSystemTime();

            // Process delayed autoshift.
            int timeDelta = (int)(currentTime - m_systemTime);
            if (m_delayDown > 0)
            {
                m_delayDown -= timeDelta;
                if (m_delayDown <= 0) 
                {
                    m_delayDown = DAS_MOVE_TIMER;
                    m_events |= Event.MOVE_DOWN;
                }
            }
            if (m_delayLeft > 0) 
            {
                m_delayLeft -= timeDelta;
                if (m_delayLeft <= 0)
                {
                    m_delayLeft = DAS_MOVE_TIMER;
                    m_events |= Event.MOVE_LEFT;
                }
            }
            else if (m_delayRight > 0)
            {
                m_delayRight -= timeDelta;
                if (m_delayRight <= 0) 
                {
                    m_delayRight = DAS_MOVE_TIMER;
                    m_events |= Event.MOVE_RIGHT;
                }
            }
#if STC_AUTO_ROTATION
            if (m_delayRotation > 0)
            {
                m_delayRotation -= timeDelta;
                if (m_delayRotation <= 0) 
                {
                    m_delayRotation = ROTATION_AUTOREPEAT_TIMER;
                    m_events |= Event.ROTATE_CW;
                }
            }
#endif
            // Always handle pause event.
            if ((m_events & Event.PAUSE) != 0) 
            {
                m_isPaused = !m_isPaused;
                m_events = Event.NONE;
            }

            // Check if the game is paused.
            if (m_isPaused) 
            {
                // We achieve the effect of pausing the game
                // adding the last frame duration to lastFallTime.
                m_lastFallTime += (currentTime - m_systemTime);
            }
            else 
            {
                if (m_events != Event.NONE) 
                {
                    if ((m_events & Event.SHOW_NEXT) != 0) 
                    {
                        m_showPreview = !m_showPreview;
                        m_stateChanged = true;
                    }
                    if ((m_events & Event.SHOW_SHADOW) != 0)
                    {
                        m_showShadow = !m_showShadow;
                        m_stateChanged = true;
                    }
                    if ((m_events & Event.DROP) != 0) 
                    {
                        DropTetromino();
                    }
                    if ((m_events & Event.ROTATE_CW) != 0)
                    {
                        RotateTetromino(true);
                    }

                    if ((m_events & Event.MOVE_RIGHT) != 0) 
                    {
                        MoveTetromino(1, 0);
                    }
                    else if ((m_events & Event.MOVE_LEFT) != 0)
                    {
                        MoveTetromino(-1, 0);
                    }

                    if ((m_events & Event.MOVE_DOWN) != 0)
                    {
                        // Update score if the player accelerates downfall.
                        m_stats.score += (long)(SCORE_2_FILLED_ROW * (m_stats.level + 1) 
                                               / SCORE_MOVE_DOWN_DIVISOR);

                        MoveTetromino(0, 1);
                    }
                    m_events = Event.NONE;
                }
                // Check if it's time to move downwards the falling tetromino.
                if (currentTime - m_lastFallTime >= m_fallingDelay) 
                {
                    MoveTetromino(0, 1);
                    m_lastFallTime = currentTime;
                }
            }
            // Save current time for next game update.
            m_systemTime = currentTime;
        }
        // Draw game state.
        m_platform.RenderGame();
    }

    // Process a key down event.
    public void OnEventStart(Event command)
    {
        switch (command)
        {
            case Event.QUIT:
                m_errorCode = Error.PLAYER_QUITS;
                break;
            case Event.MOVE_DOWN:
                m_events |= Event.MOVE_DOWN;
                m_delayDown = DAS_DELAY_TIMER;
                break;
            case Event.ROTATE_CW:
                m_events |= Event.ROTATE_CW;
#if STC_AUTO_ROTATION
                m_delayRotation = ROTATION_AUTOREPEAT_DELAY;
#endif
                break;
            case Event.MOVE_LEFT:
                m_events |= Event.MOVE_LEFT;
                m_delayLeft = DAS_DELAY_TIMER;
                break;
            case Event.MOVE_RIGHT:
                m_events |= Event.MOVE_RIGHT;
                m_delayRight = DAS_DELAY_TIMER;
                break;
            case Event.DROP:        // Fall through
            case Event.RESTART:     // Fall through
            case Event.PAUSE:       // Fall through
            case Event.SHOW_NEXT:   // Fall through
            case Event.SHOW_SHADOW:
                m_events |= command;
                break;
        }
    }

    // Process a key up event.
    public void OnEventEnd(Event command)
    {
        switch (command)
        {
            case Event.MOVE_DOWN:
                m_delayDown = -1;
                break;
            case Event.MOVE_LEFT:
                m_delayLeft = -1;
                break;
            case Event.MOVE_RIGHT:
                m_delayRight = -1;
                break;
#if STC_AUTO_ROTATION
            case Event.ROTATE_CW:
                m_delayRotation = -1;
                break;
#endif
        }
    }

    // Game events are stored in bits in this variable.
    // It must be cleared to Event.NONE after being used.
    private Event m_events;

    // Matrix that holds the cells (tilemap)
    private Cell[,] m_map = new Cell[BOARD_TILEMAP_WIDTH, BOARD_TILEMAP_HEIGHT];

    private Platform     m_platform;     // platform interface
    private StcStatics   m_stats;        // statistic data
    private StcTetromino m_fallingBlock; // current falling tetromino
    private StcTetromino m_nextBlock;    // next tetromino

    private bool  m_stateChanged;   // true if game state has changed
    private bool  m_stateMoved;     // true if falling tetromino has moved
    private Error m_errorCode;      // stores current error code
    private bool  m_isPaused;       // true if the game is over
    private bool  m_isOver;         // true if the game is over
    private bool  m_showPreview;    // true if we must show the preview block

    private bool m_showShadow; // true if we must show the shadow block
    private int  m_shadowGap;  // distance between falling block and shadow

    private long m_systemTime;   // system time in milliseconds
    private int  m_fallingDelay; // delay time for falling tetrominoes
    private long m_lastFallTime; // last time the falling tetromino dropped

    // For delayed autoshift: http://tetris.wikia.com/wiki/DAS
    private int  m_delayLeft;
    private int  m_delayRight;
    private int  m_delayDown;
#if STC_AUTO_ROTATION
    private int  m_delayRotation;
#endif
}
}
#endif