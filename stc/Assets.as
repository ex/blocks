/* ========================================================================== */
/*   Assets.as                                                                */
/*   This class contains the resources used by our game.                      */
/*   Copyright (c) 2010 Laurens Rodriguez Oscanoa.                            */
/* -------------------------------------------------------------------------- */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

package stc {

public class Assets {

    [Embed(source = "../res/back.png")]
    public static const mcBmpBack:Class;

    [Embed(source = "../res/blocks.png")]
    public static const mcBmpBlocks:Class;

    [Embed(source = "../res/numbers.png")]
    public static const mcBmpNumbers:Class;

    /* Source: http://www.proggyfonts.com/download/download_bridge.php?get=ProggyCleanSZ.ttf.zip */
    [Embed(source='../res/ProggyCleanSZ.ttf',
           fontName = "EmbeddedProggy",
           unicodeRange = "U+0041-U+005A", /* A-Z */
           embedAsCFF='true',
           mimeType="application/x-font")]
    public static const fontProggy:Class;

    [Embed(source = "../res/stc_theme_loop.mp3")]
    public static const musicLoop:Class;

}
}
