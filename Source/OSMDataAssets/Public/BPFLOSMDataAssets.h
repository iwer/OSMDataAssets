// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OSMDataAsset.h"
#include "GeoReferenceActor.h"

#include "BPFLOSMDataAssets.generated.h"

/**
 *
 */
UCLASS()
class OSMDATAASSETS_API UBPFLOSMDataAssets : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    /**
     * Checks FBuildingData for inconsistencies and repairs them when possible.
     * Returns false if repairs where not possible.
     */
    UFUNCTION(BlueprintCallable, Category="OSMDataAssets|Building")
    static bool CheckAndRepairBuildingData(UPARAM(ref) AGeoReferenceActor* GeoReference, UPARAM(ref) FBuildingData &Building, float MinVertexDistance);

    /**
     * Checks FMPBuildingData for inconsistencies and repairs them when possible
     * Returns false if repairs where not possible.
     */
    UFUNCTION(BlueprintCallable, Category="OSMDataAssets|Building")
    static bool CheckAndRepairMPBuildingData(UPARAM(ref) AGeoReferenceActor* GeoReference, UPARAM(ref) FMPBuildingData &Building, float MinVertexDistance);

private:
    static bool CheckFloorPlanVertexDistance(AGeoReferenceActor* GeoReference, TArray<FVector> &FloorPlan, float MinVertexDistance);
    static bool CheckFloorPlanWindingOrder(TArray<FVector> &FloorPlan, bool Inner);
};
