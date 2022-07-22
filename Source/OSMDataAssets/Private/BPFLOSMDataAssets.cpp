// Fill out your copyright notice in the Description page of Project Settings.


#include "BPFLOSMDataAssets.h"
#include "PolygonHelper.h"
#include "Algo/Reverse.h"

bool UBPFLOSMDataAssets::CheckAndRepairBuildingData(AGeoReferenceActor * GeoReference, FBuildingData &Building, float MinVertexDistance)
{
    return CheckFloorPlanVertexDistance(GeoReference, Building.PolygonPoints, MinVertexDistance) &&
        CheckFloorPlanWindingOrder(Building.PolygonPoints, false);
}

bool UBPFLOSMDataAssets::CheckAndRepairMPBuildingData(AGeoReferenceActor * GeoReference, FMPBuildingData &Building, float MinVertexDistance)
{
    bool ret = true;
    for(auto &Part : Building.Parts) {
        ret = CheckFloorPlanVertexDistance(GeoReference, Part.PolygonPoints, MinVertexDistance) &&
            CheckFloorPlanWindingOrder(Part.PolygonPoints, Part.bIsInner);
        if(!ret)
            return false;
    }
    return ret;
}

bool UBPFLOSMDataAssets::CheckFloorPlanVertexDistance(AGeoReferenceActor * GeoReference, TArray<FVector> &FloorPlan, float MinVertexDistance)
{
    TSet<int> RemovalCandidates;

    // find consecutive vertices with game distance smaller MinVertexDistance
    for(int i = 0; i < FloorPlan.Num(); i++){
        auto ThisGeo     = FloorPlan[i];
        auto NextGeo     = FloorPlan[(i+1)%(FloorPlan.Num()-1)];

        auto ThisOne = GeoReference->ToGameCoordinate(ThisGeo);
        auto NextOne = GeoReference->ToGameCoordinate(NextGeo);

        if(FVector::Distance(ThisOne, NextOne) < MinVertexDistance) {
            RemovalCandidates.Add(i+1);
        }
    }
    UE_LOG(LogTemp,Warning,TEXT("UBPFLOSMDataAssets: Removing %d polygon vertizes"), RemovalCandidates.Num())
    // Remove Candidates from floorplan
    for(auto i : RemovalCandidates) {
        FloorPlan.RemoveAt(i);
    }
    return true;
}

bool UBPFLOSMDataAssets::CheckFloorPlanWindingOrder(TArray<FVector> &FloorPlan, bool Inner)
{
    if(Inner){
        if(!PolygonHelper::IsClockwise(FloorPlan)){
            Algo::Reverse(FloorPlan);
        }
    } else {
        if(PolygonHelper::IsClockwise(FloorPlan)){
            Algo::Reverse(FloorPlan);
        }
    }
    return true;
}
