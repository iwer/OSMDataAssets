// Copyright 2017 Mike Fricker. All Rights Reserved.
// Copyright 2020 Iwer Petersen. All rights reserved.

#pragma once

#include "FastXml.h"
#include "Misc/FeedbackContext.h"
#include "Enums.h"

/** OpenStreetMap file loader */
class FOSMFile : public IFastXmlCallback
{

public:

    /** Default constructor for FOSMFile */
    FOSMFile();

    /** Destructor for FOSMFile */
    virtual ~FOSMFile();

    /** Loads the map from an OpenStreetMap XML file.  Note that in the case of the file path containing the XML data, the string must be mutable for us to parse it quickly. */
    bool LoadOpenStreetMapFile( FString& OSMFilePath, const bool bIsFilePathActuallyTextBuffer, class FFeedbackContext* FeedbackContext );


    struct FOSMWayInfo;

//////


    struct FOSMWayRef
    {
        // Way that we're referencing at this node
        FOSMWayInfo* Way;

        // Index of the node in the way's array of nodes
        int32 NodeIndex;
    };


    struct FOSMNodeInfo
    {
        FString NodeID;
        double Latitude;
        double Longitude;
        TArray<FOSMWayRef> WayRefs;
    };

    USTRUCT()
    struct FOSMWayInfo
    {
        //GENERATED_BODY()
        UPROPERTY()
        FString WayID;
        UPROPERTY()
        FString Name;
        UPROPERTY()
        FString Ref;
        UPROPERTY()
        TArray<FOSMNodeInfo*> Nodes;
        UPROPERTY()
        TEnumAsByte<EOSMWayType> WayType;
        UPROPERTY()
        TEnumAsByte<EOSMBuildingType> BuildingType;
        UPROPERTY()
        double Height;
        UPROPERTY()
        int32 BuildingLevels;

        // If true, way is only traversable in the order the nodes are listed in the Nodes list
        UPROPERTY()
        uint8 bIsOneWay : 1;
        UPROPERTY()
        uint8 bIsBridge : 1;
        UPROPERTY()
        int32 Layer;
        UPROPERTY()
        int32 Lanes;
        UPROPERTY()
        int32 Levels;
    };

    struct FOSMRelMember {
        FString Type;
        FString Ref;
        uint8 bIsInner : 1;
    };

    struct FOSMRelationInfo {
        FString RelationID;
        TArray<FOSMRelMember*> Members;
        TEnumAsByte<EOSMBuildingType> BuildingType;
        int32 BuildingLevels;
        double Height;
    };

    // Minimum latitude/longitude bounds
    double MinLatitude = MAX_dbl;
    double MinLongitude = MAX_dbl;
    double MaxLatitude = -MAX_dbl;
    double MaxLongitude = -MAX_dbl;

    // Average Latitude (roughly the center of the map)
    double AverageLatitude = 0.0;
    double AverageLongitude = 0.0;

    // All ways we've parsed
    TArray<FOSMWayInfo*> Ways;
    TMap<FString, FOSMWayInfo*> WayMap;

    // Maps node IDs to info about each node
    TMap<FString, FOSMNodeInfo*> NodeMap;

    TArray<FOSMRelationInfo*> Relations;

protected:

    // IFastXmlCallback overrides
    virtual bool ProcessXmlDeclaration( const TCHAR* ElementData, int32 XmlFileLineNumber ) override;
    virtual bool ProcessComment( const TCHAR* Comment ) override;
    virtual bool ProcessElement( const TCHAR* ElementName, const TCHAR* ElementData, int32 XmlFileLineNumber ) override;
    static void DecodeWayType(const TCHAR* AttributeValue, EOSMWayType& WayType);
    static void DecodeBuildingType(const TCHAR* AttributeValue, EOSMBuildingType& BuildingType);
    virtual bool ProcessAttribute( const TCHAR* AttributeName, const TCHAR* AttributeValue ) override;
    virtual bool ProcessClose( const TCHAR* Element ) override;


protected:

    enum class ParsingState
    {
        Root,
        Node,
        Way,
        Way_NodeRef,
        Way_Tag,
        Relation,
        Rel_Member,
        Rel_Tag
    };

    // Current state of parser
    ParsingState ParsingState;

    // Node that is currently being parsed
    FOSMNodeInfo* CurrentNodeInfo;

    // Way that is currently being parsed
    FOSMWayInfo* CurrentWayInfo;

    // Relation that is currently parsed
    FOSMRelationInfo * CurrentRelationInfo;

    FOSMRelMember * CurrentRelMember;

    // Current way's current tag key string
    const TCHAR* CurrentWayTagKey;

    // Current relations's current tag key string
    const TCHAR* CurrentRelTagKey;
};
