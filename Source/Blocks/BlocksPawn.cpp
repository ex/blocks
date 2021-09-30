// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlocksPawn.h"
#include "BlocksBlock.h"
#include "BlocksBlockGrid.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

#include "stc/game.hpp"

ABlocksPawn::ABlocksPawn(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	pBlocksBlockGrid = nullptr;
}

void ABlocksPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ABlocksPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("OnLeft", IE_Pressed, this, &ABlocksPawn::OnKeyLeftPress);
	PlayerInputComponent->BindAction("OnLeft", IE_Released, this, &ABlocksPawn::OnKeyLeftRelease);

	PlayerInputComponent->BindAction("OnRight", IE_Pressed, this, &ABlocksPawn::OnKeyRightPress);
	PlayerInputComponent->BindAction("OnRight", IE_Released, this, &ABlocksPawn::OnKeyRightRelease);

	PlayerInputComponent->BindAction("OnRotate", IE_Pressed, this, &ABlocksPawn::OnKeyUpPress);
	PlayerInputComponent->BindAction("OnRotate", IE_Released, this, &ABlocksPawn::OnKeyUpRelease);

	PlayerInputComponent->BindAction("OnDown", IE_Pressed, this, &ABlocksPawn::OnKeyDownPress);
	PlayerInputComponent->BindAction("OnDown", IE_Released, this, &ABlocksPawn::OnKeyDownRelease);

	PlayerInputComponent->BindAction("OnDrop", IE_Pressed, this, &ABlocksPawn::OnKeyDropPress);
}

void ABlocksPawn::OnKeyLeftPress()
{
	ABlocksBlockGrid::instance->getGame()->onEventStart(stc::Game::EVENT_MOVE_LEFT);
}
void ABlocksPawn::OnKeyLeftRelease()
{
	ABlocksBlockGrid::instance->getGame()->onEventEnd(stc::Game::EVENT_MOVE_LEFT);
}
void ABlocksPawn::OnKeyRightPress()
{
	ABlocksBlockGrid::instance->getGame()->onEventStart(stc::Game::EVENT_MOVE_RIGHT);
}
void ABlocksPawn::OnKeyRightRelease()
{
	ABlocksBlockGrid::instance->getGame()->onEventEnd(stc::Game::EVENT_MOVE_RIGHT);
}
void ABlocksPawn::OnKeyUpPress()
{
	ABlocksBlockGrid::instance->getGame()->onEventStart(stc::Game::EVENT_ROTATE_CW);
}
void ABlocksPawn::OnKeyUpRelease()
{
	ABlocksBlockGrid::instance->getGame()->onEventEnd(stc::Game::EVENT_ROTATE_CW);
}
void ABlocksPawn::OnKeyDownPress()
{
	ABlocksBlockGrid::instance->getGame()->onEventStart(stc::Game::EVENT_MOVE_DOWN);
}
void ABlocksPawn::OnKeyDownRelease()
{
	ABlocksBlockGrid::instance->getGame()->onEventEnd(stc::Game::EVENT_MOVE_DOWN);
}
void ABlocksPawn::OnKeyDropPress()
{
	ABlocksBlockGrid::instance->getGame()->onEventStart(stc::Game::EVENT_DROP);
}

