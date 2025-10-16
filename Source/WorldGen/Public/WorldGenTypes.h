#pragma once

#include "CoreMinimal.h"
#include "WorldGenTypes.generated.h"

UENUM(BlueprintType)
enum class EBodyType : uint8
{
    Star,
    Rocky,
    GasGiant,
    Ice,
    Dwarf,
    Station
};

UENUM(BlueprintType)
enum class EFacilityType : uint8
{
    Mining,
    Fabrication,
    Logistics,
    Research
};

UENUM(BlueprintType)
enum class EWorldBody : uint8
{
    Unknown,
    Sol,
    Earth,
    Moon,
    Mars,
    Jupiter,
    Io,
    Europa,
    Ganymede,
    Callisto
};

UENUM(BlueprintType)
enum class ENhiType : uint8
{
    SensorMesh,
    AcousticPhantom,
    MycelialMat,
    SubsurfaceNode
};

USTRUCT(BlueprintType)
struct FBodyDescriptor
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    EBodyType Type = EBodyType::Rocky;

    UPROPERTY(BlueprintReadOnly)
    double SMA = 0.0;

    UPROPERTY(BlueprintReadOnly)
    double Ecc = 0.0;

    UPROPERTY(BlueprintReadOnly)
    double Incl = 0.0;

    UPROPERTY(BlueprintReadOnly)
    double RadiusKm = 0.0;

    UPROPERTY(BlueprintReadOnly)
    double Density = 0.0;
};

USTRUCT(BlueprintType)
struct FStarRecord
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FVector3d GalPos = FVector3d::Zero();

    UPROPERTY(BlueprintReadOnly)
    uint8 SpectralClass = 0;

    UPROPERTY(BlueprintReadOnly)
    float Lum = 1.0f;

    UPROPERTY(BlueprintReadOnly)
    float Metallicity = 0.0f;
};

USTRUCT(BlueprintType)
struct FStarAddress
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    int32 SectorX = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 SectorY = 0;

    UPROPERTY(BlueprintReadOnly)
    uint64 LocalStarId = 0;
};

USTRUCT(BlueprintType)
struct FSystemDescriptor
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FStarRecord Star;

    UPROPERTY(BlueprintReadOnly)
    TArray<FBodyDescriptor> Bodies;
};

USTRUCT(BlueprintType)
struct FFacilityDescriptor
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString Name;

    UPROPERTY(BlueprintReadOnly)
    EFacilityType Type = EFacilityType::Mining;

    UPROPERTY(BlueprintReadOnly)
    FVector3d Pos = FVector3d::Zero();

    UPROPERTY(BlueprintReadOnly)
    EWorldBody Parent = EWorldBody::Unknown;

    UPROPERTY(BlueprintReadOnly)
    int32 WorkforceProxy = 0;
};

USTRUCT(BlueprintType)
struct FArmadaDescriptor
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FName Faction;

    UPROPERTY(BlueprintReadOnly)
    int32 FleetCount = 0;

    UPROPERTY(BlueprintReadOnly)
    FVector3d Centroid = FVector3d::Zero();

    UPROPERTY(BlueprintReadOnly)
    int32 Strength = 0;
};

USTRUCT(BlueprintType)
struct FNhiNodeDescriptor
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    ENhiType Type = ENhiType::SensorMesh;

    UPROPERTY(BlueprintReadOnly)
    FVector3d Pos = FVector3d::Zero();

    UPROPERTY(BlueprintReadOnly)
    EWorldBody Parent = EWorldBody::Unknown;

    UPROPERTY(BlueprintReadOnly)
    float Activity = 0.0f;
};

USTRUCT(BlueprintType)
struct FProxyStar
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FVector3d Position = FVector3d::Zero();

    UPROPERTY(BlueprintReadOnly)
    uint8 SpectralClass = 0;
};

USTRUCT(BlueprintType)
struct FSectorDescriptor
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FIntPoint Sector = FIntPoint::ZeroValue;

    UPROPERTY(BlueprintReadOnly)
    FVector3d Origin = FVector3d::Zero();

    UPROPERTY(BlueprintReadOnly)
    TArray<FProxyStar> ProxyStars;
};
