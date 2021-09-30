// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "stc/platform.hpp"
#include "BlocksBlock.h"
#include "Components/Audiocomponent.h"
#include "Sound/SoundCue.h"
#include "BlocksBlockGrid.generated.h"

/** Class used to spawn blocks and manage score */
UCLASS(minimalapi)
class ABlocksBlockGrid : public AActor, public stc::Platform
{
	GENERATED_BODY()

	/** Dummy root component */
	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	/** Text component for the score */
	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* ScoreText;

public:
	ABlocksBlockGrid();

	/** Number of blocks along each side of grid */
	UPROPERTY(Category=Grid, EditAnywhere, BlueprintReadOnly)
	float BlockSize;

	/** Spacing of blocks */
	UPROPERTY(Category=Grid, EditAnywhere, BlueprintReadOnly)
	float BlockSpacing;

	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadOnly)
	class USoundCue* DropSoundCue;

	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadOnly)
	class USoundCue* LineSoundCue;

protected:
	// Begin AActor interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// End AActor interface

	ABlocksBlock* getBlock(int x, int y, int idTile, bool shadow = false);

	UAudioComponent* DropAudioComponent;
	UAudioComponent* LineAudioComponent;

	stc::Game* pGame;
	TArray<ABlocksBlock*> mTetromino;
	TArray<ABlocksBlock*> mShadow;
	TArray<ABlocksBlock*> mBoard;
	TArray<ABlocksBlock*> mNext;

public:

	virtual void Tick(float DeltaSeconds) override;

	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
	/** Returns ScoreText subobject **/
	FORCEINLINE class UTextRenderComponent* GetScoreText() const { return ScoreText; }

	static ABlocksBlockGrid* instance;

public:
	// Initializes platform
	virtual int init(stc::Game* game) override;

	// Clear resources used by platform
	virtual void end() override;

	// Process events and notify game
	virtual void processEvents() override;

	// Render the state of the game
	virtual void renderGame() override;

	// Return the current system time in milliseconds
	virtual long getSystemTime() override;

	// Return a random positive integer number
	virtual int random() override;

	// Events
	virtual void onLineCompleted() override;
	virtual void onPieceDrop() override;
	virtual void onTetrominoMoved() override;

	stc::Game* getGame() { return pGame; }
};



