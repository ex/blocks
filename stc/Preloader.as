/* ========================================================================== */
/*   Preloader.as                                                             */
/*   Copyright (c) 2012 Laurens Rodriguez Oscanoa.                            */
/* -------------------------------------------------------------------------- */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

package stc {

import flash.display.Bitmap;
import flash.display.BitmapData;
import flash.display.DisplayObject;
import flash.display.MovieClip;
import flash.display.Sprite;
import flash.display.StageAlign;
import flash.display.StageScaleMode;
import flash.events.Event;
import flash.events.IOErrorEvent;
import flash.events.KeyboardEvent;
import flash.events.ProgressEvent;
import flash.geom.Point;
import flash.geom.Rectangle;
import flash.utils.getDefinitionByName;
import flash.ui.Keyboard;

public class Preloader extends PlatformBase {

    [Embed(source = "../res/loader.png")]
    public static const mcBmpBack:Class;

    [Embed(source = "../res/blocks.png")]
    public static const mcBmpBlocks:Class;

    public function Preloader() {
        if (stage) {
            stage.scaleMode = StageScaleMode.NO_SCALE;
            stage.align = StageAlign.TOP_LEFT;
        }

        // Registering events
        addEventListener(Event.ENTER_FRAME, onEnterFrame);
        stage.addEventListener(KeyboardEvent.KEY_DOWN, readInput);
        loaderInfo.addEventListener(ProgressEvent.PROGRESS, onProgress);
        loaderInfo.addEventListener(IOErrorEvent.IO_ERROR, onIOError);

        // Load background and add it to scene
        var className:Class = mcBmpBack;
        var bmpData:BitmapData = new className().bitmapData as BitmapData;
        mBackground = new Bitmap(bmpData);
        addChild(mBackground);

        // Create canvas for drawing tiles
        mCanvas = new Sprite();
        mBmpCanvas = new BitmapData(SCREEN_WIDTH, SCREEN_HEIGHT, true, 0);
        mCanvas.addChild(new Bitmap(mBmpCanvas));
        addChild(mCanvas);

        // Load tile images
        className = Preloader.mcBmpBlocks;
        mBmpBlocks = new className().bitmapData as BitmapData;

        // Set progress bar position and size.
        mBarWidth = 0.8 * stage.stageWidth;
        mBarX = 0.1 * stage.stageWidth;
        mBarY = stage.stageHeight - BAR_HEIGHT + 2;

        // Start "loader" game
        mGame = new Game(this);
        mGame.startGame();
        mGame.delay = 100;
    }

    private function onIOError(e:IOErrorEvent):void {
        trace(e.text);
    }

    private function onProgress(event:ProgressEvent):void {
        mLoadingPercent = event.bytesLoaded / event.bytesTotal;
        drawProgressBar(mLoadingPercent);
    }

    private function onEnterFrame(event:Event):void {
        if (currentFrame == totalFrames) {
            stop()
            loadingFinished();
        }
        else {
            mGame.update();
        }
    }

    private function loadingFinished():void {
        // Remove linsteners.
        removeEventListener(Event.ENTER_FRAME, onEnterFrame);
        stage.removeEventListener(KeyboardEvent.KEY_DOWN, readInput);
        loaderInfo.removeEventListener(ProgressEvent.PROGRESS, onProgress);
        loaderInfo.removeEventListener(IOErrorEvent.IO_ERROR, onIOError);

        mBmpBlocks = null;
        mBmpCanvas = null;
        removeChild(mCanvas);
        removeChild(mBackground);
        mGame = null;

        // Start "real" game.
        // Note that we are not making direct reference to the Platform class.
        var mainClass:Class = getDefinitionByName("stc.Platform") as Class;
        addChild(new mainClass() as DisplayObject);
    }

    private function drawProgressBar(percent:Number):void {
        mCanvas.graphics.clear();
        mCanvas.graphics.lineStyle(1, BORDER_COLOR);
        mCanvas.graphics.drawRect(mBarX, mBarY, mBarWidth, BAR_HEIGHT);
        mCanvas.graphics.beginFill(BAR_COLOR);
        mCanvas.graphics.drawRect(mBarX + 2, mBarY + 2, percent * (mBarWidth - 4), BAR_HEIGHT - 4);
        mCanvas.graphics.endFill();
    }

    // Draw a tile from a tetromino
    private function drawTile(x:int, y:int, tile:int, shadow:int = 0):void {
        var recSource:Rectangle = new Rectangle();
        recSource.x = TILE_SIZE * tile;
        recSource.y = (TILE_SIZE + 1) * shadow;
        recSource.width = TILE_SIZE + 1;
        recSource.height = TILE_SIZE + 1;
        mBmpCanvas.copyPixels(mBmpBlocks, recSource, new Point(x, y));
    }

    // Called when a tetromino lands over the board.
    override public function onTetrominoLand():void {
        checkHeight(mLoadingPercent);
    }

    // Check board height and delete rows if required for "loader" effect.
    private function checkHeight(percent:Number):void {
        var limit:int = (Game.BOARD_HEIGHT  - 3) * percent;

        var i:int, j:int;
        var height:int = 0;
        for (i = 0; i < Game.BOARD_WIDTH; ++i) {
            var ch:int = 0
            for (j = 0; j < Game.BOARD_HEIGHT; ++j) {
                if (mGame.map[i][j] != Game.EMPTY_CELL) {
                    ch = (Game.BOARD_HEIGHT - j);
                    break;
                }
            }
            if (ch > height) {
                height = ch;
            }
        }
        if (height > limit) {
            var dy:int = height - limit;
            for (i = 0; i < Game.BOARD_WIDTH; ++i) {
                for (j = Game.BOARD_HEIGHT - dy - 1; j >= 0; --j) {
                    mGame.map[i][j + dy] = mGame.map[i][j];
                }
                for (j = 0; j < dy; ++j) {
                    mGame.map[i][j] = Game.EMPTY_CELL;
                }
            }
        }
    }

    // Render the state of the game using platform functions
    override public function renderGame():void {
        // Don't draw if it's not necessary
        if (mGame.stateChanged) {
            var i:int, j:int;

            // Clear background
            mBmpCanvas.fillRect(new Rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT), 0);

            // Draw shadow tetromino
            if (mGame.showShadow && mGame.shadowGap > 0) {
                for (i = 0; i<4; ++i) {
                    for (j = 0; j < 4; ++j) {
                        if (mGame.fallingBlock.cells[i][j] != Game.EMPTY_CELL) {
                            drawTile(BOARD_X + (TILE_SIZE * (mGame.fallingBlock.x + i)),
                                    BOARD_Y + (TILE_SIZE * (mGame.fallingBlock.y + mGame.shadowGap + j)),
                                    mGame.fallingBlock.cells[i][j], 1);
                        }
                    }
                }
            }
            // Draw the cells in the board
            for (i = 0; i < Game.BOARD_WIDTH; ++i) {
                for (j = 0; j < Game.BOARD_HEIGHT; ++j) {
                    if (mGame.map[i][j] != Game.EMPTY_CELL) {
                        drawTile(BOARD_X + (TILE_SIZE * i),
                                BOARD_Y + (TILE_SIZE * j), mGame.map[i][j]);
                    }
                }
            }
            // Draw falling tetromino
            for (i = 0; i<4; ++i) {
                for (j = 0; j < 4; ++j) {
                    if (mGame.fallingBlock.cells[i][j] != Game.EMPTY_CELL) {
                        drawTile(BOARD_X + (TILE_SIZE * (mGame.fallingBlock.x + i)),
                                BOARD_Y + (TILE_SIZE * (mGame.fallingBlock.y + j)),
                                mGame.fallingBlock.cells[i][j]);
                    }
                }
            }
            mGame.stateChanged = false;
        }
    }

    // Read input device and notify game
    public function readInput(event:KeyboardEvent):void {
        // On key pressed
        switch (event.keyCode) {
        case KEY_S:
        case Keyboard.DOWN:
            mGame.events |= Game.EVENT_MOVE_DOWN;
            break;
        case KEY_W:
        case Keyboard.UP:
            mGame.events |= Game.EVENT_ROTATE_CW;
            break;
        case KEY_A:
        case Keyboard.LEFT:
            mGame.events |= Game.EVENT_MOVE_LEFT;
            break;
        case KEY_D:
        case Keyboard.RIGHT:
            mGame.events |= Game.EVENT_MOVE_RIGHT;
            break;
        case Keyboard.SPACE:
            mGame.events |= Game.EVENT_DROP;
            break;
        }
    }

    // Progress bar configuration
    private const BAR_HEIGHT:int = 6;
    private const BAR_COLOR:Number = 0xFFAAAAAA;
    private const BORDER_COLOR:Number = 0xFF000000;

    private var mBarWidth:int;
    private var mBarX:int;
    private var mBarY:int;

    // Graphic elements
    private var mCanvas:Sprite;
    private var mBackground:Bitmap;
    private var mBmpBlocks:BitmapData;
    private var mBmpCanvas:BitmapData;

    private var mLoadingPercent:Number = 0;
}
}
