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

    FString File = Filename;
    FOSMFile Parser;
    if(Parser.LoadOpenStreetMapFile(File, false, Warn))
    {
        UE_LOG(LogTemp, Warning, TEXT("UOSMDataAssetFactory: %d Relations"), Parser.Relations.Num())
        for(const auto Rel : Parser.Relations) {
            FMPBuildingData Building;
            Building.ID = Rel->RelationID;
            Building.BuildingType = Rel->BuildingType;
            Building.Levels = Rel->BuildingLevels;
            Building.Height = Rel->Height;
            Building.bHasHole = 0;
            UE_LOG(LogTemp,Warning,TEXT("UOSMDataAssetFactory: %d MPolygons"), Rel->Members.Num())
            for (auto m : Rel->Members){
                FMPBuildingPart Part;
                Part.bIsInner = m->bIsInner;
                if(Part.bIsInner==1)
                    Building.bHasHole=1;
                auto WayID = m->Ref;
                FOSMFile::FOSMWayInfo * Way = Parser.WayMap.FindRef(WayID);
                UE_LOG(LogTemp, Warning, TEXT("UOSMDataAssetFactory: %d Nodes"), Way->Nodes.Num())
                for(int i = 0; i < Way->Nodes.Num(); i++) {
                    const double Lat = Way->Nodes[i]->Latitude;
                    const double Lon = Way->Nodes[i]->Longitude;

                    FVector Location = FVector(Lon, Lat, 0);

                    // sometimes shapes are closed of with the first point, we dont need that
                    if(i == Way->Nodes.Num() - 1
                        && Location.Equals(Part.PolygonPoints[0]))
                    {
                        break;
                    }
                    Part.PolygonPoints.Add(Location);
                }
                Building.Parts.Add(Part);
                Parser.WayMap.Remove(WayID);
                Parser.Ways.Remove(Way);
            }
            Asset->MultiPolygonBuildings.Add(Building);
        }

        UE_LOG(LogTemp, Warning, TEXT("UOSMDataAssetFactory: %d Ways"), Parser.Ways.Num())
        for(const auto Way : Parser.Ways) {
            if (Way) {
                FBuildingData Building;
                Building.ID = Way->WayID;
                Building.BuildingType = Way->BuildingType;
                Building.Height = Way->Height;
                Building.Levels = Way->Levels;
                UE_LOG(LogTemp, Warning, TEXT("UOSMDataAssetFactory: %d Nodes"), Way->Nodes.Num())

                //for(FOSMFile::FOSMNodeInfo* node : way->Nodes){
                for(int i = 0; i < Way->Nodes.Num(); i++) {
                    const double Lat = Way->Nodes[i]->Latitude;
                    const double Lon = Way->Nodes[i]->Longitude;

                    FVector Location = FVector(Lon, Lat, 0);

                    // sometimes shapes are closed of with the first point, we dont need that
                    if(i == Way->Nodes.Num() - 1
                        && Location.Equals(Building.PolygonPoints[0]))
                    {
                        break;
                    }
                    Building.PolygonPoints.Add(Location);
                }
                Asset->Buildings.Add(Building);
            } else
            {
                UE_LOG(LogTemp, Warning, TEXT("UOSMDataAssetFactory: Invalid Way"))
            }
        }
    } else
    {
        UE_LOG(LogTemp, Error, TEXT("UOSMDataAssetFactory: Failed to parse osm file %s"), *File)
    }

    return Asset;
}

bool UOSMDataAssetFactory::FactoryCanImport(const FString & Filename)
{

    return true;
}
