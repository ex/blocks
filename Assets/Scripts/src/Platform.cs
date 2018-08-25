/* ========================================================================== */
/*   Platform.cs                                                              */
/*   Unity specific implementation.                                           */
/* -------------------------------------------------------------------------- */
/*   Copyright (c) 2011 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */
#if DEBUG || STC_USE_DLL

using UnityEngine;

namespace Stc
{
    public class Platform
    {
        // Board up-left corner coordinates
        private const int BOARD_X = -4;
        private const int BOARD_Y = 11;
        private const int BOARD_Z = 17;

        // Preview tetromino position
        private const int PREVIEW_X = -13;
        private const int PREVIEW_Y = -7;

        // Initializes platform.
        public Game.Error Init( Game game )
        {
            m_game = game;
            m_random = new System.Random();
            m_tetromino = new GameObject[Game.TETROMINO_SIZE, Game.TETROMINO_SIZE];
            m_shadow = new GameObject[Game.TETROMINO_SIZE, Game.TETROMINO_SIZE];
            m_next = new GameObject[Game.TETROMINO_SIZE, Game.TETROMINO_SIZE];
            m_board = new GameObject[Game.BOARD_TILEMAP_WIDTH, Game.BOARD_TILEMAP_HEIGHT];

            m_soundDrop = Bridge.Instantiate( "AudioDrop", Vector3.zero ).GetComponent<AudioSource>();
            m_soundDrop.playOnAwake = false;
            m_soundLine = Bridge.Instantiate( "AudioLine", Vector3.zero ).GetComponent<AudioSource>();
            m_soundLine.playOnAwake = false;
            m_music = Bridge.Instantiate( "AudioMusic", Vector3.zero ).GetComponent<AudioSource>();
            m_music.playOnAwake = false;
            m_music.loop = true;
            m_intro = Bridge.Instantiate( "AudioIntro", Vector3.zero ).GetComponent<AudioSource>();
            m_intro.Play();
            m_music.PlayDelayed( m_intro.clip.length );

            // Create background.
            m_background = new GameObject[4 * Game.BOARD_TILEMAP_HEIGHT];
            for ( int i = 0, k = 0; i < Game.BOARD_TILEMAP_HEIGHT; ++i )
            {
                m_background[k++] = GetTile( BOARD_X - 1, BOARD_Y - i, Game.Cell.WHITE );
                m_background[k++] = GetTile( BOARD_X + Game.BOARD_TILEMAP_WIDTH, BOARD_Y - i, Game.Cell.WHITE );
                m_background[k++] = GetTile( BOARD_X - 1, BOARD_Y - i, Game.Cell.WHITE, true, 2 * BOARD_Z );
                m_background[k++] = GetTile( BOARD_X + Game.BOARD_TILEMAP_WIDTH, BOARD_Y - i, Game.Cell.WHITE, true, 2 * BOARD_Z );
            }

            return Game.Error.NONE;
        }

        // Process events and notify game.
        public void ProcessEvents()
        {
            if ( Input.GetKeyDown( KeyCode.LeftArrow ) || Input.GetKeyDown( KeyCode.A ) )
            {
                m_game.OnEventStart( Game.Event.MOVE_LEFT );
            }
            if ( Input.GetKeyDown( KeyCode.RightArrow ) || Input.GetKeyDown( KeyCode.D ) )
            {
                m_game.OnEventStart( Game.Event.MOVE_RIGHT );
            }
            if ( Input.GetKeyDown( KeyCode.DownArrow ) || Input.GetKeyDown( KeyCode.S ) )
            {
                m_game.OnEventStart( Game.Event.MOVE_DOWN );
            }
            if ( Input.GetKeyDown( KeyCode.UpArrow ) || Input.GetKeyDown( KeyCode.W ) )
            {
                m_game.OnEventStart( Game.Event.ROTATE_CW );
            }
            if ( Input.GetKeyDown( KeyCode.Space ) )
            {
                m_game.OnEventStart( Game.Event.DROP );
            }
            if ( Input.GetKeyDown( KeyCode.F5 ) )
            {
                m_game.OnEventStart( Game.Event.RESTART );
            }
            if ( Input.GetKeyDown( KeyCode.F1 ) )
            {
                m_game.OnEventStart( Game.Event.PAUSE );
            }
            if ( Input.GetKeyDown( KeyCode.F2 ) )
            {
                m_game.OnEventStart( Game.Event.SHOW_NEXT );
            }
            if ( Input.GetKeyDown( KeyCode.F3 ) )
            {
                m_game.OnEventStart( Game.Event.SHOW_SHADOW );
            }

            if ( Input.GetKeyUp( KeyCode.LeftArrow ) || Input.GetKeyUp( KeyCode.A ) )
            {
                m_game.OnEventEnd( Game.Event.MOVE_LEFT );
            }
            if ( Input.GetKeyUp( KeyCode.RightArrow ) || Input.GetKeyUp( KeyCode.D ) )
            {
                m_game.OnEventEnd( Game.Event.MOVE_RIGHT );
            }
            if ( Input.GetKeyUp( KeyCode.DownArrow ) || Input.GetKeyUp( KeyCode.S ) )
            {
                m_game.OnEventEnd( Game.Event.MOVE_DOWN );
            }
            if ( Input.GetKeyUp( KeyCode.UpArrow ) || Input.GetKeyUp( KeyCode.W ) )
            {
                m_game.OnEventEnd( Game.Event.ROTATE_CW );
            }
        }

        public void End()
        {
            int i, j;
            for ( i = 0; i < Game.TETROMINO_SIZE; ++i )
            {
                for ( j = 0; j < Game.TETROMINO_SIZE; ++j )
                {
                    if ( m_tetromino[i, j] != null )
                    {
                        Bridge.Destroy( m_tetromino[i, j] );
                    }
                    if ( m_shadow[i, j] != null )
                    {
                        Bridge.Destroy( m_shadow[i, j] );
                    }
                    if ( m_next[i, j] != null )
                    {
                        Bridge.Destroy( m_next[i, j] );
                    }
                    m_tetromino[i, j] = null;
                    m_shadow[i, j] = null;
                    m_next[i, j] = null;
                }
            }
            for ( i = 0; i < Game.BOARD_TILEMAP_WIDTH; ++i )
            {
                for ( j = 0; j < Game.BOARD_TILEMAP_HEIGHT; ++j )
                {
                    if ( m_board[i, j] != null )
                    {
                        Bridge.Destroy( m_board[i, j] );
                    }
                    m_board[i, j] = null;
                }
            }
            for ( i = 0; i < m_background.Length; ++i )
            {
                Bridge.Destroy( m_background[i] );
                m_background[i] = null;
            }
            m_game = null;
            m_random = null;
        }

        // Create a tetromino tile.
        private GameObject GetTile( int x, int y, Game.Cell idTile, bool shadow = false, int z = BOARD_Z )
        {
            switch ( idTile )
            {
                case Game.Cell.BLUE:
                    return Bridge.Instantiate( shadow ? "blueS" : "blue", new Vector3( x, y, z ) );
                case Game.Cell.RED:
                    return Bridge.Instantiate( shadow ? "redS" : "red", new Vector3( x, y, z ) );
                case Game.Cell.GREEN:
                    return Bridge.Instantiate( shadow ? "greenS" : "green", new Vector3( x, y, z ) );
                case Game.Cell.CYAN:
                    return Bridge.Instantiate( shadow ? "cyanS" : "cyan", new Vector3( x, y, z ) );
                case Game.Cell.ORANGE:
                    return Bridge.Instantiate( shadow ? "orangeS" : "orange", new Vector3( x, y, z ) );
                case Game.Cell.PURPLE:
                    return Bridge.Instantiate( shadow ? "purpleS" : "purple", new Vector3( x, y, z ) );
                case Game.Cell.YELLOW:
                    return Bridge.Instantiate( shadow ? "yellowS" : "yellow", new Vector3( x, y, z ) );
                case Game.Cell.WHITE:
                    return Bridge.Instantiate( shadow ? "whiteS" : "whiteS", new Vector3( x, y, z ) );
            }
            return null;
        }

        // Render the state of the game using platform functions.
        public void RenderGame()
        {
            int i, j;

            // Check if the falling tetromino has moved, if so redraw.
            if ( m_game.hasMove )
            {
                // Draw falling tetromino
                for ( i = 0; i < Game.TETROMINO_SIZE; ++i )
                {
                    for ( j = 0; j < Game.TETROMINO_SIZE; ++j )
                    {
                        if ( m_tetromino[i, j] != null )
                        {
                            Bridge.Destroy( m_tetromino[i, j] );
                            m_tetromino[i, j] = null;
                        }
                        if ( m_game.fallingBlock.cells[i, j] != Game.Cell.EMPTY )
                        {
                            m_tetromino[i, j] = GetTile( BOARD_X + m_game.fallingBlock.x + i,
                                                        BOARD_Y - m_game.fallingBlock.y - j,
                                                        m_game.fallingBlock.cells[i, j] );
                        }
                    }
                }

                // Draw shadow tetromino
                if ( m_game.showShadow && ( m_game.shadowGap >= 0 ) )
                {
                    for ( i = 0; i < Game.TETROMINO_SIZE; ++i )
                    {
                        for ( j = 0; j < Game.TETROMINO_SIZE; ++j )
                        {
                            if ( m_shadow[i, j] != null )
                            {
                                Bridge.Destroy( m_shadow[i, j] );
                                m_shadow[i, j] = null;
                            }
                            if ( m_game.fallingBlock.cells[i, j] != Game.Cell.EMPTY )
                            {
                                m_shadow[i, j] = GetTile( BOARD_X + m_game.fallingBlock.x + i,
                                                         BOARD_Y - m_game.fallingBlock.y - m_game.shadowGap - j,
                                                         m_game.fallingBlock.cells[i, j], true );
                            }
                        }
                    }
                }

                // Inform to the game that we are done with the move.
                m_game.OnMoveProcessed();
            }

            // Check if the game state has changed, if so redraw.
            if ( m_game.hasChanged )
            {
                // Draw preview block
                if ( m_game.showPreview )
                {
                    for ( i = 0; i < Game.TETROMINO_SIZE; ++i )
                    {
                        for ( j = 0; j < Game.TETROMINO_SIZE; ++j )
                        {
                            if ( m_next[i, j] != null )
                            {
                                Bridge.Destroy( m_next[i, j] );
                                m_next[i, j] = null;
                            }
                            if ( m_game.nextBlock.cells[i, j] != Game.Cell.EMPTY )
                            {
                                m_next[i, j] = GetTile( PREVIEW_X + i, PREVIEW_Y - j,
                                                       m_game.nextBlock.cells[i, j] );
                                m_next[i, j].transform.localScale = new Vector3( 1.0f, 1.0f, 0.85f );
                            }
                        }
                    }
                }
                // Draw the cells in the board
                for ( i = 0; i < Game.BOARD_TILEMAP_WIDTH; ++i )
                {
                    for ( j = 0; j < Game.BOARD_TILEMAP_HEIGHT; ++j )
                    {
                        if ( m_game.GetCell( i, j ) == Game.Cell.EMPTY )
                        {
                            if ( m_board[i, j] != null )
                            {
                                Bridge.Destroy( m_board[i, j] );
                                m_board[i, j] = null;
                                UnityEngine.Debug.Log( "delete: " + i + "," + j );
                            }
                        }
                        else
                        {
                            if ( m_board[i, j] == null )
                            {
                                m_board[i, j] = GetTile( BOARD_X + i, BOARD_Y - j,
                                                        m_game.GetCell( i, j ) );
                                m_board[i, j].tag = m_game.GetCell( i, j ).ToString();
                                UnityEngine.Debug.Log( "create: " + i + "," + j );
                            }
                            else
                            {
                                if ( !m_board[i, j].CompareTag( m_game.GetCell( i, j ).ToString() ) )
                                {
                                    Bridge.Destroy( m_board[i, j] );
                                    m_board[i, j] = GetTile( BOARD_X + i, BOARD_Y - j,
                                                            m_game.GetCell( i, j ) );
                                    m_board[i, j].tag = m_game.GetCell( i, j ).ToString();
                                    UnityEngine.Debug.Log( "replace: " + i + "," + j );
                                }
                            }
                        }
                    }
                }

                // Inform to the game that we are done with the changed state.
                m_game.OnChangeProcessed();
            }
        }

        public long GetSystemTime()
        {
            return (long)( 1000 * Time.realtimeSinceStartup );
        }

        public void SeedRandom( long seed )
        {
            // Not neccessary when using System.Random.
        }

        public int Random()
        {
            return m_random.Next();
        }

        // Events
        public void OnLineCompleted()
        {
            m_soundLine.Play();
        }

        public void OnPieceDrop()
        {
            if ( !m_soundLine.isPlaying )
            {
                m_soundDrop.Play();
            }
        }

        private Game m_game;
        private System.Random m_random;
        private GameObject[,] m_board;
        private GameObject[,] m_tetromino;
        private GameObject[,] m_shadow;
        private GameObject[,] m_next;
        private GameObject[] m_background;

        private AudioSource m_soundDrop;
        private AudioSource m_soundLine;
        private AudioSource m_intro;
        private AudioSource m_music;
    }
}
#endif