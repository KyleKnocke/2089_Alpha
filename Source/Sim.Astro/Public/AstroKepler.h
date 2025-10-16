#pragma once

#include "CoreMinimal.h"
#include "AstroUnits.h"

USTRUCT(BlueprintType)
struct FKeplerElements
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
    double SemiMajorAxis = Astro::AstronomicalUnitInKm;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
    double Eccentricity = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
    double Inclination = 0.0; // radians

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
    double LongitudeOfAscendingNode = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
    double ArgumentOfPeriapsis = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
    double MeanAnomalyAtEpoch = 0.0;
};

/** Orbit helpers for placing system bodies. */
class SIMASTRO_API FAstroKepler
{
public:
    /** Solves Kepler's equation for the provided mean anomaly. */
    static double SolveEccentricAnomaly(double MeanAnomaly, double Eccentricity, int32 Iterations = 8);

    /** Returns Cartesian position in kilometers relative to the parent body for the provided orbital elements and mean anomaly. */
    static FVector3d ComputePosition(const FKeplerElements& Elements, double MeanAnomaly);
};
