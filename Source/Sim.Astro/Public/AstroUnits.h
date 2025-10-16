#pragma once

#include "CoreMinimal.h"

namespace Astro
{
    constexpr double KiloParsec = 3261.563777; // light years per kiloparsec
    constexpr double GalacticDiskRadiusKpc = 26.0;
    constexpr double BarRadiusKpc = 5.0;
    constexpr double LightYearInKm = 9.4607e12;
    constexpr double AstronomicalUnitInKm = 1.495978707e8;

    inline double LightYearsToKilometers(const double LightYears)
    {
        return LightYears * LightYearInKm;
    }

    inline double KilometersToLightYears(const double Kilometers)
    {
        return Kilometers / LightYearInKm;
    }

    inline double AstronomicalUnitsToKilometers(const double AU)
    {
        return AU * AstronomicalUnitInKm;
    }

    inline double KilometersToAstronomicalUnits(const double Kilometers)
    {
        return Kilometers / AstronomicalUnitInKm;
    }
}
