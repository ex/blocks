/* ========================================================================== */
/*                          STC - SIMPLE TETRIS CLONE                         */
/* -------------------------------------------------------------------------- */
/*   Copyright (c) 2012 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */
#include "PlatformObjC.hpp"

#include <assert.h>

namespace stc {

PlatformObjC::PlatformObjC(id controller) {
    mGame = NULL;
    mTexture = NULL;
    mController = controller;
    NSDate *now = [NSDate date];
    mTimeStart = [now timeIntervalSince1970];
}

void PlatformObjC::processEvents() {
}

void PlatformObjC::onTouchStart(id event) {
    
    if (event != nil) {

        for (int i = 0; i < [[event allTouches] count]; i++) {
        
            UITouch *touch = [[[event allTouches] allObjects] objectAtIndex: i];
            float tx = [touch locationInView:touch.view].x;
            float ty = [touch locationInView:touch.view].y;
            
            if (ty < TY_1) {
                if (tx < TX_1) {
                    mGame->onEventStart(Game::EVENT_SHOW_NEXT);
                }
                else if (tx < TX_2) {
                    mGame->onEventStart(Game::EVENT_MOVE_LEFT);                    
                }
                else {
					if (mGame->isOver()) {
						mGame->onEventStart(Game::EVENT_RESTART);
					}
					else {
						mGame->onEventStart(Game::EVENT_PAUSE);

						[mController setAlertRestart: [[UIAlertView alloc] initWithTitle:@"Warning" message:@"Do you want to restart?"
                                                    delegate:mController cancelButtonTitle:@"Cancel" 
                                                                       otherButtonTitles:@"Restart", nil]];
						[[mController alertRestart] show];
					}
                }
            }
            else if (ty < TY_2) {
                if (tx < TX_DROP) {
                    mGame->onEventStart(Game::EVENT_DROP);
                }
                else if (tx < TX_DOWN) {
                    mGame->onEventStart(Game::EVENT_MOVE_DOWN);
                }
                else {
                    mGame->onEventStart(Game::EVENT_ROTATE_CW);
                }
            }
            else {
                if (tx < TX_1) {
#ifdef STC_SHOW_GHOST_PIECE
                    mGame->onEventStart(Game::EVENT_SHOW_SHADOW);
#endif
                }
                else if (tx < TX_2) {
                    mGame->onEventStart(Game::EVENT_MOVE_RIGHT);
                }
                else {
					[mController setAlertPaused: [[UIAlertView alloc] initWithTitle:@"Simple Tetris Clone" message:@"Game is paused"
                                                                           delegate:mController cancelButtonTitle:@"Continue" 
                                                                  otherButtonTitles:nil, nil]];
					[[mController alertPaused] show];
					
                    mGame->onEventStart(Game::EVENT_PAUSE);
                }
            }
            NSLog(@"-- touchStart: %d %d", int(tx), int(ty));
        }
    }
}
    
void PlatformObjC::onTouchEnd(id event) {
        
    if (event != nil) {
        
        UITouch *touch = [[event allTouches] anyObject];
        float tx = [touch locationInView:touch.view].x;
        float ty = [touch locationInView:touch.view].y; 
        NSLog(@"-- touchEnd: %d %d", int(tx), int(ty));    
        
        // Just cancel any continuos action by now
        mGame->onEventEnd(Game::EVENT_MOVE_LEFT);
        mGame->onEventEnd(Game::EVENT_MOVE_RIGHT);
        mGame->onEventEnd(Game::EVENT_MOVE_DOWN);
#ifdef STC_AUTO_ROTATION
        mGame->onEventEnd(Game::EVENT_ROTATE_CW);
#endif    
    }
}

int PlatformObjC::init(Game *game) {

    // Setup OpenGL 2D view.
    // -------------------------------------------------------------------------
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrthof(0.f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.f, -1.f, 1.f);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0.375f, 0.375f, 0.f);

    // Make sure depth testing and lighting are disabled for 2D rendering
	glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Setup image texture
    // -------------------------------------------------------------------------
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mTexture = new TargaImage();
    
    NSBundle *bundle = [NSBundle mainBundle];
    NSString *dir = [bundle resourcePath];
    NSArray *parts = [NSArray arrayWithObjects:
                      dir, @"back.tga", (void *)nil];
    NSString *path = [NSString pathWithComponents:parts];
    const char *cpath = [path fileSystemRepresentation];
    
    if (!mTexture->load(cpath)) {
        return Game::ERROR_NO_IMAGES;
    }

    glGenTextures(1, &mTextureId);
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    assert(mTexture->getWidth() == TEXTURE_SIZE);
    assert(mTexture->getHeight() == TEXTURE_SIZE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXTURE_SIZE, TEXTURE_SIZE,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, mTexture->getImageData());

    // Initialize background texture coordinates
    mBackgroundTexCoord = new GLfloat[8];
    mBackgroundTexCoord[0] = 0.f;
    mBackgroundTexCoord[1] = 0.f;
    mBackgroundTexCoord[2] = BACKGROUND_WIDTH / GLfloat(TEXTURE_SIZE);
    mBackgroundTexCoord[3] = 0.f;
    mBackgroundTexCoord[4] = 0.f;
    mBackgroundTexCoord[5] = BACKGROUND_HEIGHT / GLfloat(TEXTURE_SIZE);
    mBackgroundTexCoord[6] = BACKGROUND_WIDTH / GLfloat(TEXTURE_SIZE);
    mBackgroundTexCoord[7] = BACKGROUND_HEIGHT / GLfloat(TEXTURE_SIZE);

    mBackgroundVertices = new GLfloat[12];
    mBackgroundVertices[0] = 0.f;                    // down-left
    mBackgroundVertices[1] = GLfloat(BACKGROUND_HEIGHT);
    mBackgroundVertices[2] = 0.f;
    mBackgroundVertices[3] = GLfloat(BACKGROUND_WIDTH);  // down-right
    mBackgroundVertices[4] = GLfloat(BACKGROUND_HEIGHT);
    mBackgroundVertices[5] = 0.f;
    mBackgroundVertices[6] = 0.f;                    // up-left
    mBackgroundVertices[7] = 0.f;
    mBackgroundVertices[8] = 0.f;
    mBackgroundVertices[9] = GLfloat(BACKGROUND_WIDTH);  // up-right
    mBackgroundVertices[10] = 0.f;
    mBackgroundVertices[11] = 0.f;

    // Initialize indexes used for drawing sprites
    mTextureIndexes = new GLubyte[4];
    for (int k = 0; k < 4; k++) {
        mTextureIndexes[k] = k;
    }

    // Create arrays for drawing the sprites
    mSpriteTexCoord = new GLfloat[8];

    mSpriteVertices = new GLfloat[12];
    mSpriteVertices[2] = 0.f;   // Z coordinates are all zero
    mSpriteVertices[5] = 0.f;
    mSpriteVertices[8] = 0.f;
    mSpriteVertices[11] = 0.f;

    // We don't change textures neither 2D view mode, so do this only once.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // Rotate view for landscape mode
    glRotatef(90.f, 0.f, 0.f, 1.f);
    glTranslatef(0.f, 0.5f * (SCREEN_WIDTH - BACKGROUND_HEIGHT) - SCREEN_WIDTH, 0.0f);
    
    // Save reference to game and return that everything is OK
    mGame = game;
    return Game::ERROR_NONE;
}

void PlatformObjC::end() {

    delete[] mSpriteTexCoord;
    delete[] mSpriteVertices;
    delete[] mBackgroundVertices;
    delete[] mBackgroundTexCoord;
    delete[] mTextureIndexes;

    delete mTexture;
    mTexture = NULL;
}

// Set the texture coordinates used for rendering a sprite.
void PlatformObjC::setSpriteTextureCoord(GLfloat *coords, int x, int y, int w, int h) {
    // down-left
    coords[0] = x / GLfloat(TEXTURE_SIZE);   
    coords[1] = (TEXTURE_SIZE - y - h) / GLfloat(TEXTURE_SIZE);
    // down-right
    coords[2] = (x + w) / GLfloat(TEXTURE_SIZE);   
    coords[3] = (TEXTURE_SIZE - y - h) / GLfloat(TEXTURE_SIZE);
    // up-left
    coords[4] = x / GLfloat(TEXTURE_SIZE);   
    coords[5] = (TEXTURE_SIZE - y) / GLfloat(TEXTURE_SIZE);
    // up-right
    coords[6] = (x + w) / GLfloat(TEXTURE_SIZE);   
    coords[7] = (TEXTURE_SIZE - y) / GLfloat(TEXTURE_SIZE);
}

// Draw a tile from a tetromino.
// Tile sprites start in the top-left corner of the compounded image. 
void PlatformObjC::drawTile(int x, int y, int tile, bool shadow) {

    mSpriteVertices[0] = GLfloat(x);             // down-left
    mSpriteVertices[1] = GLfloat(y + TILE_SIZE);
    mSpriteVertices[3] = GLfloat(x + TILE_SIZE); // down-right
    mSpriteVertices[4] = GLfloat(y + TILE_SIZE);
    mSpriteVertices[6] = GLfloat(x);             // up-left
    mSpriteVertices[7] = GLfloat(y);
    mSpriteVertices[9] = GLfloat(x + TILE_SIZE); // up-right
    mSpriteVertices[10] = GLfloat(y);

    setSpriteTextureCoord(mSpriteTexCoord, 
                          TILE_SIZE * (shadow? Game::TETROMINO_TYPES + tile + 1: tile), 0,
                          TILE_SIZE, TILE_SIZE);

    glVertexPointer(3, GL_FLOAT, 0, mSpriteVertices);
    glTexCoordPointer(2, GL_FLOAT, 0, mSpriteTexCoord);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, mTextureIndexes);
}

// Draw a number on the given position.
// Number sprites start at the left below the sprites for tiles. 
void PlatformObjC::drawNumber(int x, int y, long number, int length, int color) {
    // Y coordinates don't change
    mSpriteVertices[1] = GLfloat(y + NUMBER_HEIGHT); // down-left
    mSpriteVertices[4] = GLfloat(y + NUMBER_HEIGHT); // down-right
    mSpriteVertices[7] = GLfloat(y);                 // up-left
    mSpriteVertices[10] = GLfloat(y);                // up-right

    int pos = 0;
    do {
        int px = x + NUMBER_WIDTH * (length - pos);
        mSpriteVertices[0] = GLfloat(px);                // down-left
        mSpriteVertices[3] = GLfloat(px + NUMBER_WIDTH); // down-right
        mSpriteVertices[6] = GLfloat(px);                // up-left
        mSpriteVertices[9] = GLfloat(px + NUMBER_WIDTH); // up-right

        setSpriteTextureCoord(mSpriteTexCoord, 
                              NUMBER_WIDTH * (number % 10), 1 + TILE_SIZE + NUMBER_HEIGHT * color,
                              NUMBER_WIDTH, NUMBER_HEIGHT);

        glVertexPointer(3, GL_FLOAT, 0, mSpriteVertices);
        glTexCoordPointer(2, GL_FLOAT, 0, mSpriteTexCoord);
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, mTextureIndexes);

        number /= 10;
    } while (++pos < length);
}

// Render the state of the game using platform functions.
void PlatformObjC::renderGame() {
    int i, j;

    // Check if the game state has changed, if so redraw
    if (mGame->hasChanged()) {
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw background
        glVertexPointer(3, GL_FLOAT, 0, mBackgroundVertices);
        glTexCoordPointer(2, GL_FLOAT, 0, mBackgroundTexCoord);
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, mTextureIndexes);

        // Draw preview block
        if (mGame->showPreview()) {
            for (i = 0; i < Game::TETROMINO_SIZE; ++i) {
                for (j = 0; j < Game::TETROMINO_SIZE; ++j) {
                    if (mGame->nextBlock().cells[i][j] != Game::EMPTY_CELL) {
                        drawTile(PREVIEW_X + (TILE_SIZE * i),
                                 PREVIEW_Y + (TILE_SIZE * j),
                                 mGame->nextBlock().cells[i][j], false);
                    }
                }
            }
        }
#ifdef STC_SHOW_GHOST_PIECE
        // Draw shadow tetromino
        if (mGame->showShadow() && mGame->shadowGap() > 0) {
            for (i = 0; i < Game::TETROMINO_SIZE; ++i) {
                for (j = 0; j < Game::TETROMINO_SIZE; ++j) {
                    if (mGame->fallingBlock().cells[i][j] != Game::EMPTY_CELL) {
                        drawTile(BOARD_X + (TILE_SIZE * (mGame->fallingBlock().x + i)),
                                 BOARD_Y + (TILE_SIZE * (mGame->fallingBlock().y + mGame->shadowGap() + j)),
                                 mGame->fallingBlock().cells[i][j], true);
                    }
                }
            }
        }
#endif
        // Draw the cells in the board
        for (i = 0; i < Game::BOARD_TILEMAP_WIDTH; ++i) {
            for (j = 0; j < Game::BOARD_TILEMAP_HEIGHT; ++j) {
                if (mGame->getCell(i, j) != Game::EMPTY_CELL) {
                    drawTile(BOARD_X + (TILE_SIZE * i),
                             BOARD_Y + (TILE_SIZE * j),
                             mGame->getCell(i, j), false);
                }
            }
        }

        // Draw falling tetromino
        for (i = 0; i < Game::TETROMINO_SIZE; ++i) {
            for (j = 0; j < Game::TETROMINO_SIZE; ++j) {
                if (mGame->fallingBlock().cells[i][j] != Game::EMPTY_CELL) {
                    drawTile(BOARD_X + (TILE_SIZE * (mGame->fallingBlock().x + i)),
                             BOARD_Y + (TILE_SIZE * (mGame->fallingBlock().y + j)),
                             mGame->fallingBlock().cells[i][j], false);
                }
            }
        }

        /* Draw game statistic data */
        if (!mGame->isPaused()) {
            drawNumber(LEVEL_X, LEVEL_Y, mGame->stats().level, LEVEL_LENGTH, Game::COLOR_WHITE);
            drawNumber(LINES_X, LINES_Y, mGame->stats().lines, LINES_LENGTH, Game::COLOR_WHITE);
            drawNumber(SCORE_X, SCORE_Y, mGame->stats().score, SCORE_LENGTH, Game::COLOR_WHITE);

            drawNumber(TETROMINO_X, TETROMINO_L_Y, mGame->stats().pieces[Game::TETROMINO_L], TETROMINO_LENGTH, Game::COLOR_ORANGE);
            drawNumber(TETROMINO_X, TETROMINO_I_Y, mGame->stats().pieces[Game::TETROMINO_I], TETROMINO_LENGTH, Game::COLOR_CYAN);
            drawNumber(TETROMINO_X, TETROMINO_T_Y, mGame->stats().pieces[Game::TETROMINO_T], TETROMINO_LENGTH, Game::COLOR_PURPLE);
            drawNumber(TETROMINO_X, TETROMINO_S_Y, mGame->stats().pieces[Game::TETROMINO_S], TETROMINO_LENGTH, Game::COLOR_GREEN);
            drawNumber(TETROMINO_X, TETROMINO_Z_Y, mGame->stats().pieces[Game::TETROMINO_Z], TETROMINO_LENGTH, Game::COLOR_RED);
            drawNumber(TETROMINO_X, TETROMINO_O_Y, mGame->stats().pieces[Game::TETROMINO_O], TETROMINO_LENGTH, Game::COLOR_YELLOW);
            drawNumber(TETROMINO_X, TETROMINO_J_Y, mGame->stats().pieces[Game::TETROMINO_J], TETROMINO_LENGTH, Game::COLOR_BLUE);

            drawNumber(PIECES_X, PIECES_Y, mGame->stats().totalPieces, PIECES_LENGTH, Game::COLOR_WHITE);
        }

        // Inform the game that we are done with the changed state
        mGame->onChangeProcessed();
    }
}

long PlatformObjC::getSystemTime() {
    NSDate *now = [NSDate date];
    double seconds = [now timeIntervalSince1970];
    return 1000 * (seconds - mTimeStart);
}

void PlatformObjC::seedRandom(long seed) {
    srand(seed);
}

int PlatformObjC::random() {
    return rand();
}
}

