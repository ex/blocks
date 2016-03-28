/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Platform interface definition.                                           */
/*                                                                            */
/*   Copyright (c) 2013 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#ifndef STC_ARDUINO_GAME_HPP_
#define STC_ARDUINO_GAME_HPP_

#include "game.hpp"

namespace stc
{

// Game class forward declaration
class Game;

class PlatformArduino : public Platform
{
    // Sleep time (in milliseconds)
    static const int SLEEP_TIME = 40;

  public:
    // Initializes platform
    int init( Game *game );

    // Clear resources used by platform
    void end() { };

    // Process events and notify game
    void processEvents();

    // Render the state of the game
    void renderGame();

    // Return the current system time in milliseconds
    long getSystemTime();

    // Return a random positive integer number
    int random();

    // Events
    void onLineCompleted();
    void onPieceDrop();

    void println( const char *message );
    
  private:

    Game* mGame;

    void drawCell( int x, int y, int tile );
};
}

#endif // STC_ARDUINO_GAME_HPP_
