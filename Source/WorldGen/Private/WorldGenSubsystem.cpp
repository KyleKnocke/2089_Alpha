#include "WorldGenSubsystem.h"

#include "AstroRNG.h"
#include "AstroSpiral.h"
#include "SectorCache.h"
#include "SystemSynth.h"
#include "RPG/Data/ArmadaCatalog.h"
#include "RPG/Data/Gr0cFacilityCatalog.h"
#include "RPG/Data/NhiCatalog.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

namespace
{
    constexpr double SectorSizeLy = 1000.0;

    EWorldBody ParseWorldBody(const FString& BodyName)
    {
        if (BodyName.Equals(TEXT("Earth"), ESearchCase::IgnoreCase))
        {
            return EWorldBody::Earth;
        }
        if (BodyName.Equals(TEXT("Moon"), ESearchCase::IgnoreCase))
        {
            return EWorldBody::Moon;
        }
        if (BodyName.Equals(TEXT("Jupiter"), ESearchCase::IgnoreCase))
        {
            return EWorldBody::Jupiter;
        }
        if (BodyName.Equals(TEXT("Io"), ESearchCase::IgnoreCase))
        {
            return EWorldBody::Io;
        }
        if (BodyName.Equals(TEXT("Europa"), ESearchCase::IgnoreCase))
        {
            return EWorldBody::Europa;
        }
        if (BodyName.Equals(TEXT("Ganymede"), ESearchCase::IgnoreCase))
        {
            return EWorldBody::Ganymede;
        }
        if (BodyName.Equals(TEXT("Callisto"), ESearchCase::IgnoreCase))
        {
            return EWorldBody::Callisto;
        }
        return EWorldBody::Unknown;
    }
}

void UWorldGenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    SectorCache = MakeUnique<FSectorCache>();
    SectorCache->SetCapacity(128);

    if (!ArmadaCatalog)
    {
        ArmadaCatalog = NewObject<UArmadaCatalog>(this, TEXT("ArmadaCatalog"));
    }
    if (!Gr0cCatalog)
    {
        Gr0cCatalog = NewObject<UGr0cFacilityCatalog>(this, TEXT("Gr0cCatalog"));
    }
    if (!NhiCatalog)
    {
        NhiCatalog = NewObject<UNhiCatalog>(this, TEXT("NhiCatalog"));
    }

    InitGalaxy(12345);
}

void UWorldGenSubsystem::InitGalaxy(const int64 InMasterSeed)
{
    MasterSeed = InMasterSeed;
    if (SectorCache)
    {
        SectorCache->Clear();
    }

    Armadas.Reset();
    JovianFacilities.Reset();
    NhiNodes.Reset();

    GenerateArmadas();
    BuildJovianFacilities();
    GenerateNhiNodes();
}

void UWorldGenSubsystem::TeleportSector(const int32 SectorX, const int32 SectorY)
{
    if (UWorld* World = GetWorld())
    {
        if (APlayerController* PlayerController = World->GetFirstPlayerController())
        {
            if (APawn* Pawn = PlayerController->GetPawn())
            {
                const FVector TargetLocation(SectorX * SectorSizeLy * 100.0, SectorY * SectorSizeLy * 100.0, Pawn->GetActorLocation().Z);
                Pawn->SetActorLocation(TargetLocation);
            }
        }
    }

    UE_LOG(LogTemp, Log, TEXT("TeleportSector to (%d, %d)"), SectorX, SectorY);
}

void UWorldGenSubsystem::FocusSystem(const int32 SectorX, const int32 SectorY, const uint64 LocalId)
{
    const FStarAddress Address{SectorX, SectorY, LocalId};
    const FSystemDescriptor System = GetStarSystem(Address);
    UE_LOG(LogTemp, Log, TEXT("FocusSystem sector (%d,%d) id %llu -> bodies: %d"), SectorX, SectorY, LocalId, System.Bodies.Num());
}

void UWorldGenSubsystem::ListArmadas()
{
    for (const FArmadaDescriptor& Armada : Armadas)
    {
        UE_LOG(LogTemp, Log, TEXT("Armada %s fleets:%d strength:%d pos:(%.1f, %.1f, %.1f)"), *Armada.Faction.ToString(), Armada.FleetCount, Armada.Strength, Armada.Centroid.X, Armada.Centroid.Y, Armada.Centroid.Z);
    }
}

void UWorldGenSubsystem::ListNhiNodes(const FString& BodyName)
{
    const EWorldBody Body = ParseWorldBody(BodyName);
    if (Body == EWorldBody::Unknown)
    {
        UE_LOG(LogTemp, Warning, TEXT("Unknown body '%s'"), *BodyName);
        return;
    }

    TArray<FNhiNodeDescriptor> Nodes;
    NhiNodes.MultiFind(Body, Nodes);
    UE_LOG(LogTemp, Log, TEXT("NHI nodes for %s: %d"), *BodyName, Nodes.Num());
    for (const FNhiNodeDescriptor& Node : Nodes)
    {
        UE_LOG(LogTemp, Log, TEXT(" - Type %d activity %.2f pos (%.1f, %.1f, %.1f)"), static_cast<int32>(Node.Type), Node.Activity, Node.Pos.X, Node.Pos.Y, Node.Pos.Z);
    }
}

FSectorDescriptor UWorldGenSubsystem::GetSector(const FIntPoint SectorXY)
{
    if (SectorCache)
    {
        FSectorDescriptor Cached;
        if (SectorCache->Get(SectorXY, Cached))
        {
            return Cached;
        }
    }

    FSectorDescriptor Descriptor = GenerateSectorInternal(SectorXY);
    if (SectorCache)
    {
        SectorCache->Put(Descriptor);
    }
    return Descriptor;
}

FSystemDescriptor UWorldGenSubsystem::GetStarSystem(const FStarAddress& Address)
{
    return WorldGen::MakeSystem(Address, MasterSeed);
}

TArray<FArmadaDescriptor> UWorldGenSubsystem::GetNearestArmadas(const FVector3d& GalPos, const double RadiusLy)
{
    TArray<FArmadaDescriptor> Result;
    const double RadiusSquared = RadiusLy * RadiusLy;
    for (const FArmadaDescriptor& Armada : Armadas)
    {
        const double DistSq = FVector3d::DistSquared(Armada.Centroid, GalPos);
        if (DistSq <= RadiusSquared)
        {
            Result.Add(Armada);
        }
    }
    const FVector3d LocalPos = GalPos;
    Result.Sort([LocalPos](const FArmadaDescriptor& A, const FArmadaDescriptor& B)
    {
        return FVector3d::DistSquared(A.Centroid, LocalPos) < FVector3d::DistSquared(B.Centroid, LocalPos);
    });
    return Result;
}

TArray<FFacilityDescriptor> UWorldGenSubsystem::GetJovianFacilities()
{
    return JovianFacilities;
}

TArray<FNhiNodeDescriptor> UWorldGenSubsystem::GetNhiNodes(const EWorldBody Body)
{
    TArray<FNhiNodeDescriptor> Result;
    NhiNodes.MultiFind(Body, Result);
    return Result;
}

void UWorldGenSubsystem::GenerateArmadas()
{
    if (!ArmadaCatalog)
    {
        return;
    }

    FAstroPCG64 Rng(static_cast<uint64>(MasterSeed));
    for (const FArmadaFactionDefinition& Def : ArmadaCatalog->Factions)
    {
        const FVector3d Home(Def.HomeSector.X * SectorSizeLy, Def.HomeSector.Y * SectorSizeLy, 0.0);
        const int32 Range = FMath::Max(1, Def.FleetSize.Max - Def.FleetSize.Min + 1);
        const int32 FleetCount = FMath::Clamp(Def.FleetSize.Min + Rng.NextInt(Range), Def.FleetSize.Min, Def.FleetSize.Max);
        FArmadaDescriptor Descriptor;
        Descriptor.Faction = Def.Name;
        Descriptor.FleetCount = FleetCount;
        Descriptor.Centroid = Home + FVector3d(Rng.NextDouble() * 200.0, Rng.NextDouble() * 200.0, Rng.NextDouble() * 50.0);
        Descriptor.Strength = FleetCount * Def.StrengthPerFleet;
        Armadas.Add(Descriptor);
    }
}

void UWorldGenSubsystem::BuildJovianFacilities()
{
    if (!Gr0cCatalog)
    {
        return;
    }

    JovianFacilities = Gr0cCatalog->GenerateFacilities(MasterSeed);
}

void UWorldGenSubsystem::GenerateNhiNodes()
{
    if (!NhiCatalog)
    {
        return;
    }

    TMap<EWorldBody, TArray<FNhiNodeDescriptor>> Generated = NhiCatalog->GenerateNodes(MasterSeed);
    for (const TPair<EWorldBody, TArray<FNhiNodeDescriptor>>& Pair : Generated)
    {
        for (const FNhiNodeDescriptor& Node : Pair.Value)
        {
            NhiNodes.Add(Pair.Key, Node);
        }
    }
}

FSectorDescriptor UWorldGenSubsystem::GenerateSectorInternal(const FIntPoint SectorXY)
{
    const uint64 Hash = AstroHash(static_cast<uint64>(MasterSeed) ^ 0x12345678ull, static_cast<uint64>(SectorXY.X), static_cast<uint64>(SectorXY.Y));
    FAstroPCG64 Rng(Hash);

    FSectorDescriptor Descriptor;
    Descriptor.Sector = SectorXY;
    Descriptor.Origin = FVector3d(SectorXY.X * SectorSizeLy, SectorXY.Y * SectorSizeLy, 0.0);

    const int32 ProxyCount = 200 + Rng.NextInt(200);
    Descriptor.ProxyStars.Reserve(ProxyCount);

    for (int32 Idx = 0; Idx < ProxyCount; ++Idx)
    {
        FProxyStar Proxy;
        Proxy.Position = Descriptor.Origin + FVector3d(Rng.NextDouble() * SectorSizeLy, Rng.NextDouble() * SectorSizeLy, (Rng.NextDouble() - 0.5) * 500.0);
        Proxy.SpectralClass = static_cast<uint8>(Rng.NextInt(6));
        Descriptor.ProxyStars.Add(Proxy);
    }

    return Descriptor;
}
