
package com.ex.stc;

import java.util.Random;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.SurfaceHolder;

public class GameActivity extends Activity {
	/** Data structure that holds information about our tetromino blocks. **/
	public class Tetromino {
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
    
	class GameState {
		SurfaceHolder surfaceHolder;
		/* Images of tile blocks and numbers */
		Bitmap  tilesBitmap;
		Bitmap  numbersBitmap;
		
		long score;         /* user score for current game      */
		int lines;          /* total number of lines cleared    */
		int totalPieces;    /* total number of tetrominoes used */
		int level;          /* current game level               */
		/*
		 * Game events are stored in bits in this variable.
		 * It must be cleared to EVENT_NONE after being used.
		 */
		int events;
		int[] pieces = new int[TETROMINO_TYPES];    /* number of tetrominoes per type */
		int[][] map = new int[BOARD_WIDTH][BOARD_HEIGHT];
		Tetromino nextBlock;     /* next tetromino               */
		Tetromino fallingBlock;  /* current falling tetromino    */
		int errorCode;              /* game error code              */
		boolean isPaused;    /* true if the game is paused, false otherwise     */
		boolean showPreview; /* true if we must show preview tetromino          */
		boolean showShadow;  /* true if we must show ghost shadow               */
		int shadowGap;       /* height gap between shadow and falling tetromino */
		boolean stateChanged;/* true if game state changed, 0 otherwise         */
		int delay;           /* delay time for falling tetrominoes              */
		boolean isOver;      /* true if the game is over, false otherwise       */
		long lastFallTime;   /* last time the game moved the falling tetromino  */
		long systemTime;     /* system time in milliseconds */
	}
	
	/**
	 * Enable shadow piece: 
	 * http://tetris.wikia.com/wiki/Ghost_piece 
	 **/
	public static final boolean STC_SHOW_GHOST_PIECE = true;
	
	/**
	 * Enable wall kick:                        
	 * http://tetris.wikia.com/wiki/Wall_kick   
	 **/
	public static final boolean STC_WALL_KICK_ENABLED = true;
	
	/**
	 * Enable auto-rotation of the falling piece 
	 **/
	public static final boolean STC_AUTO_ROTATION = true;
	
	/**
	 * Playfield size (in tiles) 
	 **/
	public static final int BOARD_WIDTH = 10;
	public static final int BOARD_HEIGHT = 22;
	
	/**
	 * Initial time delay (in milliseconds) between falling moves 
	 **/
	private static final int INIT_DELAY_FALL = 1000;
	
	/**
	 * Score points given by filled rows (we use the original NES * 10)
	 * http://tetris.wikia.com/wiki/Scoring 
	 **/
	private static final int SCORE_1_FILLED_ROW = 400;
	private static final int SCORE_2_FILLED_ROW = 1000;
	private static final int SCORE_3_FILLED_ROW = 3000;
	private static final int SCORE_4_FILLED_ROW = 12000;
	
	/** 
	 * User gets more score if he uses hard drop. (factor of SCORE_2_FILLED_ROW) 
	 **/
	private static final double SCORE_DROP_FACTOR = 0.05;
	private static final double SCORE_DROP_WITH_SHADOW_FACTOR = 0.01;
	
	/**
	 * User gets points every time he accelerates downfall (factor of SCORE_2_FILLED_ROW) 
	 **/
	private static final double SCORE_MOVE_DOWN_FACTOR = 0.001;
	
	/**
	 * Number of filled rows required to increase the game level 
	 **/
	private static final int FILLED_ROWS_FOR_LEVEL_UP = 10;
	
	/**
	 * The falling delay is multiplied by this factor with every level up 
	 **/
	private static final double DELAY_FACTOR_FOR_LEVEL_UP = 0.9;
	
	/**
	 * Delayed autoshift initial delay 
	 **/
	protected static final int DAS_DELAY_TIMER  = 200;
	
	/** 
	 * Delayed autoshift timer for left and right moves 
	 **/
	private static final int DAS_MOVE_TIMER     = 40;
	
	/** 
	 * Rotation auto-repeat delay 
	 **/
	private static final int ROTATION_AUTOREPEAT_DELAY = 375;
	
	/**
	 * Rotation autorepeat timer 
	 **/
	private static final int ROTATION_AUTOREPEAT_TIMER = 200;
	
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
	public static final int COLOR_WHITE  = 0;   /* Used for effects (if any) */
	
	/* This value used for empty tiles */
	public static final int EMPTY_CELL = -1;
	
	/**
	 * Shared between the game view and game runner
	 */
	private GameState gameState;
	
	private GameView gameView;
	
	private GameRunner gameRunner;
	
	private GameControls gameControls;

	private GestureDetector gestureDetector;
	
	public static String TAG = "stc-android";
	
	/**
	 * Called when the activity is first created.
	 * @param savedInstanceState If the activity is being re-initialized after 
	 * previously being shut down then this Bundle contains the data it most 
	 * recently supplied in onSaveInstanceState(Bundle). <b>Note: Otherwise it is null.</b>
	 */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Log.i(TAG, "onCreate");
		setContentView(R.layout.main);
		
		DisplayMetrics metrics = new DisplayMetrics();
		getWindowManager().getDefaultDisplay().getMetrics(metrics);
		
		gameControls = new GameControls(this, metrics.widthPixels, metrics.heightPixels);
		gestureDetector = new GestureDetector(this, gameControls);
		gestureDetector.setOnDoubleTapListener(this.gameControls);
		
		gameView = (GameView) findViewById(R.id.game);
		gameView.setOnKeyListener(gameControls);
		
		gameState = new GameState();
		gameView.setGameState(gameState);

		gameRunner = new GameRunner();
		gameRunner.execute(gameState);
	}
	
	@Override
	protected void onPause() {
		super.onStop();
		
		gameRunner.cancel(true);
	}
	
	
	
	
	class GameRunner extends AsyncTask<GameState, GameState, Void> {
		private Random randomGenerator = new Random();
		private long lastTime;
		
		@Override
		protected Void doInBackground(GameState... states) {  // TODO: check if we're loading a saved state.
			GameState gameState = states[0];
			/* Load images for blocks and numbers */
			gameState.tilesBitmap = ((BitmapDrawable) getResources().getDrawable(R.drawable.blocks)).getBitmap();
			gameState.numbersBitmap = ((BitmapDrawable) getResources().getDrawable(R.drawable.numbers)).getBitmap();
			/* Initialize delayed autoshift */
			lastTime = System.currentTimeMillis();
			delayLeft = -1;
			delayRight = -1;
			delayDown = -1;
			if (GameActivity.STC_AUTO_ROTATION) {
			    delayRotation = -1;
			}
			
			start();
			
			/* Game loop */
			while (gameState != null) {
				/* Update game */
				update();
				
				/* Process delayed autoshift */
				long timeNow = System.currentTimeMillis();
				int timeDelta = (int)(timeNow - lastTime);
				if (delayDown > 0) {
					delayDown -= timeDelta;
					if (delayDown <= 0) {
						delayDown = DAS_MOVE_TIMER;
						gameState.events |= EVENT_MOVE_DOWN;
					}
				}
				if (delayLeft > 0) {
					delayLeft -= timeDelta;
					if (delayLeft <= 0) {
						delayLeft = DAS_MOVE_TIMER;
						gameState.events |= EVENT_MOVE_LEFT;
					}
				}
				else if (delayRight > 0) {
					delayRight -= timeDelta;
					if (delayRight <= 0) {
						delayRight = DAS_MOVE_TIMER;
						gameState.events |= EVENT_MOVE_RIGHT;
					}
				}
				if (STC_AUTO_ROTATION) {
					if (delayRotation > 0) {
						delayRotation -= timeDelta;
						if (delayRotation <= 0) {
							delayRotation = ROTATION_AUTOREPEAT_TIMER;
							gameState.events |= EVENT_ROTATE_CW;
						}
					}
				}
				lastTime = timeNow;

				/* Resting game */
				try {
					Thread.sleep(DAS_MOVE_TIMER);
				} catch (InterruptedException e) {
					Log.i(TAG, "Interrupted while resting between moves", e);
				}
			}
			
			return null;
		}
		
		/* Start a new game */
		private void start() {
			gameState.nextBlock = new Tetromino();
			gameState.fallingBlock = new Tetromino();
			/* Initialize game data */
			gameState.errorCode = GAME_ERROR_NONE;
			gameState.systemTime = System.currentTimeMillis();
			gameState.lastFallTime = gameState.systemTime;
			gameState.isOver = false;
			gameState.isPaused = false;
			gameState.showPreview = true;
			gameState.events = EVENT_NONE;
			gameState.delay = INIT_DELAY_FALL;
			if (STC_SHOW_GHOST_PIECE) {
				gameState.showShadow = true;
			}
			
			/* Initialize game statistics */
			gameState.score = 0;
			gameState.lines = 0;
			gameState.totalPieces = 0;
			gameState.level = 0;
			for (int i = 0; i < TETROMINO_TYPES; ++i) {
				gameState.pieces[i] = 0;
			}
			
			/* Initialize game tile map */
			setMatrixCells(gameState.map, BOARD_WIDTH, BOARD_HEIGHT, EMPTY_CELL);
			
			/* Initialize falling tetromino */
			setTetromino(randomGenerator.nextInt(TETROMINO_TYPES), gameState.fallingBlock);
			gameState.fallingBlock.x = (BOARD_WIDTH - gameState.fallingBlock.size) / 2;
			gameState.fallingBlock.y = 0;
			
			/* Initialize preview tetromino */
			setTetromino(randomGenerator.nextInt(TETROMINO_TYPES), gameState.nextBlock);
			
			/* Initialize events */
			onTetrominoMoved();
			
		}
		
		/**
		 * Main function game called every frame 
		 **/
		public void update() {
			/* Update game state */
			if (gameState.isOver) {
				if ((gameState.events & EVENT_RESTART) != 0) {
					gameState.isOver = false;
					start();
				}
			}
			else {
				long sysTime = System.currentTimeMillis();

				/* Always handle pause event */
				if ((gameState.events & EVENT_PAUSE) != 0) {
					gameState.isPaused = !gameState.isPaused;
					gameState.events = EVENT_NONE;
				}

				/* Check if the game is paused */
				if (gameState.isPaused) {
					/* We achieve the effect of pausing the game
					 * adding the last frame duration to lastFallTime */
					gameState.lastFallTime += (sysTime - gameState.systemTime);
				}
				else {
					if (gameState.events != EVENT_NONE) {
						if ((gameState.events & EVENT_SHOW_NEXT) != 0) {
							gameState.showPreview = !gameState.showPreview;
							gameState.stateChanged = true;
						}
						if (STC_SHOW_GHOST_PIECE) {
							if ((gameState.events & EVENT_SHOW_SHADOW) != 0) {
								gameState.showShadow = !gameState.showShadow;
								gameState.stateChanged = true;
							}
						}
						if ((gameState.events & EVENT_DROP) != 0) {
							dropTetromino();
						}
						if ((gameState.events & EVENT_ROTATE_CW) != 0) {
							rotateTetromino(true);
						}
						if ((gameState.events & EVENT_ROTATE_CCW) != 0) {
							rotateTetromino(false);
						}
						if ((gameState.events & EVENT_MOVE_RIGHT) != 0) {
							moveTetromino(1, 0);
						}
						else if ((gameState.events & EVENT_MOVE_LEFT) != 0) {
							moveTetromino(-1, 0);
						}
						if ((gameState.events & EVENT_MOVE_DOWN) != 0) {
							/* Update score if the user accelerates downfall */
							gameState.score += (long)(SCORE_MOVE_DOWN_FACTOR * (SCORE_2_FILLED_ROW * (gameState.level + 1)));

							moveTetromino(0, 1);
						}
						gameState.events = EVENT_NONE;
					}
					/* Check if it's time to move downwards the falling tetromino */
					if (sysTime - gameState.lastFallTime >= gameState.delay) {
						moveTetromino(0, 1);
						gameState.lastFallTime = sysTime;
					}
				}
				gameState.systemTime = sysTime;
			}
		}
		
		
		/**
		 * Rotate falling tetromino. If there are no collisions when the
		 * tetromino is rotated this modifies the tetromino's cell buffer.
		 * @param clockwise true to rotate clockwise
		 **/
		private void rotateTetromino(boolean clockwise) {
			int i, j;
			int[][] rotated = new int[TETROMINO_SIZE][TETROMINO_SIZE];  /* temporary array to hold rotated cells */

			/* If TETROMINO_O is falling return immediately */
			if (gameState.fallingBlock.type == TETROMINO_O) {
				return; /* rotation doesn't require any changes */
			}

			/* Initialize rotated cells to blank */
			setMatrixCells(rotated, TETROMINO_SIZE, TETROMINO_SIZE, EMPTY_CELL);

			/* Copy rotated cells to the temporary array */
			for (i = 0; i < gameState.fallingBlock.size; ++i) {
				for (j = 0; j < gameState.fallingBlock.size; ++j) {
					if (clockwise) {
						rotated[gameState.fallingBlock.size - j - 1][i] = gameState.fallingBlock.cells[i][j];
					} else {
						rotated[j][gameState.fallingBlock.size - i - 1] = gameState.fallingBlock.cells[i][j];
					}
				}
			}

			if (STC_WALL_KICK_ENABLED) {
				int wallDisplace = 0;

				/* Check collision with left wall */
				if (gameState.fallingBlock.x < 0) {
					for (i = 0; (wallDisplace == 0) && (i < -gameState.fallingBlock.x); ++i) {
						for (j = 0; j < gameState.fallingBlock.size; ++j) {
							if (rotated[i][j] != EMPTY_CELL) {
								wallDisplace = i - gameState.fallingBlock.x;
								break;
							}
						}
					}
				}
				/* Or check collision with right wall */
				else if (gameState.fallingBlock.x > BOARD_WIDTH - gameState.fallingBlock.size) {
					i = gameState.fallingBlock.size - 1;
					for (; (wallDisplace == 0) && (i >= BOARD_WIDTH - gameState.fallingBlock.x); --i) {
						for (j = 0; j < gameState.fallingBlock.size; ++j) {
							if (rotated[i][j] != EMPTY_CELL) {
								wallDisplace = -gameState.fallingBlock.x - i + BOARD_WIDTH - 1;
								break;
							}
						}
					}
				}

				/* Check collision with board floor and other cells on board */
				for (i = 0; i < gameState.fallingBlock.size; ++i) {
					for (j = 0; j < gameState.fallingBlock.size; ++j) {
						if (rotated[i][j] != EMPTY_CELL) {
							/* Check collision with bottom border of the map */
							if (gameState.fallingBlock.y + j >= BOARD_HEIGHT) {
								return; /* there was collision therefore return */
							}
							/* Check collision with existing cells in the map */
							if (gameState.map[i + gameState.fallingBlock.x + wallDisplace][j + gameState.fallingBlock.y] != EMPTY_CELL) {
								return; /* there was collision therefore return */
							}
						}
					}
				}
				/* Move the falling piece if there was wall collision and it's a legal move */
				if (wallDisplace != 0) {
					gameState.fallingBlock.x += wallDisplace;
				}
			}
			else {
				/* Check collision of the temporary array */
				for (i = 0; i < gameState.fallingBlock.size; ++i) {
					for (j = 0; j < gameState.fallingBlock.size; ++j) {
						if (rotated[i][j] != EMPTY_CELL) {
							/* Check collision with left, right or bottom borders of the map */
							if ((gameState.fallingBlock.x + i < 0) || (gameState.fallingBlock.x + i >= BOARD_WIDTH)
									|| (gameState.fallingBlock.y + j >= BOARD_HEIGHT)) {
								return; /* there was collision therefore return */
							}
							/* Check collision with existing cells in the map */
							if (gameState.map[i + gameState.fallingBlock.x][j + gameState.fallingBlock.y] != EMPTY_CELL) {
								return; /* there was collision therefore return */
							}
						}
					}
				}
			}
			/* There are no collisions, replace tetromino cells with rotated cells */
			for (i = 0; i < TETROMINO_SIZE; ++i) {
				for (j = 0; j < TETROMINO_SIZE; ++j) {
					gameState.fallingBlock.cells[i][j] = rotated[i][j];
				}
			}
			onTetrominoMoved();
		}
		
		/**
		 * Hard drop 
		 **/
		private void dropTetromino() {
			if (STC_SHOW_GHOST_PIECE) {
				moveTetromino(0, gameState.shadowGap);
				moveTetromino(0, 1); /* Force lock */

				/* Update score */
				if (gameState.showShadow) {
					gameState.score += (long)(SCORE_DROP_WITH_SHADOW_FACTOR * SCORE_2_FILLED_ROW * (gameState.level + 1));
				}
				else {
					gameState.score += (long)(SCORE_DROP_FACTOR * SCORE_2_FILLED_ROW * (gameState.level + 1));
				}
			}
			else {
				int y = 0;
				/* Calculate number of cells to drop */
				while (!checkCollision(0, ++y));
				moveTetromino(0, y - 1);
				moveTetromino(0, 1); /* Force lock */

				/* Update score */
				gameState.score += (long)(SCORE_DROP_FACTOR * SCORE_2_FILLED_ROW * (gameState.level + 1));
			}
		}

		/**
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
					if (gameState.fallingBlock.y <= 1) {
						gameState.isOver = true;   /* if this happens the game is over */
					}
					else {
						/* The falling tetromino has reached the bottom,
						 * so we copy their cells to the board map */
						for (i = 0; i < gameState.fallingBlock.size; ++i) {
							for (j = 0; j < gameState.fallingBlock.size; ++j) {
								if (gameState.fallingBlock.cells[i][j] != EMPTY_CELL) {
									gameState.map[gameState.fallingBlock.x + i][gameState.fallingBlock.y + j]
											= gameState.fallingBlock.cells[i][j];
								}
							}
						}

						/* Check if the landing tetromino has created full rows */
						int numFilledRows = 0;
						for (j = 1; j < BOARD_HEIGHT; ++j) {
							boolean hasFullRow = true;
							for (i = 0; i < BOARD_WIDTH; ++i) {
								if (gameState.map[i][j] == EMPTY_CELL) {
									hasFullRow = false;
									break;
								}
							}
							/* If we found a full row we need to remove that row from the map
							 * we do that by just moving all the above rows one row below */
							if (hasFullRow) {
								for (x = 0; x < BOARD_WIDTH; ++x) {
									for (y = j; y > 0; --y) {
										gameState.map[x][y] = gameState.map[x][y - 1];
									}
								}
								numFilledRows++;    /* increase filled row counter */
							}
						}

						/* Update game statistics */
						if (numFilledRows > 0) {
							onFilledRows(numFilledRows);
						}
						gameState.totalPieces++;
						gameState.pieces[gameState.fallingBlock.type]++;

						/* Use preview tetromino as falling tetromino.
						 * Copy preview tetromino for falling tetromino */
						for (i = 0; i < TETROMINO_SIZE; ++i) {
							for (j = 0; j < TETROMINO_SIZE; ++j) {
								gameState.fallingBlock.cells[i][j] = gameState.nextBlock.cells[i][j];
							}
						}
						gameState.fallingBlock.size = gameState.nextBlock.size;
						gameState.fallingBlock.type = gameState.nextBlock.type;

						/* Reset position */
						gameState.fallingBlock.y = 0;
						gameState.fallingBlock.x = (BOARD_WIDTH - gameState.fallingBlock.size) / 2;
						onTetrominoMoved();

						/* Create next preview tetromino */
						setTetromino(randomGenerator.nextInt(TETROMINO_TYPES), gameState.nextBlock);
					}
				}
			}
			else {
				/* There are no collisions, just move the tetromino */
				gameState.fallingBlock.x += x;
				gameState.fallingBlock.y += y;
			}
			onTetrominoMoved();
		}
		
		/**
		 * Game scoring: http://tetris.wikia.com/wiki/Scoring 
		 * @param filledRows the number of rows filled
		 **/
		private void onFilledRows(int filledRows) {
			/* Update total number of filled rows */
			gameState.lines += filledRows;

			/* Increase score accordingly to the number of filled rows */
			switch (filledRows) {
			case 1:
				gameState.score += (SCORE_1_FILLED_ROW * (gameState.level + 1));
				break;
			case 2:
				gameState.score += (SCORE_2_FILLED_ROW * (gameState.level + 1));
				break;
			case 3:
				gameState.score += (SCORE_3_FILLED_ROW * (gameState.level + 1));
				break;
			case 4:
				gameState.score += (SCORE_4_FILLED_ROW * (gameState.level + 1));
				break;
			default:
				gameState.errorCode = GAME_ERROR_ASSERT;    /* This can't happen */
			}
			/* Check if we need to update level */
			if (gameState.lines >= FILLED_ROWS_FOR_LEVEL_UP * (gameState.level + 1)) {
				gameState.level++;

				/* Increase speed for falling tetrominoes */
				gameState.delay = (int)(gameState.delay * DELAY_FACTOR_FOR_LEVEL_UP);
			}
		}
		
		/**
		 * Initialize tetromino cells for every type of tetromino 
		 * @param indexTetromino 0 to 6, representing a tetromino
		 * @param tetromino the tetromino object to set
		 */
		private void setTetromino(int indexTetromino, Tetromino tetromino) {

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
		
		/**
		 * Set matrix elements to indicated value 
		 * @param matrix
		 * @param width
		 * @param height
		 * @param value
		 */
		private void setMatrixCells(int[][] matrix, int width, int height, int value) {
			for (int i = 0; i < width; ++i) {
				for (int j = 0; j < height; ++j) {
					matrix[i][j] = value;
				}
			}
		}
		
		/**
		 * This event is called when the falling tetromino is moved 
		 **/
		private void onTetrominoMoved() {
			if (STC_SHOW_GHOST_PIECE) {
				int y = 0;
				/* Calculate number of cells where shadow tetromino would be */
				while (!checkCollision(0, ++y));
				gameState.shadowGap = y - 1;
			}
			gameState.stateChanged = true;
		}
		
		/**
		 * Check if tetromino will collide with something if it is moved in the requested direction.
		 * If there are collisions returns true else returns false.
		 * @param dx
		 * @param dy
		 * @return
		 */
		private boolean checkCollision(int dx, int dy) {

			int newx = gameState.fallingBlock.x + dx;
			int newy = gameState.fallingBlock.y + dy;

			for (int i = 0; i < gameState.fallingBlock.size; ++i) {
				for (int j = 0; j < gameState.fallingBlock.size; ++j) {
					if (gameState.fallingBlock.cells[i][j] != EMPTY_CELL) {
						/* Check the tetromino would be inside the left, right and bottom borders */
						if ((newx + i < 0) || (newx + i >= BOARD_WIDTH)
								|| (newy + j >= BOARD_HEIGHT)) {
							return true;
						}
						/* Check the tetromino won't collide with existing cells in the map */
						if (gameState.map[newx + i][newy + j] != EMPTY_CELL) {
							return true;
						}
					}
				}
			}
			return false;
		}
	}
	
	/* For delayed autoshift: http://tetris.wikia.com/wiki/DAS */
	private int delayLeft;
	private int delayRight;
	private int delayDown;
	private int delayRotation;
	
	/* control interaction - GameControls class uses these */
	/**
	 * Start moving right
	 */
	public void moveRightStart() {
		gameState.events |= EVENT_MOVE_RIGHT;
		delayRight = DAS_DELAY_TIMER;
	}
	
	/**
	 * Stop moving right
	 */
	public void moveRightEnd() {
		delayRight = -1;
	}
	
	/**
	 * Start moving left
	 */
	public void moveLeftStart() {
		gameState.events |= EVENT_MOVE_LEFT;
		delayLeft = DAS_DELAY_TIMER;
	}
	
	/**
	 * Stop moving left
	 */
	public void moveLeftEnd() {
		delayLeft = -1;
	}
	
	/**
	 * Start rotating the piece clockwise
	 */
	public void rotateCWStart() {
		gameState.events |= EVENT_ROTATE_CW;
		if (STC_AUTO_ROTATION) {
			delayRotation = ROTATION_AUTOREPEAT_DELAY;
		}
	}
	
	/**
	 * Stop rotating the piece clockwise
	 */
	public void rotateCWEnd() {
		if (STC_AUTO_ROTATION) {
			delayRotation = -1;
		}
	}
	
	/**
	 * Start rotating the piece counterclockwise
	 */
	public void rotateCCWStart() {
		gameState.events |= EVENT_ROTATE_CCW;
		if (STC_AUTO_ROTATION) {
			delayRotation = ROTATION_AUTOREPEAT_DELAY;
		}
	}
	
	/**
	 * Stop rotating the piece counterclockwise
	 */
	public void rotateCCWEnd() {
		if (STC_AUTO_ROTATION) {
			delayRotation = -1;
		}
	}
	
	/**
	 * Drop the piece
	 */
	public void drop() {
		gameState.events |= EVENT_DROP;
	}
	
	/** 
	 * Start moving the piece down fast
	 */
	public void moveDownStart() {
		gameState.events |= EVENT_MOVE_DOWN;
		delayDown = DAS_DELAY_TIMER;
	}
	
	/**
	 * Stop moving the piece down fast
	 */
	public void moveDownEnd() {
		delayDown = -1;
	}

	/**
	 * Gesture detector handles touch controls
	 */
	@Override
	public boolean onTouchEvent(MotionEvent event) {
		return gestureDetector.onTouchEvent(event);
	}
}
