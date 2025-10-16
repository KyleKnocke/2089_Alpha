#include "SystemSynth.h"

#include "AstroKepler.h"
#include "AstroRNG.h"
#include "AstroSpiral.h"
#include "AstroUnits.h"

namespace
{
    constexpr double SectorSizeLy = 1000.0;

    double PickSpectralLuminosity(uint8 SpectralClass)
    {
        switch (SpectralClass)
        {
        case 0: return 1.0; // G
        case 1: return 1.5; // F
        case 2: return 0.8; // K
        case 3: return 0.2; // M
        case 4: return 3.0; // A
        case 5: return 10.0; // B
        default: return 0.05; // dwarf
        }
    }

    EBodyType ChooseBodyType(FAstroPCG64& Rng)
    {
        const double Roll = Rng.NextDouble();
        if (Roll < 0.2)
        {
            return EBodyType::GasGiant;
        }
        if (Roll < 0.5)
        {
            return EBodyType::Rocky;
        }
        if (Roll < 0.7)
        {
            return EBodyType::Ice;
        }
        if (Roll < 0.85)
        {
            return EBodyType::Dwarf;
        }
        return EBodyType::Station;
    }
}

FStarRecord WorldGen::MakeStarRecord(const FStarAddress& Address, const int64 MasterSeed)
{
    const uint64 Hash = AstroHash(static_cast<uint64>(MasterSeed), static_cast<uint64>(Address.SectorX), static_cast<uint64>(Address.SectorY), Address.LocalStarId);
    FAstroPCG64 Rng(Hash);

    const FVector3d SectorOrigin(Address.SectorX * SectorSizeLy, Address.SectorY * SectorSizeLy, 0.0);
    const FVector3d Offset(Rng.NextDouble() * SectorSizeLy, Rng.NextDouble() * SectorSizeLy, (Rng.NextDouble() - 0.5) * 200.0);

    FStarRecord Record;
    Record.GalPos = SectorOrigin + Offset;
    Record.SpectralClass = static_cast<uint8>(Rng.NextInt(6));
    Record.Lum = static_cast<float>(PickSpectralLuminosity(Record.SpectralClass));

    const FGalacticSampleResult Sample = UAstroStatics::SampleGalaxy(Record.GalPos, MasterSeed);
    Record.Metallicity = Sample.Metallicity;

    return Record;
}

FSystemDescriptor WorldGen::MakeSystem(const FStarAddress& Address, const int64 MasterSeed)
{
    FSystemDescriptor Descriptor;
    Descriptor.Star = MakeStarRecord(Address, MasterSeed);

    const uint64 Hash = AstroHash(static_cast<uint64>(MasterSeed) ^ 0xABCDEFu, static_cast<uint64>(Address.SectorX), static_cast<uint64>(Address.SectorY), Address.LocalStarId);
    FAstroPCG64 Rng(Hash);

    const int32 BodyCount = 1 + Rng.NextInt(12);
    for (int32 Idx = 0; Idx < BodyCount; ++Idx)
    {
        FBodyDescriptor Body;
        Body.Type = ChooseBodyType(Rng);
        Body.SMA = Astro::AstronomicalUnitInKm * (0.2 + Idx * 0.4 + Rng.NextDouble());
        Body.Ecc = FMath::Clamp(Rng.NextDouble() * 0.3, 0.0, 0.9);
        Body.Incl = Rng.NextDouble() * 0.1;
        Body.RadiusKm = 1000.0 + Rng.NextDouble() * 60000.0;
        Body.Density = 1.0 + Rng.NextDouble() * 4.0;
        Descriptor.Bodies.Add(Body);
    }

    return Descriptor;
}
