/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   A simple tetris clone.                                                   */
/*                                                                            */
/*   Copyright (c) 2010 Laurens Rodriguez Oscanoa.                            */
/*                                                                            */
/*   Permission is hereby granted, free of charge, to any person              */
/*   obtaining a copy of this software and associated documentation           */
/*   files (the "Software"), to deal in the Software without                  */
/*   restriction, including without limitation the rights to use,             */
/*   copy, modify, merge, publish, distribute, sublicense, and/or sell        */
/*   copies of the Software, and to permit persons to whom the                */
/*   Software is furnished to do so, subject to the following                 */
/*   conditions:                                                              */
/*                                                                            */
/*   The above copyright notice and this permission notice shall be           */
/*   included in all copies or substantial portions of the Software.          */
/*                                                                            */
/*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,          */
/*   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES          */
/*   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                 */
/*   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT              */
/*   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,             */
/*   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING             */
/*   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR            */
/*   OTHER DEALINGS IN THE SOFTWARE.                                          */
/* -------------------------------------------------------------------------- */
package com.ex.stc;

import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;

public class Main extends MIDlet implements CommandListener {

    private static final Command CMD_EXIT = new Command("Exit", Command.EXIT, 99);

    private Display mDisplay;
    private boolean mFirstTime = true;
    private Platform mCanvas;

    public Main() {
        mDisplay = Display.getDisplay(this);
    }

    /* MIDlet implementation. */
    public void startApp() throws MIDletStateChangeException {
        /* Note that startApp() can be called several times if pauseApp() */
        /* has been called in between.                                    */
        if (mFirstTime) {
            mCanvas = new Platform();
            mCanvas.addCommand(CMD_EXIT);
            mCanvas.setFullScreenMode(true);
            mCanvas.setCommandListener(this);
            mDisplay.setCurrent(mCanvas);
            mCanvas.repaint();
            mFirstTime = true;
        }
    }

    /* If the MIDlet was using resources, it should release them in this method. */
    public void destroyApp(boolean unconditional) {
        mDisplay.setCurrent(null);
        mCanvas.end();
    }

    /* This method is called to notify the MIDlet to enter a paused state. */
    /* The MIDlet should use this opportunity to release shared resources. */
    public void pauseApp() { }

    /* Respond to a command issued on the Canvas */
    public void commandAction(Command cmd, Displayable dsp) {
        if (cmd == CMD_EXIT) {
            destroyApp(false);
            notifyDestroyed();
            return;
        }
    }
}
