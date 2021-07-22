// Copyright (c) Iwer Petersen. All rights reserved.
#include "OSMDataAssetFactory.h"

#include "GeoCoordinate.h"
#include "OSMDataAsset.h"
#include "OSMFileParser.h"
UOSMDataAssetFactory::UOSMDataAssetFactory( const FObjectInitializer& ObjectInitializer )
	: Super(ObjectInitializer)
{
    SupportedClass = UOSMDataAsset::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
    Formats.Add(TEXT("osm;OSM File"));
}

UObject * UOSMDataAssetFactory::FactoryCreateFile(UClass* InClass,
                                                      UObject* InParent,
                                                      FName InName,
                                                      EObjectFlags Flags,
                                                      const FString& Filename,
                                                      const TCHAR* Parms,
                                                      FFeedbackContext* Warn,
                                                      bool& bOutOperationCanceled)
{
    UOSMDataAsset* Asset = NewObject<UOSMDataAsset>(InParent, InClass, InName, Flags);

    FString file = Filename;
    FOSMFile parser;
    if(parser.LoadOpenStreetMapFile(file, false, Warn))
    {
        UE_LOG(LogTemp, Warning, TEXT("UOSMDataAssetFactory: %d Ways"), parser.Ways.Num())
        for(auto way : parser.Ways) {
            if (way) {
                FBuildingData building;
                building.BuildingType = way->BuildingType;
                building.Height = way->Height;
                building.Levels = way->Levels;
                UE_LOG(LogTemp, Warning, TEXT("UOSMDataAssetFactory: %d Nodes"), way->Nodes.Num())
                
                //for(FOSMFile::FOSMNodeInfo* node : way->Nodes){
                for(int i = 0; i < way->Nodes.Num(); i++) {
                    double lat = way->Nodes[i]->Latitude;
                    double lon = way->Nodes[i]->Longitude;

                    FVector location = FVector(lon, lat, 0);

                    // sometimes shapes are closed of with the first point, we dont need that
                    if(i == way->Nodes.Num() - 1
                        && location.Equals(building.PolygonPoints[0]))
                    {
                        break;
                    }
                    building.PolygonPoints.Add(location);
                }
                Asset->Buildings.Add(building);
            } else
            {
                UE_LOG(LogTemp, Warning, TEXT("UOSMDataAssetFactory: Invalid Way"))
            }
        }
    } else
    {
        UE_LOG(LogTemp, Error, TEXT("UOSMDataAssetFactory: Failed to parse osm file %s"), *file)
    }

    return Asset;
}

bool UOSMDataAssetFactory::FactoryCanImport(const FString & Filename)
{

    return true;
}
