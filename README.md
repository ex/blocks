_Everything should be made as simple as possible, but no simpler._<br>
,,Albert Einstein,,

NOTE: this repository moved to [http://github.com/ex/simple-tetris-clone] and is no longer maintained.

==Introduction==

There must be a gazillion Tetris clones out there. Why another one?.
Because I couldn't find a clone like this: with a _liberal_ open source license and programmed with code legibility in mind. There are many little projects like this but most of them have restrictive GPL licenses or are very difficult to read and change. This one is released under the permissive MIT license and hopefully it's easier to understand and modify.

  http://planetadev.googlepages.com/stc.png

==Objectives==

This clone must be easy to understand and because of that, easy to tweak (if you want to use it for your own projects). Changing rendering and input layers must be a breeze and even the core gameplay functions should be easy to port to another platforms or languages.
Ultimately, it must serve as an educational reference for novice game programmers. The good thing about Tetris is that it has a very well known gameplay mechanic and it is relatively simple to implement but challenging enough for not being trivial.

==Features==

The core logic is programmed in ANSI C and C++. The gameplay logic is separated from the layers in charge of presenting the game to the player. Those _layers_ are responsible for drawing sprites on screen, handling input devices, returning timer values, or even playing sounds and music, so the game must be platform agnostic. The trunk implementation uses the SDL library for graphics and input but another languages and platforms would be supported in the branches.

This is the list of expected features I'm aiming:

  # A simple _Tetris_ experience. The user must easily recognize the game and spent some time with it.
  # Full separation between the core gameplay implementation and the graphic, input, or font libraries that could be used by the game.
  # No _global_ variables in the core game implementation.
  # Good code standards.

The ambition of this little project is to become the default implementation whenever you need a simple clone and don't want to rewrite your own from zero.

==Limitations==

Because the trunk for this project _must_ be simple (and I'm lazy) some limitations are imposed:

  # No menu screens
  # No effects

Branches could implement the missing parts or create gameplay variations.

==Branches==

This is the list of currently implemented and possible future branches:

  * [http://www.typescriptlang.org/ TypeScript] using HTML5 [http://code.google.com/p/simple-tetris-clone/source/browse/#svn%2Fbranches%2Fts canvas]. You can see the generated code [http://jsfiddle.net/exdev/sxGN3/ here] and [http://www.xprezion.com/ex/stc/stc.htm play it here].

  http://sites.google.com/site/exeqtor/ts01.png

  * [https://github.com/ex/killa Killa] For [https://bitbucket.org/ex/love Love 0.8 with Killa support] and [https://github.com/ex/cocos2d-x cocos2d-x with Killa support]

  http://sites.google.com/site/exeqtor/stc_killa.png

  * [http://code.google.com/p/simple-tetris-clone/source/browse/#svn%2Fbranches%2Fnme Haxe/NME] For [http://www.haxenme.org/ NME]

  * [http://code.google.com/p/simple-tetris-clone/source/browse/#svn%2Fbranches%2Fcocos2d-x cocos2d-x] For [https://github.com/cocos2d/cocos2d-x cocos2d-x 0.11.0]

  http://sites.google.com/site/exeqtor/stc-cocos2dx.png

  * [http://code.google.com/p/simple-tetris-clone/source/browse/#svn%2Fbranches%2Flove Lua] For [http://bitbucket.org/rude/love/ Love 0.8]

  http://sites.google.com/site/exeqtor/stc-love.png

  * [http://code.google.com/p/simple-tetris-clone/source/browse/#svn%2Fbranches%2Funity Unity] For Unity 3.4 free version (programmed in C#). You can play it online [http://elrinconde-ex.blogspot.com/2011/10/tetris-clone-in-unity.html here.]

  http://sites.google.com/site/exeqtor/stc_unity.png

  * [http://code.google.com/p/simple-tetris-clone/source/browse/#svn%2Fbranches%2Fiphone iPhone] For iPhone and iPod Touch (using OpenGL ES 1.1 and an Objective-C bridge between iOS and the C++ game logic object) 

  http://sites.google.com/site/exeqtor/stc_iphone.png

  * [http://code.google.com/p/simple-tetris-clone/source/browse/#svn%2Fbranches%2FOpenGL OpenGL] For graphics cards that support OpenGL 1.1 (Windows)

  * [http://code.google.com/p/simple-tetris-clone/source/browse/#svn/branches/j2me J2ME] For cellphones with CLDC 1.1 and MIDP 2.0.

  http://planetadev.googlepages.com/stc_j2me.jpg

  * [http://code.google.com/p/simple-tetris-clone/source/browse/#svn/branches/flex AS3] This version doesn't need the Flash IDE to build only the Flex SDK. You can play it online [http://elrinconde-ex.blogspot.com/2010/02/simple-tetris-clone-flex-version.html here.]

  http://simple-tetris-clone.googlecode.com/files/fstc.png

  * PIC
  * OCaml
  * Clojure

==References==

A reference of the gameplay mechanics can be found here:
http://tetris.wikia.com/wiki/Gameplay_overview<br>
The original Tetris can be found here:
http://vadim.oversigma.com/Tetris.htm

  http://vadim.oversigma.com/Tet312-Game.gif

==Credits==

  * Programming by Laurens Rodriguez

  * Music by Jarno Alanko

----

