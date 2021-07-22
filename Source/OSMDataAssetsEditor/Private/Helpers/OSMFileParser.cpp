// Copyright 2017 Mike Fricker. All Rights Reserved.

#include "OSMFileParser.h"


FOSMFile::FOSMFile()
        : ParsingState(ParsingState::Root) {
}


FOSMFile::~FOSMFile() {
    // Clean up time
    {
        for (auto * Way : Ways) {
            delete Way;
        }
        Ways.Empty();

        for (auto HashPair : NodeMap) {
            FOSMNodeInfo * NodeInfo = HashPair.Value;
            delete NodeInfo;
        }
        NodeMap.Empty();
    }
}


bool FOSMFile::LoadOpenStreetMapFile(FString &OSMFilePath, const bool bIsFilePathActuallyTextBuffer,
                                     FFeedbackContext * FeedbackContext) {
    const bool bShowSlowTaskDialog = true;
    const bool bShowCancelButton = true;

    FText ErrorMessage;
    int32 ErrorLineNumber;
    bool success = FFastXml::ParseXmlFile(
            this,
            bIsFilePathActuallyTextBuffer ? nullptr : *OSMFilePath,
            bIsFilePathActuallyTextBuffer ? OSMFilePath.GetCharArray().GetData() : nullptr,
            FeedbackContext,
            bShowSlowTaskDialog,
            bShowCancelButton,
            /* Out */ ErrorMessage,
            /* Out */ ErrorLineNumber);
    if (success) {
        if (NodeMap.Num() > 0) {
            AverageLatitude /= NodeMap.Num();
            AverageLongitude /= NodeMap.Num();
        }

        return success;
    }

    if (FeedbackContext != nullptr) {
        FeedbackContext->Logf(
                ELogVerbosity::Error,
                TEXT("Failed to load OpenStreetMap XML file ('%s', Line %i)"),
                *ErrorMessage.ToString(),
                ErrorLineNumber);
    }

    return false;
}


bool FOSMFile::ProcessXmlDeclaration(const TCHAR * ElementData, int32 XmlFileLineNumber) {
    // Don't care about XML declaration
    return true;
}


bool FOSMFile::ProcessComment(const TCHAR * Comment) {
    // Don't care about comments
    return true;
}


bool FOSMFile::ProcessElement(const TCHAR * ElementName, const TCHAR * ElementData, int32 XmlFileLineNumber) {
    if (ParsingState == ParsingState::Root) {
        if (!FCString::Stricmp(ElementName, TEXT("node"))) {
            ParsingState = ParsingState::Node;
            CurrentNodeInfo = new FOSMNodeInfo();
            CurrentNodeInfo->Latitude = 0.0;
            CurrentNodeInfo->Longitude = 0.0;
        } else if (!FCString::Stricmp(ElementName, TEXT("way"))) {
            ParsingState = ParsingState::Way;
            CurrentWayInfo = new FOSMWayInfo();
            CurrentWayInfo->Name.Empty();
            CurrentWayInfo->Ref.Empty();
            CurrentWayInfo->WayType = EOSMWayType::OtherRoad;
            CurrentWayInfo->BuildingType = EOSMBuildingType::OtherBuilding;
            CurrentWayInfo->Height = 0.0;
            CurrentWayInfo->bIsOneWay = false;
            CurrentWayInfo->bIsBridge = false;
            CurrentWayInfo->Layer = 0;
            CurrentWayInfo->Lanes = 1;
            CurrentWayInfo->Levels = 0;
            // @todo: We're currently ignoring the "visible" tag on ways, which means that roads will always
            //        be included in our data set.  It might be nice to make this an import option.
        }
    } else if (ParsingState == ParsingState::Way) {
        if (!FCString::Stricmp(ElementName, TEXT("nd"))) {
            ParsingState = ParsingState::Way_NodeRef;
        } else if (!FCString::Stricmp(ElementName, TEXT("tag"))) {
            ParsingState = ParsingState::Way_Tag;
        }
    }

    return true;
}


bool FOSMFile::ProcessAttribute(const TCHAR * AttributeName, const TCHAR * AttributeValue) {
    //UE_LOG(LogTemp, Warning, TEXT("FOSMFile: ProcessAttribute(%s, %s)"), AttributeName, AttributeValue);

    if (ParsingState == ParsingState::Node) {
        if (!FCString::Stricmp(AttributeName, TEXT("id"))) {
            CurrentNodeID = FPlatformString::Atoi64(AttributeValue);
        } else if (!FCString::Stricmp(AttributeName, TEXT("lat"))) {
            CurrentNodeInfo->Latitude = FPlatformString::Atod(AttributeValue);

            AverageLatitude += CurrentNodeInfo->Latitude;

            // Update minimum and maximum latitude
            // @todo: Performance: Instead of computing our own bounding box, we could parse the "minlat" and
            //        "minlon" tags from the OSM file
            if (CurrentNodeInfo->Latitude < MinLatitude) {
                MinLatitude = CurrentNodeInfo->Latitude;
            }
            if (CurrentNodeInfo->Latitude > MaxLatitude) {
                MaxLatitude = CurrentNodeInfo->Latitude;
            }
        } else if (!FCString::Stricmp(AttributeName, TEXT("lon"))) {
            CurrentNodeInfo->Longitude = FPlatformString::Atod(AttributeValue);

            AverageLongitude += CurrentNodeInfo->Longitude;

            // Update minimum and maximum longitude
            if (CurrentNodeInfo->Longitude < MinLongitude) {
                MinLongitude = CurrentNodeInfo->Longitude;
            }
            if (CurrentNodeInfo->Longitude > MaxLongitude) {
                MaxLongitude = CurrentNodeInfo->Longitude;
            }
        }
    } else if (ParsingState == ParsingState::Way) {
        // ...
    } else if (ParsingState == ParsingState::Way_NodeRef) {
        if (!FCString::Stricmp(AttributeName, TEXT("ref"))) {
            FOSMNodeInfo * ReferencedNode = NodeMap.FindRef(FPlatformString::Atoi64(AttributeValue));
            const int NewNodeIndex = CurrentWayInfo->Nodes.Num();
            CurrentWayInfo->Nodes.Add(ReferencedNode);

            // Update the node with information about the way that is referencing it
            {
                FOSMWayRef NewWayRef;
                NewWayRef.Way = CurrentWayInfo;
                NewWayRef.NodeIndex = NewNodeIndex;
                ReferencedNode->WayRefs.Add(NewWayRef);
            }
        }
    } else if (ParsingState == ParsingState::Way_Tag) {
        if (!FCString::Stricmp(AttributeName, TEXT("k"))) {
            CurrentWayTagKey = AttributeValue;
        } else if (!FCString::Stricmp(AttributeName, TEXT("v"))) {
            if (!FCString::Stricmp(CurrentWayTagKey, TEXT("name"))) {
                CurrentWayInfo->Name = AttributeValue;
            }
            else if (!FCString::Stricmp(CurrentWayTagKey, TEXT("ref"))) {
                CurrentWayInfo->Ref = AttributeValue;
            }
            else if (!FCString::Stricmp(CurrentWayTagKey, TEXT("highway"))) {
                EOSMWayType WayType = EOSMWayType::OtherRoad;

                if (!FCString::Stricmp(AttributeValue, TEXT("motorway"))) {
                    WayType = EOSMWayType::Motorway;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("motorway_link"))) {
                    WayType = EOSMWayType::Motorway_Link;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("trunk"))) {
                    WayType = EOSMWayType::Trunk;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("trunk_link"))) {
                    WayType = EOSMWayType::Trunk_Link;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("primary"))) {
                    WayType = EOSMWayType::Primary;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("primary_link"))) {
                    WayType = EOSMWayType::Primary_Link;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("secondary"))) {
                    WayType = EOSMWayType::Secondary;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("secondary_link"))) {
                    WayType = EOSMWayType::Secondary_Link;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("tertiary"))) {
                    WayType = EOSMWayType::Tertiary;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("tertiary_link"))) {
                    WayType = EOSMWayType::Tertiary_Link;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("residential"))) {
                    WayType = EOSMWayType::ResidentialRoad;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("service"))) {
                    WayType = EOSMWayType::ServiceRoad;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("unclassified"))) {
                    WayType = EOSMWayType::UnclassifiedRoad;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("living_street"))) {
                    WayType = EOSMWayType::Living_Street;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("pedestrian"))) {
                    WayType = EOSMWayType::Pedestrian;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("track"))) {
                    WayType = EOSMWayType::Track;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("bus_guideway"))) {
                    WayType = EOSMWayType::Bus_Guideway;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("raceway"))) {
                    WayType = EOSMWayType::Raceway;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("road"))) {
                    WayType = EOSMWayType::Road;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("footway"))) {
                    WayType = EOSMWayType::Footway;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("cycleway"))) {
                    WayType = EOSMWayType::Cycleway;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("bridleway"))) {
                    WayType = EOSMWayType::Bridleway;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("steps"))) {
                    WayType = EOSMWayType::Steps;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("path"))) {
                    WayType = EOSMWayType::Path;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("proposed"))) {
                    WayType = EOSMWayType::Proposed;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("construction"))) {
                    WayType = EOSMWayType::RoadConstruction;
                } else {
                    // Other type that we don't recognize yet.  See http://wiki.openstreetmap.org/wiki/Key:highway
                }


                CurrentWayInfo->WayType = WayType;
            }
            else if (!FCString::Stricmp(CurrentWayTagKey, TEXT("building"))) {
                CurrentWayInfo->WayType = EOSMWayType::Building;

                if (!FCString::Stricmp(AttributeValue, TEXT("yes"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::OtherBuilding;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("apartments"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Apartments;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("bungalow"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Bungalow;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("cabin"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Cabin;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("detached"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Detached;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("dormitory"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Dormitory;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("farm"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Farm;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("ger"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Ger;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("hotel"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Hotel;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("house"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::House;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("houseboat"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Houseboat;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("residential"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::ResidentialBuilding;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("semidetached_house"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::SemiDetachedHouse;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("static_caravan"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::StaticCaravan;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("terrace"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Terrace;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("commercial"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::CommercialBuilding;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("industrial"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::IndustrialBuilding;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("kiosk"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Kiosk;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("office"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Office;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("retail"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Retail;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("supermarket"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Supermarket;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("warehouse"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Warehouse;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("cathedral"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Cathedral;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("chapel"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Chapel;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("church"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Church;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("mosque"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Mosque;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("religious"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Religious;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("shrine"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Shrine;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("synagogue"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Synagogue;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("temple"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Temple;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("bakehouse"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Bakehouse;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("civic"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Civic;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("fire_station"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::FireStation;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("government"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Government;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("hospital"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Hospital;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("kindergarten"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Kindergarten;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("public"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::PublicBuilding;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("school"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::School;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("toilets"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Toilets;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("train_station"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::TrainStation;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("transportation"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Transportation;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("university"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::University;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("barn"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Barn;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("conservatory"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Conservatory;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("cowshed"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Cowshed;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("farm_auxiliary"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::FarmAuxiliary;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("greenhouse"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Greenhouse;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("stable"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Stable;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("sty"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Sty;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("grandstand"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Grandstand;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("pavilion"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Pavilion;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("riding_hall"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::RidingHall;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("sports_hall"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::SportsHall;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("stadium"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Stadium;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("hangar"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Hangar;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("hut"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Hut;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("shed"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Shed;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("carport"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Carport;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("garage"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Garage;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("garages"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Garages;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("parking"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Parking;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("digester"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Digester;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("service"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::ServiceBuilding;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("transformer_tower"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::TransformerTower;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("water_tower"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::WaterTower;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("bunker"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Bunker;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("bridge"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Bridge;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("construction"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::BuildingConstruction;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("roof"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Roof;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("ruins"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::Ruins;
                } else if (!FCString::Stricmp(AttributeValue, TEXT("tree_house"))) {
                    CurrentWayInfo->BuildingType = EOSMBuildingType::TreeHouse;
                } else {
                    // Other type that we don't recognize yet.  See http://wiki.openstreetmap.org/wiki/Key:building
                }
            }
            else if (!FCString::Stricmp(CurrentWayTagKey, TEXT("height"))) {
                // Check to see if there is a space character in the height value.  For now, we're looking
                // for straight-up floating point values.
                if (!FString(AttributeValue).Contains(TEXT(" "))) {
                    // Okay, no space character.  So this has got to be a floating point number.  The OSM
                    // spec says that the height values are in meters.
                    CurrentWayInfo->Height = FPlatformString::Atod(AttributeValue);
                } else {
                    // Looks like the height value contains units of some sort.
                    // @todo: Add support for interpreting unit strings and converting the values
                }
            }
            else if (!FCString::Stricmp(CurrentWayTagKey, TEXT("building:levels"))) {
                CurrentWayInfo->BuildingLevels = FPlatformString::Atoi(AttributeValue);
            }
            else if (!FCString::Stricmp(CurrentWayTagKey, TEXT("oneway"))) {
                if (!FCString::Stricmp(AttributeValue, TEXT("yes"))) {
                    CurrentWayInfo->bIsOneWay = true;
                } else {
                    CurrentWayInfo->bIsOneWay = false;
                }
            }
            else if (!FCString::Stricmp(CurrentWayTagKey, TEXT("bridge"))) {
                if (!FCString::Stricmp(AttributeValue, TEXT("yes"))) {
                    CurrentWayInfo->bIsBridge = true;
                } else {
                    CurrentWayInfo->bIsBridge = false;
                }
            }
            else if (!FCString::Stricmp(CurrentWayTagKey, TEXT("layer"))) {
                CurrentWayInfo->Layer = FPlatformString::Atoi(AttributeValue);
            }
            else if (!FCString::Stricmp(CurrentWayTagKey, TEXT("lanes"))) {
                CurrentWayInfo->Lanes = FMath::Max(1, FPlatformString::Atoi(AttributeValue));
            }
            else if (!FCString::Stricmp(CurrentWayTagKey, TEXT("levels"))) {
                CurrentWayInfo->Levels = FMath::Max(1, FPlatformString::Atoi(AttributeValue));
            }
        }
    }

    return true;
}


bool FOSMFile::ProcessClose(const TCHAR * Element) {
    if (ParsingState == ParsingState::Node) {
        NodeMap.Add(CurrentNodeID, CurrentNodeInfo);
        CurrentNodeID = 0;
        CurrentNodeInfo = nullptr;

        ParsingState = ParsingState::Root;
    } else if (ParsingState == ParsingState::Way) {
        Ways.Add(CurrentWayInfo);
        CurrentWayInfo = nullptr;

        ParsingState = ParsingState::Root;
    } else if (ParsingState == ParsingState::Way_NodeRef) {
        ParsingState = ParsingState::Way;
    } else if (ParsingState == ParsingState::Way_Tag) {
        CurrentWayTagKey = TEXT("");
        ParsingState = ParsingState::Way;
    }

    return true;
}
