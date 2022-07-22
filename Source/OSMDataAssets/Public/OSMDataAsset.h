// Copyright (c) Iwer Petersen. All rights reserved.
#pragma once
#include "Enums.h"
#include "OSMDataAsset.generated.h"


USTRUCT(BlueprintType)
struct FBuildingData {
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ID;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FVector> PolygonPoints;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<EOSMBuildingType> BuildingType;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Height;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Levels;
    FBuildingData() {
        ID = 0;
        BuildingType = EOSMBuildingType::OtherBuilding;
        Height = 0;
        Levels = 0;
    }
};

USTRUCT(BlueprintType)
struct FMPBuildingPart {
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    uint8 bIsInner : 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FVector> PolygonPoints;
    FMPBuildingPart() {
        bIsInner = 0;
    }
};

USTRUCT(BlueprintType)
struct FMPBuildingData {
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ID;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FMPBuildingPart> Parts;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    uint8 bHasHole : 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<EOSMBuildingType> BuildingType;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Height;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Levels;
    FMPBuildingData() {
        ID = 0;
        bHasHole = 0;
        BuildingType = EOSMBuildingType::OtherBuilding;
        Height = 0;
        Levels = 0;
    }
};


UCLASS(BlueprintType, hidecategories=(Object))
class OSMDATAASSETS_API UOSMDataAsset : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly,EditAnywhere)
    TArray<FMPBuildingData> MultiPolygonBuildings;
    UPROPERTY(BlueprintReadOnly,EditAnywhere)
    TArray<FBuildingData> Buildings;
};
