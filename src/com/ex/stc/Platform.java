/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   J2ME implementation.                                                     */
/*   CLDC 1.1 - MIDP 2.0 (176 x 220 screen size)                              */
/*   In memorial of my Sony Ericsson W810i, my best phone ever.               */
/*                                                                            */
/*   Copyright (c) 2010 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

package com.ex.stc;

import java.io.IOException;
import java.util.Random;
import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.game.Sprite;

public class Platform extends Canvas implements Runnable {
    /*
     * UI layout (quantities are expressed in pixels)
     */

    /* Screen size */
    private static final int SCREEN_WIDTH   = 176;
    private static final int SCREEN_HEIGHT  = 220;

    /* Size of square tile */
    private static final int TILE_SIZE      = 10;

    /* Board up-left corner coordinates */
    private static final int BOARD_X0        = 0;
    private static final int BOARD_Y0        = -1;
    private static final int BOARD_X1        = BOARD_X0 + TILE_SIZE * Game.BOARD_WIDTH;
    private static final int BOARD_Y1        = BOARD_Y0 + TILE_SIZE * Game.BOARD_HEIGHT;

    /* Preview tetromino position */
    private static final int PREVIEW_X      = 127;
    private static final int PREVIEW_Y      = 60;

    /* Score position and length on screen */
    private static final int SCORE_X        = 88;
    private static final int SCORE_Y        = 2;
    private static final int SCORE_LENGTH   = 10;

    /* Lines position and length on screen */
    private static final int LINES_X        = 123;
    private static final int LINES_Y        = 17;
    private static final int LINES_LENGTH   = 5;

    /* Level position and length on screen */
    private static final int LEVEL_X        = 123;
    private static final int LEVEL_Y        = 32;
    private static final int LEVEL_LENGTH   = 5;

    /* Tetromino subtotals position */
    private static final int TETROMINO_X    = 123;
    private static final int TETROMINO_L_Y  = 100;
    private static final int TETROMINO_I_Y  = 115;
    private static final int TETROMINO_T_Y  = 130;
    private static final int TETROMINO_S_Y  = 145;
    private static final int TETROMINO_Z_Y  = 160;
    private static final int TETROMINO_O_Y  = 175;
    private static final int TETROMINO_J_Y  = 190;
    private static final int TETROMINO_LENGTH   = 5;

    /* Tetromino total position */
    private static final int PIECES_X       = 116;
    private static final int PIECES_Y       = 205;
    private static final int PIECES_LENGTH  = 6;

    /* Size of number */
    private static final int NUMBER_WIDTH   = 7;
    private static final int NUMBER_HEIGHT  = 9;

    /* Colors */
    private static final int BACK_COLOR     = 0x424242;
    private static final int GRID_COLOR_LO  = 0x585858;
    private static final int GRID_COLOR_HI  = 0x818181;

    /*
     * Image files
     */
    private static final String BMP_TILE_BLOCKS = "/blocks.png";
    private static final String BMP_NUMBERS     = "/numbers.png";

    /* Delayed autoshift initial delay */
    private static final int DAS_DELAY_TIMER    = 200;

    /* Delayed autoshift timer for left and right moves */
    private static final int DAS_MOVE_TIMER     = 40;

    /* Rotation auto-repeat delay */
    private static final int ROTATION_AUTOREPEAT_DELAY = 375;

    /* Rotation autorepeat timer */
    private static final int ROTATION_AUTOREPEAT_TIMER = 200;

    /* Game instance */
    private Game mGame;

    /* Images of tile blocks and numbers */
    private Image  mImageTiles;
    private Image  mImageNumbers;

    private Random mRandomGenerator;

    /* For delayed autoshift: http://tetris.wikia.com/wiki/DAS */
    private int delayLeft;
    private int delayRight;
    private int delayDown;
    private int delayRotation;
    private long lastTime;

    /*
     * Initializes platform, if there are no problems returns GAME_ERROR_NONE.
     */
    public Platform() {
        /* Create game */
        mGame = new Game();
        mGame.init(this);

        /* Load images for blocks and numbers */
        try {
            mImageTiles = Image.createImage(BMP_TILE_BLOCKS);
            mImageNumbers = Image.createImage(BMP_NUMBERS);
        }
        catch(IOException ioexception) {
            System.out.println("Error loading images: " + ioexception);
        }

        /* Initialize delayed autoshift */
        lastTime = System.currentTimeMillis();
        delayLeft = -1;
        delayRight = -1;
        delayDown = -1;
        if (Game.STC_AUTO_ROTATION) {
            delayRotation = -1;
        }
    }

    /*
     * Release platform allocated resources
     */
    public void end() {
        mGame.end();
        mGame = null;
    }

    /* Return the current system time in milliseconds */
    public long getSystemTime() {
        return System.currentTimeMillis();
    }

    /* Initialize the random number generator */
    public void seedRandom(long seed) {
        mRandomGenerator = new Random();
    }

    /* Return a random positive integer number */
    public int random() {
        return Math.abs(mRandomGenerator.nextInt());
    }

    private void drawTile(Graphics graphics, int x, int y, int tile, boolean shadow) {
        int sourceX = TILE_SIZE * tile;
        int sourceY = shadow? TILE_SIZE : 0;
        graphics.drawRegion(mImageTiles, sourceX, sourceY,
                            TILE_SIZE, TILE_SIZE, Sprite.TRANS_NONE, x, y, 0);
    }

    private void drawNumber(Graphics graphics, int x, int y, long number, int length, int color) {
        int sourceX, destineX;
        int sourceY = NUMBER_HEIGHT * color;
        int pos = 0;
        do {
            destineX  = x + NUMBER_WIDTH * (length - pos);
            sourceX = NUMBER_WIDTH * (int)(number % 10);

            graphics.drawRegion(mImageNumbers, sourceX, sourceY,
                                NUMBER_WIDTH, NUMBER_HEIGHT, Sprite.TRANS_NONE, destineX, y, 0);
            number /= 10;
        } while (++pos < length);
    }

    /*
     * Render the state of the game using platform functions.
     */
    public void paint(Graphics graphics) {
        int i, j;

        /* Check if the game state has changed, if so redraw */
        if (mGame.stateChanged) {

            /* Draw background. */
            graphics.setColor(BACK_COLOR);
            graphics.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

            graphics.setColor(GRID_COLOR_LO);
            for (i = 0; i <= Game.BOARD_WIDTH; ++i) {
                graphics.drawLine(BOARD_X0 + i * TILE_SIZE, BOARD_Y0, BOARD_X0 + i * TILE_SIZE, BOARD_Y1);
            }
            for (i = 0; i <= Game.BOARD_HEIGHT; ++i) {
                graphics.drawLine(BOARD_X0, BOARD_Y0 + i * TILE_SIZE, BOARD_X1, BOARD_Y0 + i * TILE_SIZE);
            }
            graphics.setColor(GRID_COLOR_HI);
            graphics.drawLine(BOARD_X0, BOARD_Y0 + 2 * TILE_SIZE, BOARD_X0, BOARD_Y1);
            graphics.drawLine(BOARD_X1, BOARD_Y0 + 2 * TILE_SIZE, BOARD_X1, BOARD_Y1);
            graphics.drawLine(BOARD_X0, BOARD_Y0 + 2 * TILE_SIZE, BOARD_X1, BOARD_Y0 + 2 * TILE_SIZE);
            graphics.drawLine(BOARD_X0, BOARD_Y1, BOARD_X1, BOARD_Y1);

            /* Draw preview block */
            if (mGame.showPreview) {
                for (i = 0; i < Game.TETROMINO_SIZE; ++i) {
                    for (j = 0; j < Game.TETROMINO_SIZE; ++j) {
                        if (mGame.nextBlock.cells[i][j] != Game.EMPTY_CELL) {
                            drawTile(graphics,
                                     PREVIEW_X + (TILE_SIZE * i),
                                     PREVIEW_Y + (TILE_SIZE * j),
                                     mGame.nextBlock.cells[i][j], false);
                        }
                    }
                }
            }

            if (Game.STC_SHOW_GHOST_PIECE) {
                /* Draw shadow tetromino */
                if (mGame.showShadow && mGame.shadowGap > 0) {
                    for (i = 0; i < Game.TETROMINO_SIZE; ++i) {
                        for (j = 0; j < Game.TETROMINO_SIZE; ++j) {
                            if (mGame.fallingBlock.cells[i][j] != Game.EMPTY_CELL) {
                                drawTile(graphics,
                                         BOARD_X0 + (TILE_SIZE * (mGame.fallingBlock.x + i)),
                                         BOARD_Y0 + (TILE_SIZE * (mGame.fallingBlock.y + mGame.shadowGap + j)),
                                         mGame.fallingBlock.cells[i][j], true);
                            }
                        }
                    }
                }
            }

            /* Draw the cells in the board */
            for (i = 0; i < Game.BOARD_WIDTH; ++i) {
                for (j = 0; j < Game.BOARD_HEIGHT; ++j) {
                    if (mGame.map[i][j] != Game.EMPTY_CELL) {
                        drawTile(graphics,
                                 BOARD_X0 + (TILE_SIZE * i),
                                 BOARD_Y0 + (TILE_SIZE * j),
                                 mGame.map[i][j], false);
                    }
                }
            }

            /* Draw falling tetromino */
            for (i = 0; i < Game.TETROMINO_SIZE; ++i) {
                for (j = 0; j < Game.TETROMINO_SIZE; ++j) {
                    if (mGame.fallingBlock.cells[i][j] != Game.EMPTY_CELL) {
                        drawTile(graphics,
                                 BOARD_X0 + (TILE_SIZE * (mGame.fallingBlock.x + i)),
                                 BOARD_Y0 + (TILE_SIZE * (mGame.fallingBlock.y + j)),
                                 mGame.fallingBlock.cells[i][j], false);
                    }
                }
            }

            /* Draw game statistic data */
            if (!mGame.isPaused) {
                drawNumber(graphics, LEVEL_X, LEVEL_Y, mGame.stats.level, LEVEL_LENGTH, Game.COLOR_WHITE);
                drawNumber(graphics, LINES_X, LINES_Y, mGame.stats.lines, LINES_LENGTH, Game.COLOR_WHITE);
                drawNumber(graphics, SCORE_X, SCORE_Y, mGame.stats.score, SCORE_LENGTH, Game.COLOR_WHITE);

                drawNumber(graphics, TETROMINO_X, TETROMINO_L_Y, mGame.stats.pieces[Game.TETROMINO_L], TETROMINO_LENGTH, Game.COLOR_ORANGE);
                drawNumber(graphics, TETROMINO_X, TETROMINO_I_Y, mGame.stats.pieces[Game.TETROMINO_I], TETROMINO_LENGTH, Game.COLOR_CYAN);
                drawNumber(graphics, TETROMINO_X, TETROMINO_T_Y, mGame.stats.pieces[Game.TETROMINO_T], TETROMINO_LENGTH, Game.COLOR_PURPLE);
                drawNumber(graphics, TETROMINO_X, TETROMINO_S_Y, mGame.stats.pieces[Game.TETROMINO_S], TETROMINO_LENGTH, Game.COLOR_GREEN);
                drawNumber(graphics, TETROMINO_X, TETROMINO_Z_Y, mGame.stats.pieces[Game.TETROMINO_Z], TETROMINO_LENGTH, Game.COLOR_RED);
                drawNumber(graphics, TETROMINO_X, TETROMINO_O_Y, mGame.stats.pieces[Game.TETROMINO_O], TETROMINO_LENGTH, Game.COLOR_YELLOW);
                drawNumber(graphics, TETROMINO_X, TETROMINO_J_Y, mGame.stats.pieces[Game.TETROMINO_J], TETROMINO_LENGTH, Game.COLOR_BLUE);

                drawNumber(graphics, PIECES_X, PIECES_Y, mGame.stats.totalPieces, PIECES_LENGTH, Game.COLOR_WHITE);
            }

            /* Clear the game state */
            mGame.stateChanged = false;
        }
    }

    /* Read input device and notify game */
    public void keyPressed(int keyCode) {
        switch (getGameAction(keyCode)) {
        case DOWN:
            mGame.events |= Game.EVENT_MOVE_DOWN;
            delayDown = DAS_DELAY_TIMER;
            break;
        case UP:
            mGame.events |= Game.EVENT_ROTATE_CW;
            if (Game.STC_AUTO_ROTATION) {
                delayRotation = ROTATION_AUTOREPEAT_DELAY;
            }
            break;
        case LEFT:
            mGame.events |= Game.EVENT_MOVE_LEFT;
            delayLeft = DAS_DELAY_TIMER;
            break;
        case RIGHT:
            mGame.events |= Game.EVENT_MOVE_RIGHT;
            delayRight = DAS_DELAY_TIMER;
            break;
        case FIRE:
            mGame.events |= Game.EVENT_DROP;
            break;
        }
    }

    public void keyReleased(int keyCode) {
        switch (getGameAction(keyCode)) {
        case DOWN:
            delayDown = -1;
            break;
        case LEFT:
            delayLeft = -1;
            break;
        case RIGHT:
            delayRight = -1;
            break;
        case UP:
            if (Game.STC_AUTO_ROTATION) {
                delayRotation = -1;
            }
            break;
        }
    }

    /* Notification handler when canvas is shown */
    protected void showNotify () {
        new Thread(this).start();
    }

    public void run() {
        /* Game loop */
        while (mGame != null) {
            try {
                /* Update game */
                mGame.update();

                /* Request redraw of screen */
                repaint();

                /* Process delayed autoshift */
                long timeNow = System.currentTimeMillis();
                int timeDelta = (int)(timeNow - lastTime);
                if (delayDown > 0) {
                    delayDown -= timeDelta;
                    if (delayDown <= 0) {
                        delayDown = DAS_MOVE_TIMER;
                        mGame.events |= Game.EVENT_MOVE_DOWN;
                    }
                }
                if (delayLeft > 0) {
                    delayLeft -= timeDelta;
                    if (delayLeft <= 0) {
                        delayLeft = DAS_MOVE_TIMER;
                        mGame.events |= Game.EVENT_MOVE_LEFT;
                    }
                }
                else if (delayRight > 0) {
                    delayRight -= timeDelta;
                    if (delayRight <= 0) {
                        delayRight = DAS_MOVE_TIMER;
                        mGame.events |= Game.EVENT_MOVE_RIGHT;
                    }
                }
                if (Game.STC_AUTO_ROTATION) {
                    if (delayRotation > 0) {
                        delayRotation -= timeDelta;
                        if (delayRotation <= 0) {
                            delayRotation = ROTATION_AUTOREPEAT_TIMER;
                            mGame.events |= Game.EVENT_ROTATE_CW;
                        }
                    }
                }
                lastTime = timeNow;

                /* Resting game */
                Thread.sleep(DAS_MOVE_TIMER);
            }
            catch (java.lang.InterruptedException e) {
                /* Ignore */
            }
        }
    }

    /* Necessary in some devices if using full screen mode. */
    protected void sizeChanged(int w, int h) {
        mGame.stateChanged = true;
        repaint();
    }
}
