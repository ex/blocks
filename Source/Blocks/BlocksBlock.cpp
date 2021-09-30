// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlocksBlock.h"
#include "BlocksBlockGrid.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstance.h"

#include "stc/game.hpp"

ABlocksBlock::ABlocksBlock()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> BlueMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> CyanMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> RedMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> YellowMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> GreenMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> PurpleMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> OrangeMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> ShadowBlueMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> ShadowCyanMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> ShadowRedMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> ShadowYellowMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> ShadowGreenMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> ShadowPurpleMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> ShadowOrangeMaterial;

		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/PuzzleCube.PuzzleCube"))
			, BlueMaterial(TEXT("/Game/Puzzle/Meshes/BlueMaterial.BlueMaterial"))
			, CyanMaterial(TEXT("/Game/Puzzle/Meshes/CyanMaterial.CyanMaterial"))
			, RedMaterial(TEXT("/Game/Puzzle/Meshes/RedMaterial.RedMaterial"))
			, YellowMaterial(TEXT("/Game/Puzzle/Meshes/YellowMaterial.YellowMaterial"))
			, GreenMaterial(TEXT("/Game/Puzzle/Meshes/GreenMaterial.GreenMaterial"))
			, PurpleMaterial(TEXT("/Game/Puzzle/Meshes/PurpleMaterial.PurpleMaterial"))
			, OrangeMaterial(TEXT("/Game/Puzzle/Meshes/OrangeMaterial.OrangeMaterial"))
			, ShadowBlueMaterial(TEXT("/Game/Puzzle/Meshes/ShadowBlueMaterial.ShadowBlueMaterial"))
			, ShadowCyanMaterial(TEXT("/Game/Puzzle/Meshes/ShadowCyanMaterial.ShadowCyanMaterial"))
			, ShadowRedMaterial(TEXT("/Game/Puzzle/Meshes/ShadowRedMaterial.ShadowRedMaterial"))
			, ShadowYellowMaterial(TEXT("/Game/Puzzle/Meshes/ShadowYellowMaterial.ShadowYellowMaterial"))
			, ShadowGreenMaterial(TEXT("/Game/Puzzle/Meshes/ShadowGreenMaterial.ShadowGreenMaterial"))
			, ShadowPurpleMaterial(TEXT("/Game/Puzzle/Meshes/ShadowPurpleMaterial.ShadowPurpleMaterial"))
			, ShadowOrangeMaterial(TEXT("/Game/Puzzle/Meshes/ShadowOrangeMaterial.ShadowOrangeMaterial"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Create static mesh component
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	BlockMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	BlockMesh->SetRelativeScale3D(FVector(0.2f,0.2f,0.25f));
	BlockMesh->SetRelativeLocation(FVector(0.f,0.f,25.f));
	BlockMesh->SetMaterial(0, ConstructorStatics.BlueMaterial.Get());
	BlockMesh->SetupAttachment(DummyRoot);

	// Save a pointer to the materials
	BlueMaterial = ConstructorStatics.BlueMaterial.Get();
	CyanMaterial = ConstructorStatics.CyanMaterial.Get();
	RedMaterial = ConstructorStatics.RedMaterial.Get();
	YellowMaterial = ConstructorStatics.YellowMaterial.Get();
	PurpleMaterial = ConstructorStatics.PurpleMaterial.Get();
	GreenMaterial = ConstructorStatics.GreenMaterial.Get();
	OrangeMaterial = ConstructorStatics.OrangeMaterial.Get();
	ShadowBlueMaterial = ConstructorStatics.ShadowBlueMaterial.Get();
	ShadowCyanMaterial = ConstructorStatics.ShadowCyanMaterial.Get();
	ShadowRedMaterial = ConstructorStatics.ShadowRedMaterial.Get();
	ShadowYellowMaterial = ConstructorStatics.ShadowYellowMaterial.Get();
	ShadowPurpleMaterial = ConstructorStatics.ShadowPurpleMaterial.Get();
	ShadowGreenMaterial = ConstructorStatics.ShadowGreenMaterial.Get();
	ShadowOrangeMaterial = ConstructorStatics.ShadowOrangeMaterial.Get();
}

void ABlocksBlock::SetColor(int color)
{
	// Change material
	switch (color)
	{
	case stc::Game::COLOR_BLUE:
		BlockMesh->SetMaterial(0, BlueMaterial);
		break;
	case stc::Game::COLOR_CYAN:
		BlockMesh->SetMaterial(0, CyanMaterial);
		break;
	case stc::Game::COLOR_GREEN:
		BlockMesh->SetMaterial(0, GreenMaterial);
		break;
	case stc::Game::COLOR_ORANGE:
		BlockMesh->SetMaterial(0, OrangeMaterial);
		break;
	case stc::Game::COLOR_PURPLE:
		BlockMesh->SetMaterial(0, PurpleMaterial);
		break;
	case stc::Game::COLOR_RED:
		BlockMesh->SetMaterial(0, RedMaterial);
		break;
	case stc::Game::COLOR_YELLOW:
		BlockMesh->SetMaterial(0, YellowMaterial);
		break;
	}
}

void ABlocksBlock::SetShadowColor(int color)
{
	// Change material
	switch (color)
	{
	case stc::Game::COLOR_BLUE:
		BlockMesh->SetMaterial(0, ShadowBlueMaterial);
		break;
	case stc::Game::COLOR_CYAN:
		BlockMesh->SetMaterial(0, ShadowCyanMaterial);
		break;
	case stc::Game::COLOR_GREEN:
		BlockMesh->SetMaterial(0, ShadowGreenMaterial);
		break;
	case stc::Game::COLOR_ORANGE:
		BlockMesh->SetMaterial(0, ShadowOrangeMaterial);
		break;
	case stc::Game::COLOR_PURPLE:
		BlockMesh->SetMaterial(0, ShadowPurpleMaterial);
		break;
	case stc::Game::COLOR_RED:
		BlockMesh->SetMaterial(0, ShadowRedMaterial);
		break;
	case stc::Game::COLOR_YELLOW:
		BlockMesh->SetMaterial(0, ShadowYellowMaterial);
		break;
	}
}
