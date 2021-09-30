_Everything should be made as simple as possible, but no simpler._<br/>
_Albert Einstein_

## Introduction

There must be a gazillion Tetris clones out there. Why another one?.
Because I couldn't find a clone like this: with a _liberal_ open source license and programmed with code legibility in mind. There are many little projects like this but most of them have restrictive GPL licenses or are very difficult to read and change. This one is released under the permissive MIT license and hopefully it's easier to understand and modify.

![alt text](http://planetadev.googlepages.com/stc.png)

## Objectives

This clone must be easy to understand and because of that, easy to tweak (if you want to use it for your own projects). Changing rendering and input layers must be a breeze and even the core gameplay functions should be easy to port to another platforms or languages.
Ultimately, it must serve as an educational reference for novice game programmers. The good thing about Tetris is that it has a very well known gameplay mechanic and it is relatively simple to implement but challenging enough for not being trivial.

## Features

The core logic is programmed in ANSI C and C++. The gameplay logic is separated from the layers in charge of presenting the game to the player. Those _layers_ are responsible for drawing sprites on screen, handling input devices, returning timer values, or even playing sounds and music, so the game must be platform agnostic. The trunk implementation uses the SDL library for graphics and input but another languages and platforms would be supported in the branches.

This is the list of expected features I'm aiming:

  1. A simple _Tetris_ experience. The user must easily recognize the game and spent some time with it.
  2. Full separation between the core gameplay implementation and the graphic, input, or font libraries that could be used by the game.
  3. No _global_ variables in the core game implementation.
  4. Good code standards.

The ambition of this little project is to become the default implementation whenever you need a simple clone and don't want to rewrite your own from zero.

## Limitations

Because the trunk for this project _must_ be simple (and I'm lazy) some limitations are imposed:

  * No menu screens
  * No effects

Branches could implement the missing parts or create gameplay variations.

## Branches

This is the list of currently implemented and possible future branches:

  * [TypeScript](http://www.typescriptlang.org/) using HTML5 canvas. You can see the generated code [here](http://jsfiddle.net/exdev/sxGN3/) and [play it here](http://www.xprezion.com/ex/stc/stc.htm).

  ![alt text](http://sites.google.com/site/exeqtor/ts01.png)

  * [Killa](http://github.com/ex/killa) for [Love 0.8 with Killa support](https://bitbucket.org/ex/love) and [with Killa support](https://github.com/ex/cocos2d-x cocos2d-x)

  ![alt text](http://sites.google.com/site/exeqtor/stc_killa.png)

  * Haxe For [NME](http://www.haxenme.org/)

  * cocos2d-x for [cocos2d-x 0.11.0](https://github.com/cocos2d/cocos2d-x)

  ![alt text](http://sites.google.com/site/exeqtor/stc-cocos2dx.png)

  * Lua for [Love 0.8](http://bitbucket.org/rude/love/)

  ![alt text](http://sites.google.com/site/exeqtor/stc-love.png)

  * Unity (programmed in C#). You can play it online [here.](http://elrinconde-ex.blogspot.com/2011/10/tetris-clone-in-unity.html)

  ![alt text](http://sites.google.com/site/exeqtor/stc_unity.png)

  * iPhone and iPod Touch (using OpenGL ES 1.1 and an Objective-C bridge between iOS and the C++ game logic object) 

  ![alt text](http://sites.google.com/site/exeqtor/stc_iphone.png)

  * OpenGL for graphics cards that support OpenGL 1.1 (Windows)

  * J2ME for cellphones with CLDC 1.1 and MIDP 2.0.

  ![alt text](http://planetadev.googlepages.com/stc_j2me.jpg)

  * AS3 This version doesn't need the Flash IDE to build only the Flex SDK. You can play it online [here.](http://elrinconde-ex.blogspot.com/2010/02/simple-tetris-clone-flex-version.html)

  ![alt text](http://simple-tetris-clone.googlecode.com/files/fstc.png)

## References

A reference of the gameplay mechanics can be found here:
http://tetris.wikia.com/wiki/Gameplay_overview<br>
The original Tetris can be found here:
http://vadim.oversigma.com/Tetris.htm

  ![alt text](http://vadim.oversigma.com/Tet312-Game.gif)

## Credits

  * Programming by Laurens Rodriguez

  * Music by Jarno Alanko
