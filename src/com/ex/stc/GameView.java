
package com.ex.stc;

import com.ex.stc.GameActivity.GameState;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Paint.Style;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * Handles the drawing of the game as well as input handling.
 */
public class GameView extends SurfaceView implements SurfaceHolder.Callback {
	private GameState gameState;
	private DrawThread drawThread;
	
	/*
	 * UI layout (quantities are expressed in pixels)
	 */
	
	/* Size of square tile */
	private int tileSize  = 10;
	private int TILE_SPRITE_SIZE = 10;
	
	/* Board up-left corner coordinates */
	private int boardX0        = 0;
	private int boardY0        = -1;
	private int boardX1        = boardX0 + tileSize * GameActivity.BOARD_WIDTH;
	private int boardY1        = boardY0 + tileSize * GameActivity.BOARD_HEIGHT;
	
	/* Preview tetromino position */
	private static final int PREVIEW_X      = 4;
	private static final int PREVIEW_Y      = 60;
	
	/* Score position and length on screen */
	private static final int SCORE_X        = 0;
	private static final int SCORE_Y        = 2;
	private static final int SCORE_LENGTH   = 10;
	
	/* Lines position and length on screen */
	private static final int LINES_X        = 0;
	private static final int LINES_Y        = 17;
	private static final int LINES_LENGTH   = 5;
	
	/* Level position and length on screen */
	private static final int LEVEL_X        = 0;
	private static final int LEVEL_Y        = 32;
	private static final int LEVEL_LENGTH   = 5;
	
	/* Tetromino subtotals position */
	private static final int TETROMINO_X    = 0;
	private static final int TETROMINO_L_Y  = 100;
	private static final int TETROMINO_I_Y  = 115;
	private static final int TETROMINO_T_Y  = 130;
	private static final int TETROMINO_S_Y  = 145;
	private static final int TETROMINO_Z_Y  = 160;
	private static final int TETROMINO_O_Y  = 175;
	private static final int TETROMINO_J_Y  = 190;
	private static final int TETROMINO_LENGTH   = 5;
	
	/* Tetromino total position */
	private static final int PIECES_X       = 0;
	private static final int PIECES_Y       = 205;
	private static final int PIECES_LENGTH  = 6;
	
	/* Size of number */
	private static final int NUMBER_WIDTH   = 7;
	private static final int NUMBER_HEIGHT  = 9;
	
	/* Colors */
	private static final int BACK_COLOR     = 0xFF424242;
	private static final int GRID_COLOR_LO  = 0xFF585858;
	private static final int GRID_COLOR_HI  = 0xFF818181;
	
	/**
	 * This thread does the painting to the surface.
	 * @author yincrash
	 *
	 */
	class DrawThread extends Thread {
		Paint gridColorLo;
		Paint playAreaPaint;
		float[] verticalLines;
		float[] horizontalLines;
		SurfaceHolder surfaceHolder;
		
		private void determineBoardMetrics() {
			int width = getWidth();
			int height = getHeight();
			tileSize = (height - 20) / GameActivity.BOARD_HEIGHT;
			if (tileSize * GameActivity.BOARD_WIDTH > width) // board is too fat! use width as constraining size
				tileSize = (width - 20) / GameActivity.BOARD_WIDTH;
			
			boardX0 = (width / 2) - (tileSize * GameActivity.BOARD_WIDTH / 2);
			boardY0 = -1;
			
			boardX1 = boardX0 + tileSize * GameActivity.BOARD_WIDTH;
			boardY1 = boardY0 + tileSize * GameActivity.BOARD_HEIGHT;
			
		}
		
		@Override
		public void run() {
			super.run();
			determineBoardMetrics();
			gridColorLo = new Paint();
			gridColorLo.setColor(GRID_COLOR_LO);
			playAreaPaint = new Paint();
			playAreaPaint.setColor(GRID_COLOR_HI);
			playAreaPaint.setStyle(Style.STROKE);
			
			int i, j;
			verticalLines = new float[(GameActivity.BOARD_WIDTH + 1) * 4];
			for (i = 0; i <= GameActivity.BOARD_WIDTH; ++i) {
				j = i * 4;
				verticalLines[j++] = boardX0 + i * tileSize;
				verticalLines[j++] = boardY0;
				verticalLines[j++] = boardX0 + i * tileSize;
				verticalLines[j] = boardY1;
			}
			horizontalLines = new float[(GameActivity.BOARD_HEIGHT + 1) * 4];
			for (i = 0; i <= GameActivity.BOARD_HEIGHT; ++i) {
				j = i * 4;
				horizontalLines[j++] = boardX0;
				horizontalLines[j++] = boardY0 + i * tileSize;
				horizontalLines[j++] = boardX1;
				horizontalLines[j] = boardY0 + i * tileSize;
			}
			while (!isInterrupted()) {
				Canvas c = null;
				if (gameState.stateChanged) {
					try {
						c = surfaceHolder.lockCanvas(null);
						synchronized (surfaceHolder) {
							paint(c);
						}
					} finally {
						// do this in a finally so that if an exception is thrown
						// during the above, we don't leave the Surface in an
						// inconsistent state
						if (c != null) {
							surfaceHolder.unlockCanvasAndPost(c);
						}
					}
				} else {
					try {
						Thread.sleep(50);
					} catch (InterruptedException e) {
						return;
					}
				}
			}
		}
		
		void paint(Canvas canvas) {
			int i, j;

			/* Check if the game state has changed, if so redraw */
			if (gameState.stateChanged) {
				synchronized (gameState) {
					/* Draw background. */
					canvas.drawColor(BACK_COLOR);
					
					canvas.drawLines(verticalLines, gridColorLo);
					canvas.drawLines(horizontalLines, gridColorLo);
					
					canvas.drawRect(boardX0, boardY0 + 2 * tileSize, boardX1, boardY1, playAreaPaint);
	
					/* Draw preview block */
					if (gameState.showPreview) {
						for (i = 0; i < GameActivity.TETROMINO_SIZE; ++i) {
							for (j = 0; j < GameActivity.TETROMINO_SIZE; ++j) {
								if (gameState.nextBlock.cells[i][j] != GameActivity.EMPTY_CELL) {
									drawTile(canvas,
											PREVIEW_X + (TILE_SPRITE_SIZE * i),
											PREVIEW_Y + (TILE_SPRITE_SIZE * j),
											gameState.nextBlock.cells[i][j], false, TILE_SPRITE_SIZE);
								}
							}
						}
					}
	
					if (GameActivity.STC_SHOW_GHOST_PIECE) {
						/* Draw shadow tetromino */
						if (gameState.showShadow && gameState.shadowGap > 0) {
							for (i = 0; i < GameActivity.TETROMINO_SIZE; ++i) {
								for (j = 0; j < GameActivity.TETROMINO_SIZE; ++j) {
									if (gameState.fallingBlock.cells[i][j] != GameActivity.EMPTY_CELL) {
										drawTile(canvas,
												boardX0 + (tileSize * (gameState.fallingBlock.x + i)),
												boardY0 + (tileSize * (gameState.fallingBlock.y + gameState.shadowGap + j)),
												gameState.fallingBlock.cells[i][j], true);
									}
								}
							}
						}
					}
	
					/* Draw the cells in the board */
					for (i = 0; i < GameActivity.BOARD_WIDTH; ++i) {
						for (j = 0; j < GameActivity.BOARD_HEIGHT; ++j) {
							if (gameState.map[i][j] != GameActivity.EMPTY_CELL) {
								drawTile(canvas,
										boardX0 + (tileSize * i),
										boardY0 + (tileSize * j),
										gameState.map[i][j], false);
							}
						}
					}
	
					/* Draw falling tetromino */
					for (i = 0; i < GameActivity.TETROMINO_SIZE; ++i) {
						for (j = 0; j < GameActivity.TETROMINO_SIZE; ++j) {
							if (gameState.fallingBlock.cells[i][j] != GameActivity.EMPTY_CELL) {
								drawTile(canvas,
										boardX0 + (tileSize * (gameState.fallingBlock.x + i)),
										boardY0 + (tileSize * (gameState.fallingBlock.y + j)),
										gameState.fallingBlock.cells[i][j], false);
							}
						}
					}
	
					/* Draw game statistic data */
					if (!gameState.isPaused) {
						drawNumber(canvas, LEVEL_X, LEVEL_Y, gameState.level, LEVEL_LENGTH, GameActivity.COLOR_WHITE);
						drawNumber(canvas, LINES_X, LINES_Y, gameState.lines, LINES_LENGTH, GameActivity.COLOR_WHITE);
						drawNumber(canvas, SCORE_X, SCORE_Y, gameState.score, SCORE_LENGTH, GameActivity.COLOR_WHITE);
	
						drawNumber(canvas, TETROMINO_X, TETROMINO_L_Y, gameState.pieces[GameActivity.TETROMINO_L], TETROMINO_LENGTH, GameActivity.COLOR_ORANGE);
						drawNumber(canvas, TETROMINO_X, TETROMINO_I_Y, gameState.pieces[GameActivity.TETROMINO_I], TETROMINO_LENGTH, GameActivity.COLOR_CYAN);
						drawNumber(canvas, TETROMINO_X, TETROMINO_T_Y, gameState.pieces[GameActivity.TETROMINO_T], TETROMINO_LENGTH, GameActivity.COLOR_PURPLE);
						drawNumber(canvas, TETROMINO_X, TETROMINO_S_Y, gameState.pieces[GameActivity.TETROMINO_S], TETROMINO_LENGTH, GameActivity.COLOR_GREEN);
						drawNumber(canvas, TETROMINO_X, TETROMINO_Z_Y, gameState.pieces[GameActivity.TETROMINO_Z], TETROMINO_LENGTH, GameActivity.COLOR_RED);
						drawNumber(canvas, TETROMINO_X, TETROMINO_O_Y, gameState.pieces[GameActivity.TETROMINO_O], TETROMINO_LENGTH, GameActivity.COLOR_YELLOW);
						drawNumber(canvas, TETROMINO_X, TETROMINO_J_Y, gameState.pieces[GameActivity.TETROMINO_J], TETROMINO_LENGTH, GameActivity.COLOR_BLUE);
	
						drawNumber(canvas, PIECES_X, PIECES_Y, gameState.totalPieces, PIECES_LENGTH, GameActivity.COLOR_WHITE);
					}
	
					/* Clear the game state */
					gameState.stateChanged = false;
				}
			}
		}
		
		private void drawNumber(Canvas canvas, int x, int y, long number, int length, int color) {
			int sourceX, destX;
			int sourceY = NUMBER_HEIGHT * color;
			int pos = 0;
			do {
				destX  = x + NUMBER_WIDTH * (length - pos);
				sourceX = NUMBER_WIDTH * (int)(number % 10);
				
				canvas.drawBitmap(gameState.numbersBitmap, new Rect(sourceX, sourceY, sourceX + NUMBER_WIDTH, sourceY + NUMBER_HEIGHT),
						new Rect(destX, y, destX + NUMBER_WIDTH, y + NUMBER_HEIGHT), null);
				number /= 10;
			} while (++pos < length);
		}

		private void drawTile(Canvas canvas, int x, int y, int tile, boolean shadow) {
			drawTile(canvas, x, y, tile, shadow, tileSize);
		}
		
		private void drawTile(Canvas canvas, int x, int y, int tile, boolean shadow, int size) {
			int sourceX = TILE_SPRITE_SIZE * tile;
			int sourceY = shadow ? TILE_SPRITE_SIZE : 0;
			canvas.drawBitmap(gameState.tilesBitmap, new Rect(sourceX, sourceY, sourceX + TILE_SPRITE_SIZE, sourceY + TILE_SPRITE_SIZE),
					new Rect(x, y, x + size, y + size), null);
		}
		
	}
	
	public GameView(Context context, AttributeSet attrs) {
		super(context, attrs);
		// register our interest in hearing about changes to our surface
		SurfaceHolder holder = getHolder();
		holder.addCallback(this);
		setFocusable(true);
	}
	
	/* (non-Javadoc)
	 * @see android.view.SurfaceHolder.Callback#surfaceChanged(android.view.SurfaceHolder, int, int, int)
	 */
	@Override
	public void surfaceChanged(SurfaceHolder surfaceHolder, int arg1, int arg2, int arg3) {
		Log.i(GameActivity.TAG, "surfaceChanged");
		if (drawThread != null) {
			Log.i(GameActivity.TAG, "canceling previous draw thread");
			drawThread.interrupt();
			while (drawThread.isAlive()) {
				try {
					drawThread.join();
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		}
		drawThread = new DrawThread();
		drawThread.surfaceHolder = surfaceHolder;
		drawThread.start();
	}

	/* (non-Javadoc)
	 * @see android.view.SurfaceHolder.Callback#surfaceCreated(android.view.SurfaceHolder)
	 */
	@Override
	public void surfaceCreated(SurfaceHolder surfaceHolder) {
		Log.i(GameActivity.TAG, "surfaceCreated");
	}

	/* (non-Javadoc)
	 * @see android.view.SurfaceHolder.Callback#surfaceDestroyed(android.view.SurfaceHolder)
	 */
	@Override
	public void surfaceDestroyed(SurfaceHolder arg0) {
		Log.i(GameActivity.TAG, "surfaceDestroyed");
		if (drawThread != null) {
			Log.i(GameActivity.TAG, "canceling previous draw thread");
			drawThread.interrupt();
		}
	}

	public void setGameState(GameState gameState) {
		this.gameState = gameState;
	}

}
