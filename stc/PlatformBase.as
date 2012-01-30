/* ========================================================================== */
/*   Platform.as                                                              */
/*   Platform interface.                                                      */
/*   Copyright (c) 2012 Laurens Rodriguez Oscanoa.                            */
/* -------------------------------------------------------------------------- */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

package stc {

import flash.display.MovieClip;
import flash.utils.getTimer;

public class PlatformBase extends MovieClip {
    // -------------------------------------------------------------------------
    // UI layout (quantities are expressed in pixels)
    // -------------------------------------------------------------------------

    // Size of square tile
    protected static const TILE_SIZE:int = 12;

    protected static const SCREEN_WIDTH:int = 480;
    protected static const SCREEN_HEIGHT:int = 272;

    // Board up-left corner coordinates
    protected static const BOARD_X:int = 180;
    protected static const BOARD_Y:int = 4;

    // Keyboard codes
    protected static const KEY_A:int = "A".charCodeAt();
    protected static const KEY_W:int = "W".charCodeAt();
    protected static const KEY_S:int = "S".charCodeAt();
    protected static const KEY_D:int = "D".charCodeAt();

    // The game object.
    protected var mGame:Game;

    // Called when a row is filled.
    public function onFilledRows():void { }

    // Called when a tetromino lands over the board.
    public function onTetrominoLand():void { }

    // Return the current system time in milliseconds
    public function getSystemTime():Number {
        return getTimer();
    }

    // Called when game is finished/restarted.
    public function onGameOver(isOver:Boolean):void { }

    // Called when game is paused/resumed.
    public function onGamePaused(isPaused:Boolean):void { }

    // Render the state of the game using platform functions
    public function renderGame():void { }
}
}
