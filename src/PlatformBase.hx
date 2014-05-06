/* ========================================================================== */
/*   Platform.as                                                              */
/*   Platform interface.                                                      */
/*   Copyright (c) 2014 Laurens Rodriguez Oscanoa.                            */
/* -------------------------------------------------------------------------- */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

import flash.display.Sprite;
import flash.Lib.getTimer;

class PlatformBase extends Sprite
{
    // The game object.
    private var mGame:Game;

    // Called when a row is filled.
    public function onFilledRows():Void { }

    // Called when a tetromino lands over the board.
    public function onTetrominoLand():Void { }

    // Return the current system time in milliseconds
    public function getSystemTime():Float
    {
        return getTimer();
    }

    // Called when game is finished/restarted.
    public function onGameOver(isOver:Bool):Void { }

    // Called when game is paused/resumed.
    public function onGamePaused(isPaused:Bool):Void { }

    // Render the state of the game using platform functions
    public function renderGame():Void { }
}
