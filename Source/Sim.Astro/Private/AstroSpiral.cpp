#include "AstroSpiral.h"

#include "AstroUnits.h"
#include "Kismet/KismetMathLibrary.h"

namespace
{
    constexpr double ArmPitch = 12.0 * PI / 180.0;
    constexpr double ArmSeparation = 2.0 * PI / 4.0;
    constexpr double ThickDiskScale = 0.15;
    constexpr double ThinDiskScale = 0.05;
}

FGalacticSampleResult UAstroStatics::SampleGalaxy(const FVector3d& GalacticPos, const uint64 Seed)
{
    FGalacticSampleResult Result;
    int32 ArmIndex = INDEX_NONE;
    const double ArmWeight = ComputeArmWeight(GalacticPos, ArmIndex);

    const double Radius = FVector2d(GalacticPos.X, GalacticPos.Y).Length();
    const double Height = FMath::Abs(GalacticPos.Z);
    const double ThickComponent = FMath::Exp(-Height / ThickDiskScale);
    const double ThinComponent = FMath::Exp(-Height / ThinDiskScale);

    Result.Density = ArmWeight * 0.7 + ThickComponent * 0.2 + ThinComponent * 0.1;
    Result.ArmIndex = ArmIndex;
    Result.Metallicity = ComputeMetallicity(GalacticPos);
    Result.RadiationBackground = FMath::Clamp(1.0 - Radius / (Astro::GalacticDiskRadiusKpc * Astro::KiloParsec), 0.1, 1.0);

    FAstroPCG64 Rng = MakeRng(GalacticPos, Seed, 0);
    Result.NearbyProxyCount = 20 + Rng.NextInt(80);

    return Result;
}

FAstroPCG64 UAstroStatics::MakeRng(const FVector3d& GalacticPos, const uint64 Seed, const int32 LOD)
{
    const uint64 Hash = AstroHash(Seed, static_cast<uint64>(GalacticPos.X * 1000.0), static_cast<uint64>(GalacticPos.Y * 1000.0), static_cast<uint64>(LOD));
    return FAstroPCG64(Hash);
}

double UAstroStatics::ComputeMetallicity(const FVector3d& GalacticPos)
{
    const double Radius = FVector2d(GalacticPos.X, GalacticPos.Y).Length();
    const double Normalized = 1.0 - FMath::Clamp(Radius / (Astro::GalacticDiskRadiusKpc * Astro::KiloParsec), 0.0, 1.0);
    return 0.1 + Normalized * 0.9;
}

double UAstroStatics::ComputeArmWeight(const FVector3d& GalacticPos, int32& OutArmIndex)
{
    const double Radius = FVector2d(GalacticPos.X, GalacticPos.Y).Length();
    const double Angle = FMath::Atan2(GalacticPos.Y, GalacticPos.X);
    double BestWeight = 0.0;
    int32 BestIndex = 0;
    for (int32 Arm = 0; Arm < 4; ++Arm)
    {
        const double ArmAngle = Arm * ArmSeparation + FMath::Loge(FMath::Max(Radius, 1e-3)) / FMath::Tan(ArmPitch);
        const double Delta = FMath::FindDeltaAngleRadians(Angle, ArmAngle);
        const double Weight = FMath::Exp(-FMath::Square(Delta * 2.0));
        if (Weight > BestWeight)
        {
            BestWeight = Weight;
            BestIndex = Arm;
        }
    }

    OutArmIndex = BestIndex;
    return BestWeight;
}
