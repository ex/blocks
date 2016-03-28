/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Arduino port.                                                            */
/*                                                                            */
/*   Copyright (c) 2016 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#include "arduino_game.hpp"

// Game object
stc::Game game;

// Platform object
stc::PlatformArduino platform;

void setup()
{
    // Start the game
    game.init( &platform );
}

void loop()
{
    game.update();    
}
