// Copyright (c) Iwer Petersen. All rights reserved.
#pragma once
#include "Enums.h"
#include "OSMDataAsset.generated.h"


USTRUCT(BlueprintType)
struct FBuildingData {
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> PolygonPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EOSMBuildingType> BuildingType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Height;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Levels;
};

UCLASS(BlueprintType, hidecategories=(Object))
class OSMDATAASSETS_API UOSMDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly,EditAnywhere)
	TArray<FBuildingData> Buildings;
};
