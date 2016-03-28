/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Arduino support, tested on Arduino Mega                                  */
/*                                                                            */
/*   Copyright (c) 2015 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#include "arduino_game.hpp"
#include <LedControl.h>

//-----------------------------------------------------------------------------
// Set up the Ledcontrol library.
// http://playground.arduino.cc/Main/LedControl
#define DATA_PIN    7
#define SELECT_PIN  6
#define CLOCK_PIN   5
#define NUM_LED_MATRIXES 3
LedControl lc = LedControl( DATA_PIN, CLOCK_PIN, SELECT_PIN, NUM_LED_MATRIXES ); 

// Pin used by randomSeed, must be unconnected.
#define PIN_RANDOM_SEED   8

//-----------------------------------------------------------------------------
// Pins used by joystick.
// https://www.arduino.cc/en/Tutorial/JoyStick
#define PIN_JOYSTICK_X      14
#define PIN_JOYSTICK_Y      15
#define PIN_JOYSTICK_BUTTON 16

namespace stc
{
//-----------------------------------------------------------------------------  
// Initializes platform, if there are no problems returns ERROR_NONE.
int PlatformArduino::init( Game *game )
{
    // Initialize the random number generator
    randomSeed( analogRead( PIN_RANDOM_SEED ) );

    mGame = game;

    // Initialize led matrixes
    for ( int k = 0; k < NUM_LED_MATRIXES; k++ )
    {
      // Wake up the MAX72XX from power-saving mode 
      lc.shutdown( k, false );
      // Set brightness for the Leds
      lc.setIntensity( k, 5 );
      // Clear matrix
      lc.clearDisplay( k );
    }

    // Serial port initialization (for debug prints)
    Serial.begin( 9600 );
    Serial.println( "init" );

    delay( 500 );
    
    return Game::ERROR_NONE;
}

// Return the current system time in milliseconds
long PlatformArduino::getSystemTime()
{
    return millis();
}

// Process events and notify game
void PlatformArduino::processEvents()
{
}

// Draw a tile from a tetromino
void PlatformArduino::drawCell( int x, int y, int tile )
{
    int ly = y / 8;
    lc.setLed( 2 - ly, 7 - y % 8, x, true );
}

// Render the state of the game using platform functions
void PlatformArduino::renderGame()
{
    int i, j;

    // Check if the game state has changed, if so redraw
    if ( mGame->hasChanged() )
    {
        // Clear leds
        for ( int k = 0; k < NUM_LED_MATRIXES; k++ )
        {
          lc.clearDisplay( k );
        }

        // Draw the cells in the board
        for ( i = 0; i < Game::BOARD_TILEMAP_WIDTH; ++i )
        {
            for ( j = 0; j < Game::BOARD_TILEMAP_HEIGHT; ++j )
            {
                if ( mGame->getCell( i, j ) != Game::EMPTY_CELL )
                {
                    drawCell( i, j, mGame->getCell( i, j ) );
                }
            }
        }

        // Draw falling tetromino
        for ( i = 0; i < Game::TETROMINO_SIZE; ++i )
        {
            for ( j = 0; j < Game::TETROMINO_SIZE; ++j )
            {
                if ( mGame->fallingBlock().cells[i][j] != Game::EMPTY_CELL )
                {
                    drawCell( mGame->fallingBlock().x + i, mGame->fallingBlock().y + j,
                              mGame->fallingBlock().cells[i][j] );
                }
            }
        }

        // Inform the game that we are done with the changed state
        mGame->onChangeProcessed();
    }

    delay( SLEEP_TIME );
}

// Return a random positive integer number
int PlatformArduino::random()
{
    return ::random( 1048576 );
}

void PlatformArduino::onLineCompleted()
{
    Serial.println( "onLineCompleted" );  
}

void PlatformArduino::onPieceDrop()
{
    Serial.println( "onPieceDrop" );
}

void PlatformArduino::println( const char *message )
{
    Serial.println( message );  
}
}
