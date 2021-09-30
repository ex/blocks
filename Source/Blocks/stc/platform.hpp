/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Platform interface definition.                                           */
/*                                                                            */
/*   Copyright (c) 2013 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#ifndef STC_SRC_PLATFORM_HPP_
#define STC_SRC_PLATFORM_HPP_

namespace stc
{

// Game class forward declaration
class Game;

class Platform
{
public:
    // Initializes platform
    virtual int init(Game *game) = 0;

    // Clear resources used by platform
    virtual void end() = 0;

    // Process events and notify game
    virtual void processEvents() = 0;

    // Render the state of the game
    virtual void renderGame() = 0;

    // Return the current system time in milliseconds
    virtual long getSystemTime() = 0;

    // Return a random positive integer number
    virtual int random() = 0;

    // Events
    virtual void onLineCompleted() = 0;
    virtual void onPieceDrop() = 0;
    virtual void onTetrominoMoved() = 0;
};
}

#endif // STC_SRC_PLATFORM_HPP_
