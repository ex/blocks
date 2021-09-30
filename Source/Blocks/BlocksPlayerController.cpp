// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlocksPlayerController.h"

ABlocksPlayerController::ABlocksPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}
