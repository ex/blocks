/* ========================================================================== */
/*   Assets.as                                                                */
/*   This class contains the resources used by our game.                      */
/*   Copyright (c) 2012 Laurens Rodriguez Oscanoa.                            */
/* -------------------------------------------------------------------------- */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

package stc {

public class Assets {

    [Embed(source = "../res/back.png")]
    public static const mcBmpBack:Class;

    [Embed(source = "../res/numbers.png")]
    public static const mcBmpNumbers:Class;

    // Embedded font with ascii printable characters.
    [Embed(source = '../res/luconex.ttf',
           fontName = "ConsoleEx",
           unicodeRange = "U+0021-U+007E",
           embedAsCFF='false',
           mimeType="application/x-font")]
    public static const fontProggy:Class;

    [Embed(source = "../res/stc_theme_loop.mp3")]
    public static const musicGame:Class;

    [Embed(source = "../res/fx_line.mp3")]
    public static const soundRow:Class;
}
}
