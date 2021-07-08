// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlocksGameMode.h"
#include "BlocksPlayerController.h"
#include "BlocksPawn.h"

ABlocksGameMode::ABlocksGameMode()
{
	// no pawn by default
	DefaultPawnClass = ABlocksPawn::StaticClass();
	// use our own player controller class
	PlayerControllerClass = ABlocksPlayerController::StaticClass();
}
