#pragma once

#include "CoreMinimal.h"
#include "AstroRNG.h"

USTRUCT(BlueprintType)
struct FGalacticSampleResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    double Density = 0.0;

    UPROPERTY(BlueprintReadOnly)
    int32 ArmIndex = INDEX_NONE;

    UPROPERTY(BlueprintReadOnly)
    double Metallicity = 0.0;

    UPROPERTY(BlueprintReadOnly)
    double RadiationBackground = 0.0;

    UPROPERTY(BlueprintReadOnly)
    int32 NearbyProxyCount = 0;
};

/**
 * Utilities for approximating Milky Way spiral arm density and metallicity.
 */
UCLASS()
class SIMASTRO_API UAstroStatics : public UObject
{
    GENERATED_BODY()

public:
    /**
     * Samples the galactic density field using logarithmic spiral arms.
     */
    UFUNCTION(BlueprintCallable, Category = "Astro")
    static FGalacticSampleResult SampleGalaxy(const FVector3d& GalacticPos, uint64 Seed);

    /** Returns a deterministic RNG seeded from galactic coordinates. */
    static FAstroPCG64 MakeRng(const FVector3d& GalacticPos, uint64 Seed, int32 LOD);

    /** Simple metallicity gradient: higher toward the core. */
    static double ComputeMetallicity(const FVector3d& GalacticPos);

    /** Returns density weight for a given spiral arm index. */
    static double ComputeArmWeight(const FVector3d& GalacticPos, int32& OutArmIndex);
};
