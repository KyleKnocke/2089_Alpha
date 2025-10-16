#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Templates/UniquePtr.h"
#include "WorldGenTypes.h"
#include "WorldGenSubsystem.generated.h"

class UArmadaCatalog;
class UGr0cFacilityCatalog;
class UNhiCatalog;
class FSectorCache;

/**
 * Deterministic world generation subsystem.
 */
UCLASS()
class WORLDGEN_API UWorldGenSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(Exec)
    void InitGalaxy(int64 MasterSeed);

    UFUNCTION(Exec)
    void TeleportSector(int32 SectorX, int32 SectorY);

    UFUNCTION(Exec)
    void FocusSystem(int32 SectorX, int32 SectorY, uint64 LocalId);

    UFUNCTION(Exec)
    void ListArmadas();

    UFUNCTION(Exec)
    void ListNhiNodes(const FString& BodyName);

    UFUNCTION(BlueprintCallable, Category = "WorldGen")
    FSectorDescriptor GetSector(FIntPoint SectorXY);

    UFUNCTION(BlueprintCallable, Category = "WorldGen")
    FSystemDescriptor GetStarSystem(const FStarAddress& Address);

    UFUNCTION(BlueprintCallable, Category = "WorldGen")
    TArray<FArmadaDescriptor> GetNearestArmadas(const FVector3d& GalPos, double RadiusLy);

    UFUNCTION(BlueprintCallable, Category = "WorldGen")
    TArray<FFacilityDescriptor> GetJovianFacilities();

    UFUNCTION(BlueprintCallable, Category = "WorldGen")
    TArray<FNhiNodeDescriptor> GetNhiNodes(EWorldBody Body);

    /** Returns the current master seed. */
    int64 GetMasterSeed() const { return MasterSeed; }

private:
    void GenerateArmadas();
    void BuildJovianFacilities();
    void GenerateNhiNodes();

    FSectorDescriptor GenerateSectorInternal(FIntPoint SectorXY);

    int64 MasterSeed = 0;

    /** Cached sector descriptors with a small LRU policy. */
    TUniquePtr<FSectorCache> SectorCache;

    /** Armada descriptors. */
    TArray<FArmadaDescriptor> Armadas;

    /** Precomputed facility data. */
    TArray<FFacilityDescriptor> JovianFacilities;

    /** Cached NHI nodes keyed by body. */
    TMultiMap<EWorldBody, FNhiNodeDescriptor> NhiNodes;

    UPROPERTY()
    TObjectPtr<UArmadaCatalog> ArmadaCatalog;

    UPROPERTY()
    TObjectPtr<UGr0cFacilityCatalog> Gr0cCatalog;

    UPROPERTY()
    TObjectPtr<UNhiCatalog> NhiCatalog;
};
