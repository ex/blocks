// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlocksBlockGrid.h"
#include "BlocksBlock.h"
#include "Components/TextRenderComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "stc/game.hpp"

#define LOCTEXT_NAMESPACE "PuzzleBlockGrid"

ABlocksBlockGrid* ABlocksBlockGrid::instance = nullptr;

ABlocksBlockGrid::ABlocksBlockGrid()
{
    instance = this;

	PrimaryActorTick.bCanEverTick = true;

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Create static mesh component
	ScoreText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ScoreText0"));
	ScoreText->SetRelativeLocation(FVector(200.f,0.f,0.f));
	ScoreText->SetRelativeRotation(FRotator(90.f,0.f,0.f));
	ScoreText->SetText(FText::Format(LOCTEXT("ScoreFmt", "Score: {0}"), FText::AsNumber(0)));
	ScoreText->SetupAttachment(DummyRoot);

    static ConstructorHelpers::FObjectFinder<USoundCue> DropSoundCueObject(TEXT("SoundCue'/Game/Audio/DropSoundCue.DropSoundCue'"));
    if (DropSoundCueObject.Succeeded())
    {
        DropSoundCue = DropSoundCueObject.Object;

        DropAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("DropAudioComponent"));
        DropAudioComponent->SetupAttachment(RootComponent);
    }
    static ConstructorHelpers::FObjectFinder<USoundCue> LineSoundCueObject(TEXT("SoundCue'/Game/Audio/LineSoundCue.LineSoundCue'"));
    if (LineSoundCueObject.Succeeded())
    {
        LineSoundCue = LineSoundCueObject.Object;

        LineAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("LineAudioComponent"));
        LineAudioComponent->SetupAttachment(RootComponent);
    }

	// Set defaults
	BlockSize = 1.f;
	BlockSpacing = 51.f;

	pGame = new stc::Game();
	pGame->init(this);
}


int ABlocksBlockGrid::init(stc::Game* game)
{
    return 0;
}

void ABlocksBlockGrid::BeginPlay()
{
	Super::BeginPlay();

    ScoreText->SetText(FText::Format(LOCTEXT("ScoreFmt", "Score: {0}\nLines: {1}\nLevel: {2}"), pGame->stats().score, pGame->stats().lines, pGame->stats().level));

    check(mTetromino.Num() == 0);
    for (int i = 0; i < stc::Game::TETROMINO_SIZE; ++i)
    {
        for (int j = 0; j < stc::Game::TETROMINO_SIZE; ++j)
        {
            mTetromino.Add(nullptr);
        }
    }
    check(mShadow.Num() == 0);
    for (int i = 0; i < stc::Game::TETROMINO_SIZE; ++i)
    {
        for (int j = 0; j < stc::Game::TETROMINO_SIZE; ++j)
        {
            mShadow.Add(nullptr);
        }
    }
    check(mNext.Num() == 0);
    for (int i = 0; i < stc::Game::TETROMINO_SIZE; ++i)
    {
        for (int j = 0; j < stc::Game::TETROMINO_SIZE; ++j)
        {
            mNext.Add(nullptr);
        }
    }
    check(mBoard.Num() == 0);
    for (int i = 0; i < stc::Game::BOARD_TILEMAP_WIDTH; ++i)
    {
        for (int j = 0; j < stc::Game::BOARD_TILEMAP_HEIGHT; ++j)
        {
            mBoard.Add(nullptr);
        }
    }

    if (DropAudioComponent && DropSoundCue)
    {
        DropAudioComponent->SetSound(DropSoundCue);
    }
    if (LineAudioComponent && LineSoundCue)
    {
        LineAudioComponent->SetSound(LineSoundCue);
    }
}

void ABlocksBlockGrid::end()
{
}

void ABlocksBlockGrid::processEvents()
{
}

void ABlocksBlockGrid::onLineCompleted()
{
    ScoreText->SetText(FText::Format(LOCTEXT("ScoreFmt", "Score: {0}\nLines: {1}\nLevel: {2}"), pGame->stats().score, pGame->stats().lines, pGame->stats().level));

    if (LineAudioComponent && LineSoundCue)
    {
        LineAudioComponent->Play();
    }
}

void ABlocksBlockGrid::onPieceDrop()
{
    if (DropAudioComponent && DropSoundCue)
    {
        DropAudioComponent->Play();
    }
}

void ABlocksBlockGrid::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    for (int i = 0; i < stc::Game::TETROMINO_SIZE; ++i)
    {
        for (int j = 0; j < stc::Game::TETROMINO_SIZE; ++j)
        {
            if (mTetromino[i * stc::Game::TETROMINO_SIZE + j] != nullptr)
            {
                GetWorld()->DestroyActor(mTetromino[i * stc::Game::TETROMINO_SIZE + j]);
            }
        }
    }
    mTetromino.Empty();
    for (int i = 0; i < stc::Game::TETROMINO_SIZE; ++i)
    {
        for (int j = 0; j < stc::Game::TETROMINO_SIZE; ++j)
        {
            if (mShadow[i * stc::Game::TETROMINO_SIZE + j] != nullptr)
            {
                GetWorld()->DestroyActor(mShadow[i * stc::Game::TETROMINO_SIZE + j]);
            }
        }
    }
    mShadow.Empty();
    for (int i = 0; i < stc::Game::TETROMINO_SIZE; ++i)
    {
        for (int j = 0; j < stc::Game::TETROMINO_SIZE; ++j)
        {
            if (mNext[i * stc::Game::TETROMINO_SIZE + j] != nullptr)
            {
                GetWorld()->DestroyActor(mNext[i * stc::Game::TETROMINO_SIZE + j]);
            }
        }
    }
    mNext.Empty();
    for (int i = 0; i < stc::Game::BOARD_TILEMAP_WIDTH; ++i)
    {
        for (int j = 0; j < stc::Game::BOARD_TILEMAP_HEIGHT; ++j)
        {
            if (mBoard[j + i * stc::Game::BOARD_TILEMAP_HEIGHT] != nullptr)
            {
                GetWorld()->DestroyActor(mBoard[j + i * stc::Game::BOARD_TILEMAP_HEIGHT]);
            }
        }
    }
    mBoard.Empty();
}

void ABlocksBlockGrid::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	pGame->update();
}

// Return the current system time in milliseconds
long ABlocksBlockGrid::getSystemTime()
{
	if (GetWorld())
	{
		return long(1000 * GetWorld()->GetUnpausedTimeSeconds());
	}
	return -1;
}

void ABlocksBlockGrid::onTetrominoMoved()
{
#if 0
	if (GEngine)
	{
		FString text = FString(pGame->getDebugString().c_str());
		GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Red, text);
	}
#endif
}

// Return a random positive integer number
int ABlocksBlockGrid::random()
{
	return FMath::Rand();
}

ABlocksBlock* ABlocksBlockGrid::getBlock(int x, int y, int idTile, bool shadow)
{
    // Calculate offset postion (UE coordinate system is XY reverse)
    const float XOffset = BlockSpacing * y;
    const float YOffset = BlockSpacing * x;
    const FVector BlockLocation = FVector(XOffset, YOffset, 0.f) + GetActorLocation();

    // Spawn a block
    ABlocksBlock* NewBlock = GetWorld()->SpawnActor<ABlocksBlock>(BlockLocation, FRotator(0, 0, 0));
    NewBlock->SetActorScale3D(FVector(BlockSize, BlockSize, BlockSize));
    if (shadow)
    {
        NewBlock->SetShadowColor(idTile);
    }
    else
    {
        NewBlock->SetColor(idTile);
    }
    return NewBlock;
}

void ABlocksBlockGrid::renderGame()
{
    const int BOARD_X = 0;
    const int BOARD_Y = 13;
    const int PREVIEW_X = 16;
    const int PREVIEW_Y = 3;

    // Check if the falling tetromino has moved, if so redraw.
    if (pGame->hasMove())
    {
        // Draw falling tetromino
        for (int i = 0; i < stc::Game::TETROMINO_SIZE; ++i)
        {
            for (int j = 0; j < stc::Game::TETROMINO_SIZE; ++j)
            {
                if (mTetromino[i * stc::Game::TETROMINO_SIZE + j] != nullptr)
                {
                    GetWorld()->DestroyActor(mTetromino[i * stc::Game::TETROMINO_SIZE + j]);
                    mTetromino[i * stc::Game::TETROMINO_SIZE + j] = nullptr;
                }
                if (pGame->fallingBlock().cells[i][j] != stc::Game::EMPTY_CELL )
                {
                    mTetromino[i * stc::Game::TETROMINO_SIZE + j] = getBlock(BOARD_X + pGame->fallingBlock().x + i,
                                                                             BOARD_Y - pGame->fallingBlock().y - j,
                                                                             pGame->fallingBlock().cells[i][j]);
                }
            }
        }

        // Draw shadow tetromino
        if (pGame->showShadow() && (pGame->shadowGap() >= 0))
        {
            for (int i = 0; i < stc::Game::TETROMINO_SIZE; ++i)
            {
                for (int j = 0; j < stc::Game::TETROMINO_SIZE; ++j)
                {
                    if (mShadow[i * stc::Game::TETROMINO_SIZE + j] != nullptr)
                    {
                        GetWorld()->DestroyActor(mShadow[i * stc::Game::TETROMINO_SIZE + j]);
                        mShadow[i * stc::Game::TETROMINO_SIZE + j] = nullptr;
                    }
                    if (pGame->fallingBlock().cells[i][j] != stc::Game::EMPTY_CELL)
                    {
                        mShadow[i * stc::Game::TETROMINO_SIZE + j] = getBlock(BOARD_X + pGame->fallingBlock().x + i,
                                                                              BOARD_Y - pGame->fallingBlock().y - pGame->shadowGap() - j,
                                                                              pGame->fallingBlock().cells[i][j], true);
                    }
                }
            }
        }

        // Inform to the game that we are done with the move.
        pGame->onMoveProcessed();
    }

    // Check if the game state has changed, if so redraw.
    if (pGame->hasChanged())
    {
        // Draw preview block
        if (pGame->showPreview())
        {
            for (int i = 0; i < stc::Game::TETROMINO_SIZE; ++i)
            {
                for (int j = 0; j < stc::Game::TETROMINO_SIZE; ++j)
                {
                    if (mNext[i * stc::Game::TETROMINO_SIZE + j] != nullptr)
                    {
                        GetWorld()->DestroyActor(mNext[i * stc::Game::TETROMINO_SIZE + j]);
                        mNext[i * stc::Game::TETROMINO_SIZE + j] = nullptr;
                    }
                    if (pGame->nextBlock().cells[i][j] != stc::Game::EMPTY_CELL)
                    {
                        mNext[i * stc::Game::TETROMINO_SIZE + j] = getBlock(PREVIEW_X + i,
                                                                            PREVIEW_Y - j,
                                                                            pGame->nextBlock().cells[i][j]);
                    }
                }
            }
        }
        // Draw the cells in the board
        for (int i = 0; i < stc::Game::BOARD_TILEMAP_WIDTH; ++i)
        {
            for (int j = 0; j < stc::Game::BOARD_TILEMAP_HEIGHT; ++j)
            {
                if (pGame->getCell(i, j) == stc::Game::EMPTY_CELL)
                {
                    if (mBoard[j + i * stc::Game::BOARD_TILEMAP_HEIGHT] != nullptr)
                    {
                        GetWorld()->DestroyActor(mBoard[j + i * stc::Game::BOARD_TILEMAP_HEIGHT]);
                        mBoard[j + i * stc::Game::BOARD_TILEMAP_HEIGHT] = nullptr;
                    }
                }
                else
                {
                    if (mBoard[j + i * stc::Game::BOARD_TILEMAP_HEIGHT] == nullptr)
                    {
                        mBoard[j + i * stc::Game::BOARD_TILEMAP_HEIGHT] = getBlock(BOARD_X + i, BOARD_Y - j, pGame->getCell(i, j));
                    }
                    else
                    {
                        //if (!m_board[i, j].CompareTag(m_game.GetCell(i, j).ToString()))
                        //{
                            GetWorld()->DestroyActor(mBoard[j + i * stc::Game::BOARD_TILEMAP_HEIGHT]);
                            mBoard[j + i * stc::Game::BOARD_TILEMAP_HEIGHT] = getBlock(BOARD_X + i, BOARD_Y - j, pGame->getCell(i, j));
                            //m_board[i, j].tag = m_game.GetCell(i, j).ToString();
                            //GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Red, "replace");
                        //}
                    }
                }
            }
        }

        // Inform to the game that we are done with the changed state.
        pGame->onChangeProcessed();
    }
}

#undef LOCTEXT_NAMESPACE
