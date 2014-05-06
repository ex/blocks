/* ========================================================================== */
/*   Platform.hx                                                              */
/*   This class contains NME especific code.                                  */
/*   Copyright (c) 2014 Laurens Rodriguez Oscanoa.                            */
/* -------------------------------------------------------------------------- */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

import flash.events.Event;
import flash.events.KeyboardEvent;
import flash.events.MouseEvent;
import flash.display.Bitmap;
import flash.display.BitmapData;
import flash.display.Sprite;
import flash.display.StageAlign;
import flash.display.StageScaleMode;
import flash.geom.Rectangle;
import flash.geom.Point;
#if android
import flash.Lib;
#end
import flash.Lib.current;
import flash.media.Sound;
import flash.media.SoundChannel;
import flash.media.SoundTransform;
import flash.text.TextField;
import flash.text.Font;
import flash.text.TextFormatAlign;
import flash.text.TextFormat;
import flash.text.TextFieldType;
import flash.ui.Keyboard;
import openfl.Assets;
import openfl.display.Tilesheet;

// NME implementation for tetris game
class Platform extends PlatformBase
{
    // -------------------------------------------------------------------------
    // UI layout (quantities are expressed in pixels)
    // -------------------------------------------------------------------------

    // Size of square tile
    private static inline var TILE_SIZE:Int = 12;

    private static inline var SCREEN_WIDTH:Int = 480;
    private static inline var SCREEN_HEIGHT:Int = 320;
    private static inline var BACKGROUND_HEIGHT:Int = 272;

    // Board up-left corner coordinates
    private static inline var BOARD_X:Int = 180;
    private static inline var BOARD_Y:Int = 4;

    // Keyboard codes
    private static inline var KEY_A:Int = 65;
    private static inline var KEY_W:Int = 87;
    private static inline var KEY_S:Int = 83;
    private static inline var KEY_D:Int = 68;

    // Preview tetromino position
    private static inline var PREVIEW_X:Int = 112;
    private static inline var PREVIEW_Y:Int = 210;

    // Score position and length on screen
    private static inline var SCORE_X:Int = 72;
    private static inline var SCORE_Y:Int = 64;
    private static inline var SCORE_LENGTH:Int = 10;

    // Lines position and length on screen
    private static inline var LINES_X:Int = 108;
    private static inline var LINES_Y:Int = 46;
    private static inline var LINES_LENGTH:Int = 5;

    // Level position and length on screen
    private static inline var LEVEL_X:Int = 108;
    private static inline var LEVEL_Y:Int = 28;
    private static inline var LEVEL_LENGTH:Int = 5;

    // Tetromino subtotals position
    private static inline var TETROMINO_X:Int = 425;
    private static inline var TETROMINO_L_Y:Int = 53;
    private static inline var TETROMINO_I_Y:Int = 77;
    private static inline var TETROMINO_T_Y:Int = 101;
    private static inline var TETROMINO_S_Y:Int = 125;
    private static inline var TETROMINO_Z_Y:Int = 149;
    private static inline var TETROMINO_O_Y:Int = 173;
    private static inline var TETROMINO_J_Y:Int = 197;
    private static inline var TETROMINO_LENGTH:Int = 5;

    // Tetromino total position
    private static inline var PIECES_X:Int = 418;
    private static inline var PIECES_Y:Int = 221;
    private static inline var PIECES_LENGTH:Int = 6;

    // Size of number
    private static inline var NUMBER_WIDTH:Int = 7;
    private static inline var NUMBER_HEIGHT:Int = 9;

    // Symbol names
    private static inline var BMP_BACK:String = "mcBmpBack";
    private static inline var BMP_TILE_BLOCKS:String = "mcBmpBlocks";
    private static inline var FLA_POPUP_PAUSE:String = "mcPopUpPaused";
    private static inline var FLA_POPUP_OVER:String = "mcPopUpOver";

    // Symbol names
    private static inline var MUSIC_VOLUME:Float = 0.4;
    private static inline var MUSIC_LOOP_START:Int = 3693;

    private static inline var STR_PAUSE:String = "GAME PAUSED";
    private static inline var STR_END:String = "GAME OVER";
    private static inline var STR_CREDITS:String = "Programming: Laurens Rodriguez\n" +
                                                   "      Music: Jarno Alanko";
    // Platform data
    private var mPopUp:Sprite;
    private var mPopUpLabel:TextField;
    private var mPopUpCredits:TextField;
    private var mBmpCanvas:BitmapData;
    private var mBmpTextCanvas:BitmapData;
    private var mCanvasBlocks:Sprite;
    private var mCanvasNumbers:Sprite;
    private var mTilesBlocks:Tilesheet;
    private var mTilesNumbers:Tilesheet;

    private var mMusicSound:Sound;
    private var mRowSound:Sound;
    private var mMusicChannel:SoundChannel;
    private var mMusicPosition:Float;
    private var mIsMuted:Bool;

    private var mRefreshBoard:Bool;
    private var mRefreshFrames:Int;

    private var mYOffset:Int;

    private var mPadLeft:Sprite;
    private var mPadRight:Sprite;
    private var mPadDown:Sprite;
    private var mPadRotate:Sprite;
    private var mPadDrop:Sprite;
    private var mPadPause:Sprite;
    private var mPadRestart:Sprite;
    private var mPadShadow:Sprite;
    private var mPadNext:Sprite;
    private var mPadMute:Sprite;
    private var mPadMaster:Sprite;

    //--------------------------------------------------------------------------
    public function new()
    {
        super();
        init();
#if (android || ios || blackberry)
        resize(null);
#end
    }

    // Initializes platform.
    public function init():Void
    {
		current.stage.align = StageAlign.TOP_LEFT;
		current.stage.scaleMode = StageScaleMode.NO_SCALE;

        // Create game
        mGame = new Game(this);
        mGame.startGame();

        // Calculate offset for drawing.
        mYOffset = Std.int((SCREEN_HEIGHT - BACKGROUND_HEIGHT) / 2);

        // Load background and add it to scene
        addChild(new Bitmap(Assets.getBitmapData("assets/images/back.png", false)));

        // Create canvas for drawing tiles
        mBmpCanvas = new BitmapData(SCREEN_WIDTH, SCREEN_HEIGHT, true, 0);
        addChild(new Bitmap(mBmpCanvas));

        // Create canvas for drawing text info
        mBmpTextCanvas = new BitmapData(SCREEN_WIDTH, SCREEN_HEIGHT, true, 0);
        addChild(new Bitmap(mBmpTextCanvas));

        // Load tile images.
        // The tiles are stored in 2 rows of 8 cells, second row are the shadow cells.
        mTilesBlocks = new Tilesheet(Assets.getBitmapData("assets/images/blocks.png", false));
        for (k in  0 ... 16)
        {
            var shadow:Int = (k < 8)? 0 : 1;
            var recSource:Rectangle = new Rectangle();
            recSource.x = TILE_SIZE * (k % 8);
            recSource.y = (TILE_SIZE + 1) * shadow;
            recSource.width = TILE_SIZE + 1;
            recSource.height = TILE_SIZE + 1 - shadow;
            mTilesBlocks.addTileRect(recSource);
        }
        mCanvasBlocks = new Sprite();
        addChild(mCanvasBlocks);

        // Load number images.
        // The tiles are stored in 8 rows of 10 cells.
        mTilesNumbers = new Tilesheet(Assets.getBitmapData("assets/images/numbers.png", false));
        for (k in  0 ... 80)
        {
            var row:Int = Std.int(k / 10);
            var recSource:Rectangle = new Rectangle();
            recSource.x = NUMBER_WIDTH * (k % 10);
            recSource.y = NUMBER_HEIGHT * row;
            recSource.width = NUMBER_WIDTH;
            recSource.height = NUMBER_HEIGHT;
            mTilesNumbers.addTileRect(recSource);
        }
        mCanvasNumbers = new Sprite();
        addChild(mCanvasNumbers);

        // Create popup
        var width:Int = current.stage.stageWidth;
        var height:Int = current.stage.stageHeight;
        trace("width: " + width + " height:" +height);

        mPopUp = new Sprite();
        var popupBack:Sprite = new Sprite();
        popupBack.graphics.beginFill(0x111133);
        popupBack.graphics.drawRect(-50, -50, SCREEN_WIDTH + 100, SCREEN_HEIGHT + 100);
        popupBack.graphics.endFill();
        popupBack.alpha = 0.6;
        mPopUp.addChild(popupBack);

        var font:Font = Assets.getFont("assets/fonts/luconex.ttf");

        var textFormat:TextFormat = new TextFormat();
        textFormat.font = font.fontName;
        textFormat.size = 28;
        textFormat.letterSpacing = 3;
        textFormat.color = 0xBBBBBB;
        textFormat.align = TextFormatAlign.LEFT;

        mPopUpLabel = new TextField();
        mPopUpLabel.embedFonts = true;
        mPopUpLabel.selectable = false;
        mPopUpLabel.defaultTextFormat = textFormat;
        mPopUpLabel.width = SCREEN_WIDTH;
        mPopUpLabel.y = SCREEN_HEIGHT / 2 - 20;
        mPopUp.addChild(mPopUpLabel);

        var creditFormat:TextFormat = new TextFormat();
        creditFormat.font = font.fontName;
        creditFormat.size = 13;
        creditFormat.letterSpacing = 1;
        creditFormat.color = 0xFFFFFF;
        creditFormat.align = TextFormatAlign.LEFT;

        mPopUpCredits = new TextField();
        mPopUpCredits.embedFonts = true;
        mPopUpCredits.selectable = false;
        mPopUpCredits.defaultTextFormat = creditFormat;
        mPopUpCredits.width = SCREEN_WIDTH;
        mPopUpCredits.text = STR_CREDITS;
        mPopUpCredits.x = SCREEN_WIDTH / 2 - mPopUpCredits.textWidth / 2;
        mPopUpCredits.y = SCREEN_HEIGHT / 2 + 65;
        mPopUp.addChild(mPopUpCredits);
        mPopUp.visible = false;
        addChild(mPopUp);

        // Registering events
        current.stage.addEventListener(Event.ENTER_FRAME, onEnterFrame);
        current.stage.addEventListener(KeyboardEvent.KEY_DOWN, onKeyDown);
        current.stage.addEventListener(KeyboardEvent.KEY_UP, onEvent);
        current.stage.addEventListener(MouseEvent.MOUSE_UP, onEvent);
        current.stage.addEventListener(Event.RESIZE, resize);

        // Play music background
        mMusicSound = Assets.getSound("stc_theme_loop");
        mMusicChannel = mMusicSound.play(MUSIC_LOOP_START, 0, new SoundTransform(MUSIC_VOLUME));
        mMusicChannel.addEventListener(Event.SOUND_COMPLETE, onSoundComplete);
        mMusicPosition = 0;
        mIsMuted = false;

        // Load sound effects
        mRowSound = Assets.getSound("fx_line");

        mRefreshBoard = false;
        mRefreshFrames = 0;

        // Add control pads
#if debug
        var alphaPad:Float = 0.10;
#else
        var alphaPad:Float = 0;
#end

        mPadLeft = new Sprite();
        drawBox(mPadLeft, 0, 0, 200, 160, 0xFF0000, alphaPad);
        mPadLeft.x = -50;
        mPadLeft.y = 80;
        addChild(mPadLeft);
        mPadLeft.addEventListener(MouseEvent.MOUSE_DOWN, onMouseDownPads);

        mPadRight = new Sprite();
        drawBox(mPadRight, 0, 0, 200, 160, 0xFF0000, alphaPad);
        mPadRight.x = 330;
        mPadRight.y = 80;
        addChild(mPadRight);
        mPadRight.addEventListener(MouseEvent.MOUSE_DOWN, onMouseDownPads);

        mPadRotate = new Sprite();
        drawBox(mPadRotate, 0, 0, 170, 100, 0xFF0000, alphaPad);
        mPadRotate.x = 155;
        mPadRotate.y = 0;
        addChild(mPadRotate);
        mPadRotate.addEventListener(MouseEvent.MOUSE_DOWN, onMouseDownPads);

        mPadDrop = new Sprite();
        drawBox(mPadDrop, 0, 0, 170, 100, 0xFF0000, alphaPad);
        mPadDrop.x = 155;
        mPadDrop.y = 220;
        addChild(mPadDrop);
        mPadDrop.addEventListener(MouseEvent.MOUSE_DOWN, onMouseDownPads);

        mPadDown = new Sprite();
        drawBox(mPadDown, 0, 0, 170, 110, 0x00FF00, alphaPad);
        mPadDown.x = 155;
        mPadDown.y = 105;
        addChild(mPadDown);
        mPadDown.addEventListener(MouseEvent.MOUSE_DOWN, onMouseDownPads);

        mPadNext = new Sprite();
        drawBox(mPadNext, 0, 0, 75, 75, 0xFFFF00, alphaPad);
        mPadNext.x = 75;
        mPadNext.y = 0;
        addChild(mPadNext);
        mPadNext.addEventListener(MouseEvent.MOUSE_DOWN, onMouseDownPads);

        mPadMute = new Sprite();
        drawBox(mPadMute, 0, 0, 75, 75, 0x00FF00, alphaPad);
        mPadMute.x = 0;
        mPadMute.y = 0;
        addChild(mPadMute);
        mPadMute.addEventListener(MouseEvent.MOUSE_DOWN, onMouseDownPads);

        mPadRestart = new Sprite();
        drawBox(mPadRestart, 0, 0, 150, 75, 0xFFFF00, alphaPad);
        mPadRestart.x = 0;
        mPadRestart.y = 245;
        addChild(mPadRestart);
        mPadRestart.addEventListener(MouseEvent.MOUSE_DOWN, onMouseDownPads);

        mPadShadow = new Sprite();
        drawBox(mPadShadow, 0, 0, 75, 75, 0xFFFF00, alphaPad);
        mPadShadow.x = 330;
        mPadShadow.y = 0;
        addChild(mPadShadow);
        mPadShadow.addEventListener(MouseEvent.MOUSE_DOWN, onMouseDownPads);

        mPadMaster = new Sprite();
        drawBox(mPadMaster, 0, 0, 75, 75, 0x00FF00, alphaPad);
        mPadMaster.x = 405;
        mPadMaster.y = 0;
        addChild(mPadMaster);
        mPadMaster.addEventListener(MouseEvent.MOUSE_DOWN, onMouseDownPads);

        mPadPause = new Sprite();
        drawBox(mPadPause, 0, 0, 150, 75, 0xFFFF00, alphaPad);
        mPadPause.x = 330;
        mPadPause.y = 245;
        addChild(mPadPause);
        mPadPause.addEventListener(MouseEvent.MOUSE_DOWN, onMouseDownPads);
    }

    public function onMouseDownPads(event:MouseEvent):Void
    {
        if (event.target == mPadLeft)
        {
            mGame.onEventStart(Game.EVENT_MOVE_LEFT);
        }
        else if (event.target == mPadRight)
        {
            mGame.onEventStart(Game.EVENT_MOVE_RIGHT);
        }
        else if (event.target == mPadRotate)
        {
            mGame.onEventStart(Game.EVENT_ROTATE_CW);
        }
        else if (event.target == mPadDrop)
        {
            mGame.onEventStart(Game.EVENT_DROP);
        }
        else if (event.target == mPadDown)
        {
            mGame.onEventStart(Game.EVENT_MOVE_DOWN);
        }
        else if (event.target == mPadNext)
        {
            mGame.onEventStart(Game.EVENT_SHOW_NEXT);
        }
        else if (event.target == mPadRestart)
        {
            if (!mGame.isOver)
            {
                mGame.isOver = true;
                onGameOver(mGame.isOver);
            }
            else
            {
                onRestart();
            }
        }
        else if (event.target == mPadShadow)
        {
            mGame.onEventStart(Game.EVENT_SHOW_SHADOW);
        }
        else if (event.target == mPadPause)
        {
            mGame.onEventStart(Game.EVENT_PAUSE);
        }
        else if (event.target == mPadMute)
        {
            onMute();
        }
        else if (event.target == mPadMaster)
        {
            onMasterMode();
        }
    }

    // Called if it's necessary to redraw the board.
    override public function onTetrominoLand():Void
    {
        mRefreshBoard = true;
    }

    // Called when a row is filled.
    override public function onFilledRows():Void
    {
        mRowSound.play(0, 0, new SoundTransform(MUSIC_VOLUME));
    }

    // Makes the background music to loop in section
    public function onSoundComplete(event:Event):Void
    {
        if (mMusicChannel != null)
        {
            mMusicChannel.removeEventListener(Event.SOUND_COMPLETE, onSoundComplete);
            mMusicChannel = mMusicSound.play(MUSIC_LOOP_START, 0, new SoundTransform(MUSIC_VOLUME));
            mMusicChannel.addEventListener(Event.SOUND_COMPLETE, onSoundComplete);
        }
    }

    // Called every frame
    public function onEnterFrame(event:Event):Void
    {
        mGame.update();
    }

    public function onEvent(event:Event):Void
    {
        // Just cancel any continuos action by now
        mGame.onEventEnd(Game.EVENT_MOVE_LEFT);
        mGame.onEventEnd(Game.EVENT_MOVE_RIGHT);
        mGame.onEventEnd(Game.EVENT_MOVE_DOWN);
        mGame.onEventEnd(Game.EVENT_ROTATE_CW);
    }

    public function onClosePopUp(event:Event):Void
    {
        if (mGame.isPaused)
        {
            mGame.onEventStart(Game.EVENT_PAUSE);
        }
        else if (mGame.isOver)
        {
            mGame.onEventStart(Game.EVENT_RESTART);
        }
    }

    private function onMasterMode():Void
    {
        if (!mGame.isOver)
        {
            mGame.setMasterMode(!mGame.getMasterMode());
        }
    }

    private function onMute():Void
    {
        if (!mGame.isOver && !mGame.isPaused)
        {
            mIsMuted = !mIsMuted;

            if (mMusicChannel != null)
            {
                mMusicPosition = mMusicChannel.position;
                mMusicChannel.stop();
                mMusicChannel.removeEventListener(Event.SOUND_COMPLETE, onSoundComplete);
                mMusicChannel = null;
            }
            else
            {
                mMusicChannel = mMusicSound.play(mMusicPosition, 0, new SoundTransform(MUSIC_VOLUME));
                mMusicChannel.addEventListener(Event.SOUND_COMPLETE, onSoundComplete);
            }
        }
    }

    private function onRestart():Void
    {
        if (!mIsMuted && mGame.isOver)
        {
            // Restart music if the game was over.
            if (mMusicChannel != null)
            {
                mMusicChannel.stop();
                mMusicChannel.removeEventListener(Event.SOUND_COMPLETE, onSoundComplete);
            }
            mMusicChannel = mMusicSound.play(0, 0, new SoundTransform(MUSIC_VOLUME));
            mMusicChannel.addEventListener(Event.SOUND_COMPLETE, onSoundComplete);
        }
        mGame.onEventStart(Game.EVENT_RESTART);
    }

    public function onKeyDown(event:KeyboardEvent):Void
    {
#if android
		if (event.keyCode == 27)
        {
			event.stopImmediatePropagation();
			Lib.exit();
		}
#end
        // On key pressed
        switch (event.keyCode)
        {
            // On quit game
            case Keyboard.ESCAPE:
                mGame.isOver = true;
                onGameOver(mGame.isOver);
            case KEY_S, Keyboard.DOWN:
                mGame.onEventStart(Game.EVENT_MOVE_DOWN);
            case KEY_W, Keyboard.UP:
                mGame.onEventStart(Game.EVENT_ROTATE_CW);
            case KEY_A, Keyboard.LEFT:
                mGame.onEventStart(Game.EVENT_MOVE_LEFT);
            case KEY_D, Keyboard.RIGHT:
                mGame.onEventStart(Game.EVENT_MOVE_RIGHT);
            case Keyboard.SPACE:
                mGame.onEventStart(Game.EVENT_DROP);
            case Keyboard.F5:
                onRestart();
            case Keyboard.F1:
                mGame.onEventStart(Game.EVENT_PAUSE);
            case Keyboard.F2:
                mGame.onEventStart(Game.EVENT_SHOW_NEXT);
            case Keyboard.F3:
                mGame.onEventStart(Game.EVENT_SHOW_SHADOW);
            case Keyboard.F4:
                onMute();
            case Keyboard.ENTER, Keyboard.NUMPAD_ENTER, 10:
                onMasterMode();
        }
    }

    // Called when game is finished/restarted.
    override public function onGameOver(isOver:Bool):Void
    {
        if (isOver)
        {
            if (mMusicChannel != null)
            {
                mMusicChannel.stop();
                mMusicChannel.removeEventListener(Event.SOUND_COMPLETE, onSoundComplete);
                mMusicChannel = null;
            }
            mPopUpLabel.text = STR_END;
            mPopUpLabel.x = SCREEN_WIDTH / 2 - mPopUpLabel.textWidth / 2;

            current.stage.addEventListener(MouseEvent.MOUSE_DOWN, onClosePopUp);
        }
        else
        {
            current.stage.removeEventListener(MouseEvent.MOUSE_DOWN, onClosePopUp);
        }
        mPopUp.visible = isOver;
        mGame.setMasterMode(false);
    }

    // Called when game is paused/resumed.
    override public function onGamePaused(isPaused:Bool):Void
    {
        if (isPaused)
        {
            if (mMusicChannel != null)
            {
                mMusicPosition = mMusicChannel.position;
                mMusicChannel.stop();
                mMusicChannel.removeEventListener(Event.SOUND_COMPLETE, onSoundComplete);
                mMusicChannel = null;
            }
            mPopUpLabel.text = STR_PAUSE;
            mPopUpLabel.x = SCREEN_WIDTH / 2 - mPopUpLabel.textWidth / 2;

            current.stage.addEventListener(MouseEvent.MOUSE_DOWN, onClosePopUp);
        }
        else
        {
            if (!mIsMuted)
            {
                mMusicChannel = mMusicSound.play(mMusicPosition, 0, new SoundTransform(MUSIC_VOLUME));
                mMusicChannel.addEventListener(Event.SOUND_COMPLETE, onSoundComplete);
            }
            current.stage.removeEventListener(MouseEvent.MOUSE_DOWN, onClosePopUp);
        }
        mPopUp.visible = isPaused;
    }

    // Draw a tile from a tetromino
    private function drawTile(x:Int, y:Int, tile:Int, shadow:Int = 0):Void
    {
        var data:Array<Float> = [x, mYOffset + y, tile + shadow * 8];
        mTilesBlocks.drawTiles(mCanvasBlocks.graphics, data, false, 0);
    }

    // Draw a number on the given position
    private function drawNumber(x:Int, y:Int, number:Int, length:Int, color:Int):Void
    {
        var pos:Int = 0;
        do
        {
            var data:Array<Float> = [x + NUMBER_WIDTH * (length - pos), mYOffset + y, (number % 10) + 10 * color];
            mTilesNumbers.drawTiles(mCanvasNumbers.graphics, data, false, 0);

            number = Std.int(number / 10);
        } while (++pos < length);
    }

    // Render the state of the game using platform functions
    override public function renderGame():Void
    {
        if (mRefreshFrames > 0)
        {
            if (--mRefreshFrames == 0)
            {
                mGame.stateChanged = true;
            }
        }

        // Don't draw if it's not necessary
        if (mGame.stateChanged)
        {
            var i:Int, j:Int;

            // Clear background
            mCanvasBlocks.graphics.clear();

            // Draw preview block
            if (mGame.showPreview)
            {
                for (i in 0 ... 4)
                {
                    for (j in 0 ... 4)
                    {
                        if (mGame.nextBlock.cells[i][j] != Game.EMPTY_CELL)
                        {
                            drawTile(PREVIEW_X + (TILE_SIZE * i),
                                    PREVIEW_Y + (TILE_SIZE * j), mGame.nextBlock.cells[i][j]);
                        }
                    }
                }
            }

            // Draw shadow tetromino
            if (mGame.showShadow && mGame.shadowGap > 0)
            {
                for (i in 0 ... 4)
                {
                    for (j in 0 ... 4)
                    {
                        if (mGame.fallingBlock.cells[i][j] != Game.EMPTY_CELL)
                        {
                            drawTile(BOARD_X + (TILE_SIZE * (mGame.fallingBlock.x + i)),
                                    BOARD_Y + (TILE_SIZE * (mGame.fallingBlock.y + mGame.shadowGap + j)),
                                    mGame.fallingBlock.cells[i][j], 1);
                        }
                    }
                }
            }

            // Draw the cells in the board
            if (mGame.getMasterMode())
            {
                if (mRefreshBoard)
                {
                    for (i in 0 ... Game.BOARD_WIDTH)
                    {
                        for (j in 0 ... Game.BOARD_HEIGHT)
                        {
                            if (mGame.map[i][j] != Game.EMPTY_CELL)
                            {
                                drawTile(BOARD_X + (TILE_SIZE * i),
                                        BOARD_Y + (TILE_SIZE * j), mGame.map[i][j]);
                            }
                        }
                    }
                    mRefreshBoard = false;
                    mRefreshFrames = 10;
                }
            }
            else
            {
                for (i in 0 ... Game.BOARD_WIDTH)
                {
                    for (j in 0 ... Game.BOARD_HEIGHT)

                    {
                        if (mGame.map[i][j] != Game.EMPTY_CELL)
                        {
                            drawTile(BOARD_X + (TILE_SIZE * i),
                                    BOARD_Y + (TILE_SIZE * j), mGame.map[i][j]);
                        }
                    }
                }
            }

            // Draw falling tetromino
            for (i in 0 ... 4)
            {
                for (j in 0 ... 4)
                {
                    if (mGame.fallingBlock.cells[i][j] != Game.EMPTY_CELL)
                    {
                        drawTile(BOARD_X + (TILE_SIZE * (mGame.fallingBlock.x + i)),
                                BOARD_Y + (TILE_SIZE * (mGame.fallingBlock.y + j)),
                                mGame.fallingBlock.cells[i][j]);
                    }
                }
            }
            mGame.stateChanged = false;
        }

        // Update game statistic data
        if (mGame.scoreChanged)
        {
            mCanvasNumbers.graphics.clear();

            drawNumber(LEVEL_X, LEVEL_Y, mGame.stats.level, LEVEL_LENGTH, Game.COLOR_WHITE);
            drawNumber(LINES_X, LINES_Y, mGame.stats.lines, LINES_LENGTH, Game.COLOR_WHITE);
            drawNumber(SCORE_X, SCORE_Y, Std.int(mGame.stats.score), SCORE_LENGTH, Game.COLOR_WHITE);

            drawNumber(TETROMINO_X, TETROMINO_L_Y, mGame.stats.pieces[Game.TETROMINO_L], TETROMINO_LENGTH, Game.COLOR_ORANGE);
            drawNumber(TETROMINO_X, TETROMINO_I_Y, mGame.stats.pieces[Game.TETROMINO_I], TETROMINO_LENGTH, Game.COLOR_CYAN);
            drawNumber(TETROMINO_X, TETROMINO_T_Y, mGame.stats.pieces[Game.TETROMINO_T], TETROMINO_LENGTH, Game.COLOR_PURPLE);
            drawNumber(TETROMINO_X, TETROMINO_S_Y, mGame.stats.pieces[Game.TETROMINO_S], TETROMINO_LENGTH, Game.COLOR_GREEN);
            drawNumber(TETROMINO_X, TETROMINO_Z_Y, mGame.stats.pieces[Game.TETROMINO_Z], TETROMINO_LENGTH, Game.COLOR_RED);
            drawNumber(TETROMINO_X, TETROMINO_O_Y, mGame.stats.pieces[Game.TETROMINO_O], TETROMINO_LENGTH, Game.COLOR_YELLOW);
            drawNumber(TETROMINO_X, TETROMINO_J_Y, mGame.stats.pieces[Game.TETROMINO_J], TETROMINO_LENGTH, Game.COLOR_BLUE);

            drawNumber(PIECES_X, PIECES_Y, mGame.stats.totalPieces, PIECES_LENGTH, Game.COLOR_WHITE);
            mGame.scoreChanged = false;
        }
    }

	private function resize(event:Event):Void
    {
        var sx:Float = current.stage.stageWidth / SCREEN_WIDTH;
        var sy:Float = current.stage.stageHeight / SCREEN_HEIGHT;
        if (sx > sy)
        {
            this.scaleX = this.scaleY = sy;
            this.x = (current.stage.stageWidth - sy * SCREEN_WIDTH) / 2;
        }
        else
        {
            this.scaleX = this.scaleY = sx;
        }
	}

    public static function drawBox(canvas:Sprite,
                                   iniX:Int,
                                   iniY:Int,
                                   width:Int,
                                   height:Int,
                                   colorBody:Int,
                                   alphaBody:Float = 1.0,
                                   borderSize:Float = 0,
                                   colorBorder:Int = 0,
                                   borderAlpha:Float = 0):Void
    {
        canvas.graphics.lineStyle(borderSize, colorBorder, borderAlpha);
        canvas.graphics.beginFill(colorBody, alphaBody);
        canvas.graphics.moveTo(iniX, iniY);
        canvas.graphics.lineTo(iniX, iniY + height);
        canvas.graphics.lineTo(iniX + width, iniY + height);
        canvas.graphics.lineTo(iniX + width, iniY);
        canvas.graphics.endFill();
    }

	// Entry point
	public static function main()
    {
#if (flash9 || flash10)
        haxe.Log.trace = function(v,?pos) { untyped __global__["trace"](pos.className+"#"+pos.methodName+"("+pos.lineNumber+"):",v); }
#elseif flash
        haxe.Log.trace = function(v,?pos) { flash.Lib.trace(pos.className+"#"+pos.methodName+"("+pos.lineNumber+"): "+v); }
#end
		current.addChild(new Platform());
	}
}
