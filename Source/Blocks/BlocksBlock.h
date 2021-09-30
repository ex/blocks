// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlocksBlock.generated.h"

/** A block that can be clicked */
UCLASS(minimalapi)
class ABlocksBlock : public AActor
{
	GENERATED_BODY()

	/** Dummy root component */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	/** StaticMesh component for the clickable block */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BlockMesh;

public:
	ABlocksBlock();

	/** Are we currently active? */
	bool bIsActive;

	/** Pointers to color materials used for blocks */
	UPROPERTY()
	class UMaterialInstance* BlueMaterial;
	UPROPERTY()
	class UMaterialInstance* CyanMaterial;
	UPROPERTY()
	class UMaterialInstance* RedMaterial;
	UPROPERTY()
	class UMaterialInstance* GreenMaterial;
	UPROPERTY()
	class UMaterialInstance* YellowMaterial;
	UPROPERTY()
	class UMaterialInstance* PurpleMaterial;
	UPROPERTY()
	class UMaterialInstance* OrangeMaterial;
	UPROPERTY()
	class UMaterialInstance* ShadowBlueMaterial;
	UPROPERTY()
	class UMaterialInstance* ShadowCyanMaterial;
	UPROPERTY()
	class UMaterialInstance* ShadowRedMaterial;
	UPROPERTY()
	class UMaterialInstance* ShadowGreenMaterial;
	UPROPERTY()
	class UMaterialInstance* ShadowYellowMaterial;
	UPROPERTY()
	class UMaterialInstance* ShadowPurpleMaterial;
	UPROPERTY()
	class UMaterialInstance* ShadowOrangeMaterial;

	void SetColor(int color);
	void SetShadowColor(int color);

public:
	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
	/** Returns BlockMesh subobject **/
	FORCEINLINE class UStaticMeshComponent* GetBlockMesh() const { return BlockMesh; }
};



