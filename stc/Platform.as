/* ========================================================================== */
/*   Platform.as                                                              */
/*   This class contains Flash/Flex especific code.                           */
/*   Copyright (c) 2012 Laurens Rodriguez Oscanoa.                            */
/* -------------------------------------------------------------------------- */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

package stc {

import flash.events.Event;
import flash.events.KeyboardEvent;
import flash.events.MouseEvent;
import flash.display.Bitmap;
import flash.display.BitmapData;
import flash.display.MovieClip;
import flash.display.Sprite;
import flash.geom.Rectangle;
import flash.geom.Point;
import flash.media.Sound;
import flash.media.SoundChannel;
import flash.media.SoundTransform;
import flash.text.TextField;
import flash.text.Font;
import flash.text.TextFieldAutoSize;
import flash.text.TextFormat;
import flash.ui.Keyboard;

// Below is the "magic" line that allow us to load our resources in the second frame.
// http://blogs.adobe.com/rgonzalez/2006/06/modular_applications_part_2.html
[Frame(factoryClass="stc.Preloader")]

// Flash platform implementation for tetris game
public class Platform extends PlatformBase {
    // -------------------------------------------------------------------------
    // UI layout (quantities are expressed in pixels)
    // -------------------------------------------------------------------------

    // Preview tetromino position
    private static const PREVIEW_X:int = 112;
    private static const PREVIEW_Y:int = 210;

    // Score position and length on screen
    private static const SCORE_X:int = 72;
    private static const SCORE_Y:int = 52;
    private static const SCORE_LENGTH:int = 10;

    // Lines position and length on screen
    private static const LINES_X:int = 108;
    private static const LINES_Y:int = 34;
    private static const LINES_LENGTH:int = 5;

    // Level position and length on screen
    private static const LEVEL_X:int = 108;
    private static const LEVEL_Y:int = 16;
    private static const LEVEL_LENGTH:int = 5;

    // Tetromino subtotals position
    private static const TETROMINO_X:int = 425;
    private static const TETROMINO_L_Y:int = 53;
    private static const TETROMINO_I_Y:int = 77;
    private static const TETROMINO_T_Y:int = 101;
    private static const TETROMINO_S_Y:int = 125;
    private static const TETROMINO_Z_Y:int = 149;
    private static const TETROMINO_O_Y:int = 173;
    private static const TETROMINO_J_Y:int = 197;
    private static const TETROMINO_LENGTH:int = 5;

    // Tetromino total position
    private static const PIECES_X:int = 418;
    private static const PIECES_Y:int = 221;
    private static const PIECES_LENGTH:int = 6;

    // Size of number
    private static const NUMBER_WIDTH:int = 7;
    private static const NUMBER_HEIGHT:int = 9;

    // Symbol names
    private static const BMP_BACK:String = "mcBmpBack";
    private static const BMP_TILE_BLOCKS:String = "mcBmpBlocks";
    private static const FLA_POPUP_PAUSE:String = "mcPopUpPaused";
    private static const FLA_POPUP_OVER:String = "mcPopUpOver";

    // Symbol names
    private static const MUSIC_VOLUME:Number = 0.2;
    private static const MUSIC_LOOP_START:int = 3693;

    // Platform data
    private var mPopUp:Sprite;
    private var mPopUpLabel:TextField;
    private var mPopUpCredits:TextField;
    private var mBmpCanvas:BitmapData;
    private var mBmpTextCanvas:BitmapData;
    private var mBmpBlocks:BitmapData;
    private var mBmpNumbers:BitmapData;

    private var mMusicSound:Sound;
    private var mRowSound:Sound;
    private var mMusicChannel:SoundChannel;
    private var mMusicPosition:Number;
    private var mIsMuted:Boolean;

    private var mRefreshBoard:Boolean;
    private var mRefreshFrames:int;

    public function Platform() {
        if (stage) {
            init(null);
        }
        else {
            addEventListener(Event.ADDED_TO_STAGE, init);
        }
    }

    // Initializes platform.
    public function init(event:Event):void {
        removeEventListener(Event.ADDED_TO_STAGE, init);
        stage.quality = "HIGH";

        // Platform Setup
        mGame = new Game(this);
        mGame.startGame();

        // Load background and add it to scene
        var className:Class = Assets.mcBmpBack;
        var bmpData:BitmapData = new className().bitmapData as BitmapData;
        addChild(new Bitmap(bmpData));

        // Create canvas for drawing tiles
        mBmpCanvas = new BitmapData(SCREEN_WIDTH, SCREEN_HEIGHT, true, 0);
        addChild(new Bitmap(mBmpCanvas));

        // Create canvas for drawing text info
        mBmpTextCanvas = new BitmapData(SCREEN_WIDTH, SCREEN_HEIGHT, true, 0);
        addChild(new Bitmap(mBmpTextCanvas));

        // Load tile images
        className = Preloader.mcBmpBlocks;
        mBmpBlocks = new className().bitmapData as BitmapData;

        // Load number images
        className = Assets.mcBmpNumbers;
        mBmpNumbers = new className().bitmapData as BitmapData;

        // Create popup
        mPopUp = new MovieClip();
        var popupBack:Sprite = new Sprite;
        popupBack.graphics.beginFill(0x000000);
        popupBack.graphics.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        popupBack.graphics.endFill();
        popupBack.alpha = 0.6;
        mPopUp.addChild(popupBack);

        var textFormat:TextFormat = new TextFormat();
        textFormat.font = "ConsoleEx";
        textFormat.size = 28;
        textFormat.letterSpacing = 3;
        textFormat.color = 0xBBBBBB;

        mPopUpLabel = new TextField();
        mPopUpLabel.embedFonts = true;
        mPopUpLabel.selectable = false;
        mPopUpLabel.autoSize = TextFieldAutoSize.CENTER;
        mPopUpLabel.defaultTextFormat = textFormat;
        mPopUpLabel.x = SCREEN_WIDTH / 2;
        mPopUpLabel.y = SCREEN_HEIGHT / 2 - 20;
        mPopUp.addChild(mPopUpLabel);

        var creditFormat:TextFormat = new TextFormat();
        creditFormat.font = "ConsoleEx";
        creditFormat.size = 13;
        creditFormat.letterSpacing = 1.5;
        creditFormat.color = 0xFFFFFF;

        mPopUpCredits = new TextField();
        mPopUpCredits.embedFonts = true;
        mPopUpCredits.selectable = false;
        mPopUpCredits.autoSize = TextFieldAutoSize.CENTER;
        mPopUpCredits.defaultTextFormat = creditFormat;
        mPopUpCredits.text = "Programming: Laurens Rodriguez\n"
                            +"      Music: Jarno Alanko";
        mPopUpCredits.x = SCREEN_WIDTH / 2 - 140;
        mPopUpCredits.y = SCREEN_HEIGHT / 2 + 65;
        mPopUp.addChild(mPopUpCredits);

        mPopUp.visible = false;
        addChild(mPopUp);

        // Registering events
        stage.addEventListener(Event.ENTER_FRAME, onEnterFrame, false, 0, true);
        stage.addEventListener(KeyboardEvent.KEY_DOWN, readInput, false, 0, true);
        stage.addEventListener(MouseEvent.CLICK, onMouseClick, false, 0, true);

        // Play music background
        className = Assets.musicGame;
        mMusicSound = new className() as Sound;
        mMusicChannel = mMusicSound.play(0, 0, new SoundTransform(MUSIC_VOLUME));
        mMusicChannel.addEventListener(Event.SOUND_COMPLETE, onSoundComplete, false, 0, true);
        mMusicPosition = 0;
        mIsMuted = false;

        // Load sound effects
        className = Assets.soundRow;
        mRowSound = new className() as Sound;

        mRefreshBoard = false;
        mRefreshFrames = 0;
    }

    // Called if it's necessary to redraw the board.
    override public function onTetrominoLand():void {
        mRefreshBoard = true;
    }

    // Called when a row is filled.
    override public function onFilledRows():void {
        mRowSound.play(0, 0, new SoundTransform(MUSIC_VOLUME));
    }

    // Makes the background music to loop in section
    public function onSoundComplete(event:Event):void {
        if (mMusicChannel) {
            mMusicChannel.removeEventListener(Event.SOUND_COMPLETE, onSoundComplete);
            mMusicChannel = mMusicSound.play(MUSIC_LOOP_START, 0, new SoundTransform(MUSIC_VOLUME));
            mMusicChannel.addEventListener(Event.SOUND_COMPLETE, onSoundComplete, false, 0, true);
        }
    }

    // Called every frame
    public function onEnterFrame(event:Event):void {
        mGame.update();
    }

    // Called on mouse click
    public function onMouseClick(event:MouseEvent):void {
        if (mGame.isPaused) {
            mGame.events |= Game.EVENT_PAUSE;
        }
        else if (mGame.isOver) {
            mGame.events |= Game.EVENT_RESTART;
        }
    }

    // Read input device and notify game
    public function readInput(event:KeyboardEvent):void {
        // On key pressed
        switch (event.keyCode) {
        // On quit game
        case Keyboard.ESCAPE:
            mGame.isOver = true;
            onGameOver(mGame.isOver);
            break;
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
        case Keyboard.F5:
            if (!mIsMuted && mGame.isOver) {
                // Restart music if the game was over.
                if (mMusicChannel) {
                    mMusicChannel.stop();
                    mMusicChannel.removeEventListener(Event.SOUND_COMPLETE, onSoundComplete);
                }
                mMusicChannel = mMusicSound.play(0, 0, new SoundTransform(MUSIC_VOLUME));
                mMusicChannel.addEventListener(Event.SOUND_COMPLETE, onSoundComplete, false, 0, true);
            }

            mGame.events |= Game.EVENT_RESTART;
            break;
        case Keyboard.F1:
            mGame.events |= Game.EVENT_PAUSE;
            break;
        case Keyboard.F2:
            mGame.events |= Game.EVENT_SHOW_NEXT;
            break;
        case Keyboard.F3:
            mGame.events |= Game.EVENT_SHOW_SHADOW;
            break;
        case Keyboard.F4:
            if (!mGame.isOver && !mGame.isPaused) {
                mIsMuted = !mIsMuted;

                if (mMusicChannel) {
                    mMusicPosition = mMusicChannel.position;
                    mMusicChannel.stop();
                    mMusicChannel.removeEventListener(Event.SOUND_COMPLETE, onSoundComplete);
                    mMusicChannel = null;
                }
                else {
                    mMusicChannel = mMusicSound.play(mMusicPosition, 0, new SoundTransform(MUSIC_VOLUME));
                    mMusicChannel.addEventListener(Event.SOUND_COMPLETE, onSoundComplete, false, 0, true);
                }
            }
            break;
        case Keyboard.ENTER:
        case Keyboard.NUMPAD_ENTER:
            if (!mGame.isOver) {
                mGame.masterMode = !mGame.masterMode;
            }
            break;
        }
    }

    // Called when game is finished/restarted.
    override public function onGameOver(isOver:Boolean):void {
        if (isOver) {
            if (mMusicChannel) {
                mMusicChannel.stop();
                mMusicChannel.removeEventListener(Event.SOUND_COMPLETE, onSoundComplete);
                mMusicChannel = null;
            }
            mPopUpLabel.text = "GAME OVER";
        }
        mPopUp.visible = isOver;
        mGame.masterMode = false;
    }

    // Called when game is paused/resumed.
    override public function onGamePaused(isPaused:Boolean):void {
        if (isPaused) {
            if (mMusicChannel) {
                mMusicPosition = mMusicChannel.position;
                mMusicChannel.stop();
                mMusicChannel.removeEventListener(Event.SOUND_COMPLETE, onSoundComplete);
                mMusicChannel = null;
            }
            mPopUpLabel.text = "GAME PAUSED";
        }
        else if (!mIsMuted) {
            mMusicChannel = mMusicSound.play(mMusicPosition, 0, new SoundTransform(MUSIC_VOLUME));
            mMusicChannel.addEventListener(Event.SOUND_COMPLETE, onSoundComplete, false, 0, true);
        }
        mPopUp.visible = isPaused;
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

    // Draw a number on the given position
    private function drawNumber(x:int, y:int, number:int, length:int, color:int):void {
        var recSource:Rectangle = new Rectangle();
        recSource.y = NUMBER_HEIGHT * color;
        recSource.width = NUMBER_WIDTH;
        recSource.height = NUMBER_HEIGHT;

        var pos:int = 0;
        do {
            recSource.x = NUMBER_WIDTH * (number % 10);
            mBmpTextCanvas.copyPixels(mBmpNumbers, recSource, new Point(x + NUMBER_WIDTH * (length - pos), y));
            number /= 10;
        } while (++pos < length);
    }

    // Render the state of the game using platform functions
    override public function renderGame():void {
        if (mRefreshFrames > 0) {
            if (--mRefreshFrames == 0) {
                mGame.stateChanged = true;
            }
        }

        // Don't draw if it's not necessary
        if (mGame.stateChanged) {
            var i:int, j:int;

            // Clear background
            mBmpCanvas.fillRect(new Rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT), 0);

            // Draw preview block
            if (mGame.showPreview) {
                for (i = 0; i < 4; ++i) {
                    for (j = 0; j < 4; ++j) {
                        if (mGame.nextBlock.cells[i][j] != Game.EMPTY_CELL) {
                            drawTile(PREVIEW_X + (TILE_SIZE * i),
                                    PREVIEW_Y + (TILE_SIZE * j), mGame.nextBlock.cells[i][j]);
                        }
                    }
                }
            }
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
            if (mGame.masterMode) {
                if (mRefreshBoard) {
                    for (i = 0; i < Game.BOARD_WIDTH; ++i) {
                        for (j = 0; j < Game.BOARD_HEIGHT; ++j) {
                            if (mGame.map[i][j] != Game.EMPTY_CELL) {
                                drawTile(BOARD_X + (TILE_SIZE * i),
                                        BOARD_Y + (TILE_SIZE * j), mGame.map[i][j]);
                            }
                        }
                    }
                    mRefreshBoard = false;
                    mRefreshFrames = 10;
                }
            }
            else {
                for (i = 0; i < Game.BOARD_WIDTH; ++i) {
                    for (j = 0; j < Game.BOARD_HEIGHT; ++j) {
                        if (mGame.map[i][j] != Game.EMPTY_CELL) {
                            drawTile(BOARD_X + (TILE_SIZE * i),
                                    BOARD_Y + (TILE_SIZE * j), mGame.map[i][j]);
                        }
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
        // Update game statistic data
        if (mGame.scoreChanged) {

            drawNumber(LEVEL_X, LEVEL_Y, mGame.stats.level, LEVEL_LENGTH, Game.COLOR_WHITE);
            drawNumber(LINES_X, LINES_Y, mGame.stats.lines, LINES_LENGTH, Game.COLOR_WHITE);
            drawNumber(SCORE_X, SCORE_Y, mGame.stats.score, SCORE_LENGTH, Game.COLOR_WHITE);

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
}
}
