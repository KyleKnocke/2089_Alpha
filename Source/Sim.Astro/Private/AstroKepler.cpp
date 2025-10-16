#include "AstroKepler.h"

#include "Kismet/KismetMathLibrary.h"

static double NormalizeAngle(double Angle)
{
    while (Angle < -PI)
    {
        Angle += 2.0 * PI;
    }
    while (Angle > PI)
    {
        Angle -= 2.0 * PI;
    }
    return Angle;
}

double FAstroKepler::SolveEccentricAnomaly(double MeanAnomaly, const double Eccentricity, const int32 Iterations)
{
    MeanAnomaly = NormalizeAngle(MeanAnomaly);
    double EccentricAnomaly = MeanAnomaly;
    for (int32 Iter = 0; Iter < Iterations; ++Iter)
    {
        const double Delta = (EccentricAnomaly - Eccentricity * FMath::Sin(EccentricAnomaly) - MeanAnomaly) /
            (1.0 - Eccentricity * FMath::Cos(EccentricAnomaly));
        EccentricAnomaly -= Delta;
    }
    return EccentricAnomaly;
}

FVector3d FAstroKepler::ComputePosition(const FKeplerElements& Elements, const double MeanAnomaly)
{
    const double EccentricAnomaly = SolveEccentricAnomaly(MeanAnomaly, Elements.Eccentricity);
    const double CosE = FMath::Cos(EccentricAnomaly);
    const double SinE = FMath::Sin(EccentricAnomaly);

    const double Distance = Elements.SemiMajorAxis * (1.0 - Elements.Eccentricity * CosE);
    const double XPrime = Elements.SemiMajorAxis * (CosE - Elements.Eccentricity);
    const double YPrime = Elements.SemiMajorAxis * FMath::Sqrt(1.0 - Elements.Eccentricity * Elements.Eccentricity) * SinE;

    const double CosO = FMath::Cos(Elements.LongitudeOfAscendingNode);
    const double SinO = FMath::Sin(Elements.LongitudeOfAscendingNode);
    const double CosI = FMath::Cos(Elements.Inclination);
    const double SinI = FMath::Sin(Elements.Inclination);
    const double CosW = FMath::Cos(Elements.ArgumentOfPeriapsis);
    const double SinW = FMath::Sin(Elements.ArgumentOfPeriapsis);

    const double X = (CosO * CosW - SinO * SinW * CosI) * XPrime + (-CosO * SinW - SinO * CosW * CosI) * YPrime;
    const double Y = (SinO * CosW + CosO * SinW * CosI) * XPrime + (-SinO * SinW + CosO * CosW * CosI) * YPrime;
    const double Z = (SinW * SinI) * XPrime + (CosW * SinI) * YPrime;

    return FVector3d(X, Y, Z).GetSafeNormal() * Distance;
}
