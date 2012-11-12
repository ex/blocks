/* ========================================================================== */
/*   PlatformCocos2DX.cpp                                                     */
/* -------------------------------------------------------------------------- */
/*   Copyright (c) 2012 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

#include "PlatformCocos2DX.h"

#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace stc;

PlatformCocos2DX::PlatformCocos2DX()
{
    // Create game object.
    m_game = new Game();

    // Initialize variable for elapsed time in milliseconds.
    m_elapsedTime = 0L;
}   

PlatformCocos2DX::~PlatformCocos2DX()
{
    // Delete created game.
    delete m_game;
    m_game = NULL;
}

CCScene * PlatformCocos2DX::scene()
{
	// 'scene' is an autorelease object.
	CCScene *scene = CCScene::node();
	
	// 'layer' is an autorelease object.
	PlatformCocos2DX *layer = PlatformCocos2DX::node();

	// Add layer as a child to scene.
	scene->addChild(layer);

	// Return the scene.
	return scene;
}

// on "init" you need to initialize your instance
bool PlatformCocos2DX::init()
{
	// First init super class.
    if (!CCLayerColor::initWithColor(ccc4f(255,255,255,255)))
    {
        return false;
    }

    // Initialize random number generator.
    srand((unsigned int)(time(NULL)));

    // Activate touches.
    setIsTouchEnabled(true);
    
    // Set this to avoid blurry rendering of sprites.
    CCDirector::sharedDirector()->setProjection(kCCDirectorProjection2D);

    // Disable FPS label.
    CCDirector::sharedDirector()->setDisplayFPS(false); 

    // Get game view size.
    CCSize size = CCDirector::sharedDirector()->getWinSize();

    // Calculate offset for drawing. (the origin in cocos2d-x is the bottom-left
    // corner and UI original postions were done for a 272x480 screen size)
    m_yOffset = int(size.height - (size.height - BACKGROUND_HEIGHT) / 2);

    //--------------------------------------------------------------------------
    // Add background image.
	CCSprite* pBackground = CCSprite::spriteWithFile("back.png");

	// Position the background on the center of the screen.
	pBackground->setPosition(ccp(size.width/2, size.height/2));

	// Add the background as a child to this layer.
	addChild(pBackground, 0);

#ifdef STC_COCOS2DX_USE_OGLES2
    //--------------------------------------------------------------------------
    // Create a render texture to draw the game state.
    m_canvas = CCRenderTexture::renderTextureWithWidthAndHeight(size.width, size.height);

	if (NULL == m_canvas)
	{
		return false;
	}

    m_canvas->setPosition(ccp(size.width/2, size.height/2));

    // Note that the render texture is a cocos node, and contains a sprite of it's texture 
    // for convience, so we can just parent it to the scene like any other cocos node.
    addChild(m_canvas, 1);

    //--------------------------------------------------------------------------
    // Create the sprites for drawing the tetrominoes.
    for (int k = 0; k < Game::TETROMINO_TYPES + 1; ++k)
    {
        // Images for tiles have 2 extra pixels
        m_tiles[k] = CCSprite::spriteWithFile("blocks.png", 
                                              CCRectMake((TILE_SIZE + 2) * k, 0, 
                                                          TILE_SIZE + 2, TILE_SIZE + 2));
        m_tiles[k]->retain();
    }

    //--------------------------------------------------------------------------
    // Create the sprites for drawing the shadows.
    for (int k = 0; k < Game::TETROMINO_TYPES + 1; ++k)
    {
        // Images for shadows have 2 pixels less
        m_shadows[k] = CCSprite::spriteWithFile("blocks.png", 
                                                CCRectMake((TILE_SIZE  + 2) * k, TILE_SIZE + 2, 
                                                            TILE_SIZE - 2, TILE_SIZE - 2));
        m_shadows[k]->retain();
    }

    //--------------------------------------------------------------------------
    // Create the sprites for drawing the numbers.
    for (int k = 0; k < Game::TETROMINO_TYPES + 1; ++k)
    {
        for (int i = 0; i < 10; ++i)
        {
            // Images for numbers have 1 extra pixel
            m_numbers[k][i] = CCSprite::spriteWithFile("numbers.png", 
                                                       CCRectMake((NUMBER_WIDTH + 1) * i, (NUMBER_HEIGHT  + 1) * k,
                                                                  NUMBER_WIDTH + 1, NUMBER_HEIGHT + 1));
            m_numbers[k][i]->retain();
        }
    }
#else
    for (int k = 0; k < Game::TETROMINO_SIZE * Game::TETROMINO_SIZE; k += 1)
    {
        m_tetromino.push_back(NULL);
        m_shadow.push_back(NULL);
        m_next.push_back(NULL);
    }
    for (int k = 0; k < Game::BOARD_TILEMAP_WIDTH * Game::BOARD_TILEMAP_HEIGHT; k += 1)
    {
        m_board.push_back(NULL);
    }
#endif

    //--------------------------------------------------------------------------
	// Add a "close" icon to exit the game, it's an autorelease object.
    CCMenuItemImage *pCloseItem = CCMenuItemImage::itemFromNormalImage(
                                        "close_normal.png",
                                        "close_selected.png",
                                        this,
                                        menu_selector(PlatformCocos2DX::menuCloseCallback));
    pCloseItem->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20));

	// Create menu, it's an autorelease object.
	CCMenu* pMenu = CCMenu::menuWithItems(pCloseItem, NULL);
	pMenu->setPosition(CCPointZero);
	this->addChild(pMenu, 1);

    //--------------------------------------------------------------------------
    // Schedule update event.
    this->schedule(schedule_selector(PlatformCocos2DX::update));

    // Connect game with this platform. 
    m_game->init(this);

	return true;
}

#ifndef STC_COCOS2DX_USE_OGLES2
// Return a sprite for a tetromino tile.
CCSprite* PlatformCocos2DX::getTile(int x, int y, int id, bool shadow)
{
    CCSprite* tile;
    if (!shadow)
    {
        // Images for tiles have 2 extra pixels
        tile = CCSprite::spriteWithFile("blocks.png", 
                                        CCRectMake((TILE_SIZE + 2) * id, 0, 
                                                    TILE_SIZE + 2, TILE_SIZE + 2));
        tile->setPosition(CCPoint(x + TILE_SIZE / 2, m_yOffset - y - TILE_SIZE / 2));
    }
    else
    {
        // Images for shadows have 2 pixels less
        tile = CCSprite::spriteWithFile("blocks.png", 
                                        CCRectMake((TILE_SIZE  + 2) * id, TILE_SIZE + 2, 
                                                    TILE_SIZE - 2, TILE_SIZE - 2));
        tile->setPosition(CCPoint(x + TILE_SIZE / 2 - 1, m_yOffset - y - TILE_SIZE / 2));
    }
    return tile;
}
#endif

void PlatformCocos2DX::menuCloseCallback(CCObject* pSender)
{
	CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

void PlatformCocos2DX::ccTouchesBegan(CCSet * touches, CCEvent * event)
{
    for (CCSetIterator it = touches->begin(); it != touches->end(); ++it) 
    {
        CCTouch * touch = (CCTouch *)(*it);
        float tx = touch->locationInView().x;
        float ty = touch->locationInView().y;
        
        if (tx < TX_1)
        {
            if (ty < TY_1)
            {
                m_game->onEventStart(Game::EVENT_SHOW_NEXT);
            }
            else if (ty < TY_2)
            {
                m_game->onEventStart(Game::EVENT_MOVE_LEFT);                    
            }
            else
            {
				m_game->onEventStart(Game::EVENT_RESTART);
            }
        }
        else if (tx < TX_2)
        {
            if (ty < TY_DOWN)
            {
                m_game->onEventStart(Game::EVENT_ROTATE_CW);
            }
            else if (ty > TY_DROP)
            {
                m_game->onEventStart(Game::EVENT_DROP);
            }
            else
            {
                m_game->onEventStart(Game::EVENT_MOVE_DOWN);
            }
        }
        else
        {
            if (ty < TY_1)
            {
#ifdef STC_SHOW_GHOST_PIECE
                m_game->onEventStart(Game::EVENT_SHOW_SHADOW);
#endif
            }
            else if (ty < TY_2)
            {
                m_game->onEventStart(Game::EVENT_MOVE_RIGHT);
            }
            else
            {
                m_game->onEventStart(Game::EVENT_PAUSE);
            }
        }
        CCLOG("-- touchStart: %d %d", int(tx), int(ty));
    }
}

void PlatformCocos2DX::ccTouchesEnded(CCSet* touches, CCEvent* event)
{
    for (CCSetIterator it = touches->begin(); it != touches->end(); ++it) 
    {
        CCTouch* touch = (CCTouch *)(*it);
        float tx = touch->locationInView().x;
        float ty = touch->locationInView().y;
        CCLOG("-- touchEnd: %d %d", int(tx), int(ty));    
        
        // Just cancel any continuos action by now
        m_game->onEventEnd(Game::EVENT_MOVE_LEFT);
        m_game->onEventEnd(Game::EVENT_MOVE_RIGHT);
        m_game->onEventEnd(Game::EVENT_MOVE_DOWN);
#ifdef STC_AUTO_ROTATION
        m_game->onEventEnd(Game::EVENT_ROTATE_CW);
#endif    
    } 
}

void PlatformCocos2DX::update(ccTime dt)
{
    // Update the elapsed time.
    m_elapsedTime += long(1000 * dt);

    // Update game.
    m_game->update();
} 

void PlatformCocos2DX::processEvents()
{
}

int PlatformCocos2DX::init(Game *game)
{
    // Platform was already initialized.
    return Game::ERROR_NONE;
}

void PlatformCocos2DX::end()
{
}

// Draw a tile from a tetromino.
// Tile sprites start in the top-left corner of the compounded image. 
void PlatformCocos2DX::drawTile(int x, int y, int tile, bool shadow)
{
#ifdef STC_COCOS2DX_USE_OGLES2
    if (!shadow) 
    {
        m_tiles[tile]->setPosition(ccp(x + TILE_SIZE / 2, m_yOffset - y - TILE_SIZE / 2));
        m_tiles[tile]->visit();
    }
    else
    {
        m_shadows[tile]->setPosition(ccp(x + TILE_SIZE / 2 - 1, m_yOffset - y - TILE_SIZE / 2));
        m_shadows[tile]->visit();
    }
#endif
}

// Draw a number on the given position.
// Number sprites start at the left below the sprites for tiles. 
void PlatformCocos2DX::drawNumber(int x, int y, long number, int length, int color)
{
#ifdef STC_COCOS2DX_USE_OGLES2
    int pos = 0;
    do
    {
        int px = x + NUMBER_WIDTH * (length - pos);
        m_numbers[color][number % 10]->setPosition(ccp(px + NUMBER_WIDTH / 2 + 1,
                                                       m_yOffset - y - NUMBER_HEIGHT / 2));
        m_numbers[color][number % 10]->visit();

        number /= 10;

    } while (++pos < length);
#else

#endif    
}

// Render the state of the game using platform functions.
void PlatformCocos2DX::renderGame()
{
    int i, j;
#ifdef STC_COCOS2DX_USE_OGLES2
    // Check if the game state has changed, if so redraw
    if (m_game->hasChanged())
    {
    	m_canvas->beginWithClear(0.f, 0.f, 0.f, 0.f);

        // Draw preview block
        if (m_game->showPreview())
        {
            for (i = 0; i < Game::TETROMINO_SIZE; ++i)
            {
                for (j = 0; j < Game::TETROMINO_SIZE; ++j)
                {
                    if (m_game->nextBlock().cells[i][j] != Game::EMPTY_CELL)
                    {
                        drawTile(PREVIEW_X + (TILE_SIZE * i),
                                 PREVIEW_Y + (TILE_SIZE * j),
                                 m_game->nextBlock().cells[i][j], false);
                    }
                }
            }
        }
#ifdef STC_SHOW_GHOST_PIECE
        // Draw shadow tetromino
        if (m_game->showShadow() && m_game->shadowGap() > 0)
        {
            for (i = 0; i < Game::TETROMINO_SIZE; ++i)
            {
                for (j = 0; j < Game::TETROMINO_SIZE; ++j)
                {
                    if (m_game->fallingBlock().cells[i][j] != Game::EMPTY_CELL)
                    {
                        drawTile(BOARD_X + (TILE_SIZE * (m_game->fallingBlock().x + i)),
                                 BOARD_Y + (TILE_SIZE * (m_game->fallingBlock().y + m_game->shadowGap() + j)),
                                 m_game->fallingBlock().cells[i][j], true);
                    }
                }
            }
        }
#endif
        // Draw the cells in the board
        for (i = 0; i < Game::BOARD_TILEMAP_WIDTH; ++i)
        {
            for (j = 0; j < Game::BOARD_TILEMAP_HEIGHT; ++j)
            {
                if (m_game->getCell(i, j) != Game::EMPTY_CELL)
                {
                    drawTile(BOARD_X + (TILE_SIZE * i),
                             BOARD_Y + (TILE_SIZE * j),
                             m_game->getCell(i, j), false);
                }
            }
        }

        // Draw falling tetromino
        for (i = 0; i < Game::TETROMINO_SIZE; ++i)
        {
            for (j = 0; j < Game::TETROMINO_SIZE; ++j) 
            {
                if (m_game->fallingBlock().cells[i][j] != Game::EMPTY_CELL)
                {
                    drawTile(BOARD_X + (TILE_SIZE * (m_game->fallingBlock().x + i)),
                             BOARD_Y + (TILE_SIZE * (m_game->fallingBlock().y + j)),
                             m_game->fallingBlock().cells[i][j], false);
                }
            }
        }

        // Draw game statistic data
        if (!m_game->isPaused())
        {
            drawNumber(LEVEL_X, LEVEL_Y, m_game->stats().level, LEVEL_LENGTH, Game::COLOR_WHITE);
            drawNumber(LINES_X, LINES_Y, m_game->stats().lines, LINES_LENGTH, Game::COLOR_WHITE);
            drawNumber(SCORE_X, SCORE_Y, m_game->stats().score, SCORE_LENGTH, Game::COLOR_WHITE);

            drawNumber(TETROMINO_X, TETROMINO_L_Y, m_game->stats().pieces[Game::TETROMINO_L], TETROMINO_LENGTH, Game::COLOR_ORANGE);
            drawNumber(TETROMINO_X, TETROMINO_I_Y, m_game->stats().pieces[Game::TETROMINO_I], TETROMINO_LENGTH, Game::COLOR_CYAN);
            drawNumber(TETROMINO_X, TETROMINO_T_Y, m_game->stats().pieces[Game::TETROMINO_T], TETROMINO_LENGTH, Game::COLOR_PURPLE);
            drawNumber(TETROMINO_X, TETROMINO_S_Y, m_game->stats().pieces[Game::TETROMINO_S], TETROMINO_LENGTH, Game::COLOR_GREEN);
            drawNumber(TETROMINO_X, TETROMINO_Z_Y, m_game->stats().pieces[Game::TETROMINO_Z], TETROMINO_LENGTH, Game::COLOR_RED);
            drawNumber(TETROMINO_X, TETROMINO_O_Y, m_game->stats().pieces[Game::TETROMINO_O], TETROMINO_LENGTH, Game::COLOR_YELLOW);
            drawNumber(TETROMINO_X, TETROMINO_J_Y, m_game->stats().pieces[Game::TETROMINO_J], TETROMINO_LENGTH, Game::COLOR_BLUE);

            drawNumber(PIECES_X, PIECES_Y, m_game->stats().totalPieces, PIECES_LENGTH, Game::COLOR_WHITE);
        }

        // Inform the game that we are done with the changed state
        m_game->onChangeProcessed();

        m_canvas->end();
    }

#else // !STC_COCOS2DX_USE_OGLES2

    // Check if the game state has changed, if so redraw
    if (m_game->hasChanged())
    {
        // Draw shadow tetromino
        if (m_game->shadowGap() >= 0)
        {
            for (i = 0; i < Game::TETROMINO_SIZE; i += 1)
            {
                for (j = 0; j < Game::TETROMINO_SIZE; j += 1)
                {
                    CCSprite* tile = m_shadow[i * Game::TETROMINO_SIZE + j];
                    if (tile != NULL)
                    {
                        removeChild(tile, true);
                        m_shadow[i * Game::TETROMINO_SIZE + j] = NULL;
                    }
                    if (m_game->showShadow() && (m_game->fallingBlock().cells[i][j] != Game::EMPTY_CELL))
                    {
                        tile = getTile(BOARD_X + TILE_SIZE * (m_game->fallingBlock().x + i),
                                       BOARD_Y + TILE_SIZE * (m_game->fallingBlock().y + m_game->shadowGap() + j),
                                       m_game->fallingBlock().cells[i][j], true);
                        addChild(tile);
                        m_shadow[i * Game::TETROMINO_SIZE + j] = tile;
                    }
                }
            }
        }

        // Draw falling tetromino
        for (i = 0; i < Game::TETROMINO_SIZE; i += 1)
        {
            for (j = 0; j < Game::TETROMINO_SIZE; j += 1)
            {
                CCSprite* tile = m_tetromino[i * Game::TETROMINO_SIZE + j];
                if (tile != NULL)
                {
                    removeChild(tile, true);
                    m_tetromino[i * Game::TETROMINO_SIZE + j] = NULL;
                }
                if (m_game->fallingBlock().cells[i][j] != Game::EMPTY_CELL)
                {
                    tile = getTile(BOARD_X + TILE_SIZE * (m_game->fallingBlock().x + i),
                                   BOARD_Y + TILE_SIZE * (m_game->fallingBlock().y + j),
                                   m_game->fallingBlock().cells[i][j], false);
                    addChild(tile);
                    m_tetromino[i * Game::TETROMINO_SIZE + j] = tile;
                }
            }
        }

        // Draw preview block
        for (i = 0; i < Game::TETROMINO_SIZE; i += 1)
        {
            for (j = 0; j < Game::TETROMINO_SIZE; j += 1)
            {
                CCSprite* tile = m_next[i * Game::TETROMINO_SIZE + j];
                if (tile != NULL)
                {
                    removeChild(tile, true);
                    m_next[i * Game::TETROMINO_SIZE + j] = NULL;
                }
                if (m_game->showPreview() && (m_game->nextBlock().cells[i][j] != Game::EMPTY_CELL))
                {
                    tile = getTile(PREVIEW_X + (TILE_SIZE * i),
                                   PREVIEW_Y + (TILE_SIZE * j),
                                   m_game->nextBlock().cells[i][j], false);
                    addChild(tile);
                    m_next[i * Game::TETROMINO_SIZE + j] = tile;
                }
            }
        }

        // Draw the cells in the board
        for (i = 0; i < Game::BOARD_TILEMAP_WIDTH; i += 1)
        {
            for (j = 0; j < Game::BOARD_TILEMAP_HEIGHT; j += 1)
            {
                int index = i + j * Game::BOARD_TILEMAP_WIDTH;
                CCSprite* tile = m_board[index];
                int id = m_game->getCell(i, j);

                if (id == Game::EMPTY_CELL)
                {
                    if (tile != NULL)
                    {
                        removeChild(tile, true);
                        m_board[index] = NULL;
                    }
                }
                else
                {
                    if (tile == NULL)
                    {
                        tile = getTile(BOARD_X + (TILE_SIZE * i),
                                       BOARD_Y + (TILE_SIZE * j),
                                       id, false);
                        addChild(tile, 0, id);
                        m_board[index] = tile;
                    }
                    else
                    {
                        if (tile->getTag() != id)
                        {
                            removeChild(tile, true);
                            tile = getTile(BOARD_X + (TILE_SIZE * i),
                                           BOARD_Y + (TILE_SIZE * j),
                                           id, false);
                            addChild(tile, 0, id);
                            m_board[index] = tile;
                        }
                    }
                }
            }
        }

        // Inform to the game that we are done with the move.
        m_game->onChangeProcessed();
    }
#endif
}

long PlatformCocos2DX::getSystemTime()
{
    return m_elapsedTime;
}

int PlatformCocos2DX::random()
{
    return rand();
}
