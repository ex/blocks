// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SOverlay.h"

class FBlocksGameOverOverlay
{
public:
    void Show(UGameViewportClient* Viewport);
    void Hide(UGameViewportClient* Viewport);

private:
    TSharedPtr<SWidget> OverlayWidget;
};
