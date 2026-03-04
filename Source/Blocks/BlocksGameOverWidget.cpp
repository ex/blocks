// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlocksGameOverWidget.h"
#include "Engine/GameViewportClient.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"

void FBlocksGameOverOverlay::Show(UGameViewportClient* Viewport)
{
    if (!Viewport || OverlayWidget.IsValid())
    {
        return;
    }

    OverlayWidget = SNew(SOverlay)
        + SOverlay::Slot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        [
            SNew(SImage)
            .ColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.65f))
        ]
        + SOverlay::Slot()
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("GAME OVER")))
            .Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(), 72))
            .ColorAndOpacity(FLinearColor::Red)
        ];

    Viewport->AddViewportWidgetContent(OverlayWidget.ToSharedRef(), 100);
}

void FBlocksGameOverOverlay::Hide(UGameViewportClient* Viewport)
{
    if (Viewport && OverlayWidget.IsValid())
    {
        Viewport->RemoveViewportWidgetContent(OverlayWidget.ToSharedRef());
        OverlayWidget.Reset();
    }
}
