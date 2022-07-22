// Copyright (c) Iwer Petersen. All rights reserved.
#pragma once

/** Types of ways */
//enum class EOSMWayType
UENUM(BlueprintType)
enum EOSMWayType
{
    ///
    /// ROADS
    ///

    /** A restricted access major divided highway, normally with 2 or more running lanes plus emergency hard shoulder. Equivalent to the Freeway, Autobahn, etc. */
    Motorway,

    /** The link roads (sliproads/ramps) leading to/from a motorway from/to a motorway or lower class highway. Normally with the same motorway restrictions. */
    Motorway_Link,

    /** The most important roads in a country's system that aren't motorways. (Need not necessarily be a divided highway.) */
    Trunk,

    /** The link roads (sliproads/ramps) leading to/from a trunk road from/to a trunk road or lower class highway. */
    Trunk_Link,

    /** The next most important roads in a country's system. (Often link larger towns.) */
    Primary,

    /** The link roads (sliproads/ramps) leading to/from a primary road from/to a primary road or lower class highway. */
    Primary_Link,

    /** The next most important roads in a country's system. (Often link smaller towns and villages.) */
    Secondary,

    /** The link roads (sliproads/ramps) leading to/from a secondary road from/to a secondary road or lower class highway. */
    Secondary_Link,

    /** The next most important roads in a country's system. */
    Tertiary,

    /** The link roads (sliproads/ramps) leading to/from a tertiary road from/to a tertiary road or lower class highway. */
    Tertiary_Link,

    /** Roads which are primarily lined with and serve as an access to housing. */
    ResidentialRoad,

    /** For access roads to, or within an industrial estate, camp site, business park, car park etc. */
    ServiceRoad,

    /** The least most important through roads in a country's system, i.e. minor roads of a lower classification than tertiary, but which serve a purpose other than access to properties. */
    UnclassifiedRoad,


    ///
    /// NON-ROADS
    ///

    /** Residential streets where pedestrians have legal priority over cars, speeds are kept very low and where children are allowed to play on the street. */
    Living_Street,

    /** For roads used mainly/exclusively for pedestrians in shopping and some residential areas which may allow access by motorised vehicles only for very limited periods of the day. */
    Pedestrian,

    /** Roads for agricultural or forestry uses etc, often rough with unpaved/unsealed surfaces, that can be used only by off-road vehicles (4WD, tractors, ATVs, etc.) */
    Track,

    /** A busway where the vehicle guided by the way (though not a railway) and is not suitable for other traffic. */
    Bus_Guideway,

    /** A course or track for (motor) racing */
    Raceway,

    /** A road where the mapper is unable to ascertain the classification from the information available. */
    Road,


    ///
    /// PATHS
    ///

    /** For designated footpaths; i.e., mainly/exclusively for pedestrians. This includes walking tracks and gravel paths. */
    Footway,

    /** For designated cycleways. */
    Cycleway,

    /** Paths normally used by horses */
    Bridleway,

    /** For flights of steps (stairs) on footways. */
    Steps,

    /** A non-specific path. */
    Path,


    ///
    /// LIFECYCLE
    ///

    /** For planned roads, use with proposed=* and also proposed=* with a value of the proposed highway value. */
    Proposed,

    /** For roads under construction. */
    RoadConstruction,


    ///
    /// BUILDINGS
    ///

    /** Default type of building.  A general catch-all. */
    Building,


    ///
    /// UNSUPPORTED
    ///

    /** Currently unrecognized type */
    OtherRoad
};

UENUM(BlueprintType)
enum EOSMBuildingType {
    /// ACCOMMODATION
    /** A building arranged into individual dwellings, often on separate floors. May also have retail outlets on the ground floor. */
    Apartments,
    /** A single-storey detached small house, Dacha.  */
    Bungalow,
    /** A cabin is a small, roughly built house usually with a wood exterior and typically found in rural areas.  */
    Cabin,
    /** A detached house, a free-standing residential building usually housing a single family.  */
    Detached,
    /** For a shared building, as used by college/university students (not a share room for multiple occupants as implied by the term in British English). */
    Dormitory,
    /** A residential building on a farm (farmhouse). */
    Farm,
    /** A permanent or seasonal round yurt or ger used as dwelling.  */
    Ger,
    /** A building designed with separate rooms available for overnight accommodation. */
    Hotel,
    /** A dwelling unit inhabited by a single household (a family or small group sharing facilities such as a kitchen). */
    House,
    /** A boat used primarily as a home  */
    Houseboat,
    /** A general tag for a building used primarily for residential purposes.  */
    ResidentialBuilding,
    /** A residential house that shares a common wall with another on one side.  */
    SemiDetachedHouse,
    /** A mobile home (semi)permanently left on a single site  */
    StaticCaravan,
    /** A single way used to define the outline of a linear row of residential dwellings, each of which normally has its own entrance, which form a terrace (row-house in North American English).  */
    Terrace,

    /// COMMERCIAL

    /** A building where non-specific commercial activities take place, not necessarily an office building. */
    CommercialBuilding,
    /** A building where some industrial process takes place. */
    IndustrialBuilding,
    /** A small one-room retail building.  */
    Kiosk,
    /** An office building.  */
    Office,
    /** A building primarily used for selling goods that are sold to the public */
    Retail,
    /** A building constructed to house a self-service large-area store.  */
    Supermarket,
    /** A building primarily used for the storage or goods or as part of a distribution system.  */
    Warehouse,

    /// RELIGIOUS

    /** A building that was built as a cathedral.  */
    Cathedral,
    /** A building that was built as a chapel. */
    Chapel,
    /** A building that was built as a church. */
    Church,
    /** A mosque.  */
    Mosque,
    /** Unspecific religious building. */
    Religious,
    /** A building that was built as a shrine.  */
    Shrine,
    /** A building that was built as a synagogue. */
    Synagogue,
    /** A building that was built as a temple. */
    Temple,

    /// CIVIC / AMENITY

    /** A building that was built as a bakehouse (i.e. for baking bread).  */
    Bakehouse,
    /** For any civic amenity, for example amenity=community_centre, amenity=library, amenity=toilets, leisure=sports_centre, leisure=swimming_pool, amenity=townhall etc. */
    Civic,
    /** A building which houses fire fighting equipment ready for use. */
    FireStation,
    /** For government buildings in general, including municipal, provincial and divisional secretaries, government agencies and departments, town halls, (regional) parliaments and court houses.  */
    Government,
    /** A building which forms part of a hospital. */
    Hospital,
    /** For any generic kindergarten buildings. */
    Kindergarten,
    /** A building constructed as accessible to the general public (a town hall, police station, court house, etc.).  */
    PublicBuilding,
    /** For any generic school buildings. */
    School,
    /** A toilet block.  */
    Toilets,
    /** A building constructed to be a train station building, including buildings that are abandoned and used nowadays for a different purpose.  */
    TrainStation,
    /** A building related to public transport. */
    Transportation,
    /** A university building. */
    University,

    /// AGRICULTURAL / PLANT PRODUCTION

    /** An agricultural building used for storage and as a covered workplace.  */
    Barn,
    /** A building or room having glass or tarpaulin roofing and walls used as an indoor garden or a sunroom (winter garden).  */
    Conservatory,
    /** A cowshed (cow barn, cow house) is a building for housing cows, usually found on farms.  */
    Cowshed,
    /** A building on a farm that is not a dwelling (use 'farm' or 'house' for the farm house).  */
    FarmAuxiliary,
    /** A greenhouse is a glass or plastic covered building used to grow plants. */
    Greenhouse,
    /** A stable is a building where horses are kept.  */
    Stable,
    /** A sty (pigsty, pig ark, pig-shed) is a building for raising domestic pigs, usually found on farms.  */
    Sty,

    /// SPORTS

    /** The main stand, usually roofed, commanding the best view for spectators at racecourses or sports grounds.  */
    Grandstand,
    /** A sports pavilion usually with changing rooms, storage areas and possibly an space for functions & events.  */
    Pavilion,
    /** A building that was built as a riding hall.  */
    RidingHall,
    /** A building that was built as a sports hall.  */
    SportsHall,
    /** A building constructed to be a stadium building, including buildings that are abandoned and used nowadays for a different purpose.  */
    Stadium,

    /// STORAGE

    /** A hangar is a building used for the storage of airplanes, helicopters or space-craft.  */
    Hangar,
    /** A hut is a small and crude shelter.  */
    Hut,
    /** A shed is a simple, single-storey structure in a back garden or on an allotment that is used for storage, hobbies, or as a workshop.  */
    Shed,

    /// CARS

    /** A carport is a covered structure used to offer limited protection to vehicles, primarily cars, from the elements. Unlike most structures a carport does not have four walls, and usually has one or two.  */
    Carport,
    /** A garage is a building suitable for the storage of one or possibly more motor vehicle or similar. */
    Garage,
    /** A building that consists of a number of discrete storage spaces for different owners/tenants. */
    Garages,
    /** Structure purpose-built for parking cars.  */
    Parking,

    /// POWER / TECHNICAL BUILDINGS

    /** A digester is a bioreactor for the production of inflatable biogas from biomass.  */
    Digester,
    /** Service building usually is a small unmanned building with certain machinery (like pumps or transformers).  */
    ServiceBuilding,
    /** A transformer tower is a characteristic tall building comprising a distribution transformer and constructed to connect directly to a medium voltage overhead power line. Quite often the power line has since been undergrounded but the building may still serve as a substation. */
    TransformerTower,
    /** A water tower  */
    WaterTower,

    /// OTHER BUILDINGS

    /** A hardened military building. */
    Bunker,
    /** A building used as a bridge. Can also represent a gatehouse for drawbridges.  */
    Bridge,
    /** Used for buildings under construction. */
    BuildingConstruction,
    /** A structure that consists of a roof with open sides, such as a rain shelter, and also gas stations  */
    Roof,
    /** Frequently used for a house or other building that is abandoned and in poor repair. However, some believe this usage is incorrect, and the tag should only be used for buildings constructed as fake ruins (for example sham ruins in an English landscape garden).  */
    Ruins,
    /** An accommodation, often designed as a small hut, sometimes also as a room or small apartment. Built on tree posts or on a natural tree. A tree house has no contact with the ground. Access via ladders, stairs or bridgeways. */
    TreeHouse,
    /** Use this value (building=yes) where it is not possible to determine a more specific value.   */
    OtherBuilding
};
