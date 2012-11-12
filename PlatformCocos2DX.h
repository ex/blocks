/* ========================================================================== */
/*   PlatformCocos2DX.h                                                       */
/*   Define: STC_COCOS2DX_USE_OGLES2    for OGLES 2 devices                   */
/* -------------------------------------------------------------------------- */
/*   Copyright (c) 2012 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#ifndef __PLATFORM_COCOS2DX_H__
#define __PLATFORM_COCOS2DX_H__

#include "cocos2d.h"
#include <vector>
#include "../../trunk/src/game.hpp"

class PlatformCocos2DX : public cocos2d::CCLayerColor, public stc::Platform
{
    // UI layout (quantities are expressed in pixels)

    // Background size
    static const int BACKGROUND_WIDTH  = 480;
    static const int BACKGROUND_HEIGHT = 272;

    // Size of square tile
    static const int TILE_SIZE = 12;

    // Board up-left corner coordinates
    static const int BOARD_X = 180;
    static const int BOARD_Y = 4;

    // Preview tetromino position
    static const int PREVIEW_X = 112;
    static const int PREVIEW_Y = 210;

    // Score position and length on screen
    static const int SCORE_X      = 72;
    static const int SCORE_Y      = 64;
    static const int SCORE_LENGTH = 10;

    // Lines position and length on screen
    static const int LINES_X      = 108;
    static const int LINES_Y      = 46;
    static const int LINES_LENGTH = 5;

    // Level position and length on screen
    static const int LEVEL_X      = 108;
    static const int LEVEL_Y      = 28;
    static const int LEVEL_LENGTH = 5;

    // Tetromino subtotals position
    static const int TETROMINO_X   = 425;
    static const int TETROMINO_L_Y = 53;
    static const int TETROMINO_I_Y = 77;
    static const int TETROMINO_T_Y = 101;
    static const int TETROMINO_S_Y = 125;
    static const int TETROMINO_Z_Y = 149;
    static const int TETROMINO_O_Y = 173;
    static const int TETROMINO_J_Y = 197;

    // Size of subtotals
    static const int TETROMINO_LENGTH = 5;

    // Tetromino total position
    static const int PIECES_X      = 418;
    static const int PIECES_Y      = 221;
    static const int PIECES_LENGTH = 6;

    // Size of number
    static const int NUMBER_WIDTH  = 7;
    static const int NUMBER_HEIGHT = 9;

    // Touch zones limits
    static const int TX_1 = 160;
    static const int TX_2 = 320;

    static const int TY_DROP = 250;    
    static const int TY_DOWN = 70;    
    
    static const int TY_1 = 50;    
    static const int TY_2 = 270;    

public:

    PlatformCocos2DX();
    ~PlatformCocos2DX();

    virtual int init(stc::Game* game);
    virtual void end();
    virtual void processEvents();
    virtual void renderGame();
    virtual long getSystemTime();
    virtual int random();

	// Cocos2d-x init function.
	virtual bool init();  

	// Return the scene pointer.
	static cocos2d::CCScene* scene();
	
	// Callback for the "close" menu.
	virtual void menuCloseCallback(CCObject* pSender);

    // Touch events.
    void ccTouchesEnded(cocos2d::CCSet * touches, cocos2d::CCEvent * event);
    void ccTouchesBegan(cocos2d::CCSet * touches, cocos2d::CCEvent * event);

    // Platform cocos2d-x update.
    void update(cocos2d::ccTime dt);

	// Implement the "static node()" method manually.
	LAYER_NODE_FUNC(PlatformCocos2DX);

private:

    void drawTile(int x, int y, int tile, bool shadow);
    void drawNumber(int x, int y, long number, int length, int color);

    long       m_elapsedTime; // Used to store the time passed.
    int        m_yOffset;     // Used to correct rendering positions.

    stc::Game* m_game;  // The game instance.

#ifdef STC_COCOS2DX_USE_OGLES2

    // Array of sprites for drawing, there is one more color than tetromino types.
    cocos2d::CCSprite* m_tiles[stc::Game::TETROMINO_TYPES + 1];
    cocos2d::CCSprite* m_shadows[stc::Game::TETROMINO_TYPES + 1];
    cocos2d::CCSprite* m_numbers[stc::Game::TETROMINO_TYPES + 1][10];

    // We draw the game state over this rendering target object.
    cocos2d::CCRenderTexture *m_canvas;

#else

    std::vector<cocos2d::CCSprite*> m_board;     // Board sprites.
    std::vector<cocos2d::CCSprite*> m_tetromino; // Falling tetromino sprites.
    std::vector<cocos2d::CCSprite*> m_shadow;    // Shadow tetromino sprites.
    std::vector<cocos2d::CCSprite*> m_next;      // Next tetromino sprites.

    cocos2d::CCSprite* getTile(int x, int y, int id, bool shadow);

#endif
};

#endif // __PLATFORM_COCOS2D_X_H__
