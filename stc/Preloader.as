/* ========================================================================== */
/*   Preloader.as                                                             */
/*   Copyright (c) 2012 Laurens Rodriguez Oscanoa.                            */
/* -------------------------------------------------------------------------- */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

package stc {

import flash.display.DisplayObject;
import flash.display.MovieClip;
import flash.display.Sprite;
import flash.display.StageAlign;
import flash.display.StageScaleMode;
import flash.events.Event;
import flash.events.IOErrorEvent;
import flash.events.ProgressEvent;
import flash.utils.getDefinitionByName;

public class Preloader extends MovieClip {

    public function Preloader() {
        if (stage) {
            stage.scaleMode = StageScaleMode.NO_SCALE;
            stage.align = StageAlign.TOP_LEFT;
        }
        addEventListener(Event.ENTER_FRAME, onEnterFrame);
        loaderInfo.addEventListener(ProgressEvent.PROGRESS, onProgress);
        loaderInfo.addEventListener(IOErrorEvent.IO_ERROR, onIOError);

        // Set progress bar position and size.
        mWidth = 0.8 * stage.stageWidth;
        mX = 0.1 * stage.stageWidth;
        mY = 0.5 * stage.stageHeight - 0.5 * HEIGHT;

        drawProgressBar(0);
    }

    private function onIOError(e:IOErrorEvent):void {
        trace(e.text);
    }

    private function onProgress(e:ProgressEvent):void {
        drawProgressBar(e.bytesLoaded / e.bytesTotal);
    }

    private function onEnterFrame(event:Event):void {
        if (currentFrame == totalFrames) {
            stop();
            loadingFinished();
        }
    }

    private function loadingFinished():void {
        removeEventListener(Event.ENTER_FRAME, onEnterFrame);
        loaderInfo.removeEventListener(ProgressEvent.PROGRESS, onProgress);
        loaderInfo.removeEventListener(IOErrorEvent.IO_ERROR, onIOError);

        graphics.clear();

        // Start game.
        var mainClass:Class = getDefinitionByName("stc.Platform") as Class;
        var platform:Platform = new mainClass() as Platform;
        addChild(platform);
        platform.init();
    }

    private function drawProgressBar(percent:Number):void {
        trace(percent);
        graphics.clear();
        graphics.lineStyle(1, BORDER_COLOR);
        graphics.drawRect(mX, mY, mWidth, HEIGHT);
        graphics.beginFill(BAR_COLOR);
        graphics.drawRect(mX + 2, mY + 2, percent * (mWidth - 4), HEIGHT - 4);
        graphics.endFill();
    }

    // Progress bar sizes.
    private const HEIGHT:int = 8;
    private const BAR_COLOR:Number = 0xFFAAAAAA;
    private const BORDER_COLOR:Number = 0xFF000000;

    private var mWidth:int;
    private var mX:int;
    private var mY:int;
}
}
