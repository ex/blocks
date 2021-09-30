// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BlocksPawn.generated.h"

class ABlocksBlockGrid;

UCLASS(config=Game)
class ABlocksPawn : public APawn
{
	GENERATED_UCLASS_BODY()

public:

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	ABlocksBlockGrid* pBlocksBlockGrid;

protected:

	void OnKeyLeftPress();
	void OnKeyLeftRelease();
	void OnKeyRightPress();
	void OnKeyRightRelease();
	void OnKeyUpPress();
	void OnKeyUpRelease();
	void OnKeyDownPress();
	void OnKeyDownRelease();
	void OnKeyDropPress();

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	class ABlocksBlock* CurrentBlockFocus;
};
