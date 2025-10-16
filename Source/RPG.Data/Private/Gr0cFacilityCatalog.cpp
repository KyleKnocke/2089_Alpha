#include "Gr0cFacilityCatalog.h"

#include "AstroRNG.h"

UGr0cFacilityCatalog::UGr0cFacilityCatalog()
{
    Facilities =
    {
        {TEXT("Io Mining Pad"), EFacilityType::Mining, EWorldBody::Io, 4, 6},
        {TEXT("Europa Ice Borer"), EFacilityType::Research, EWorldBody::Europa, 3, 5},
        {TEXT("Ganymede Fabricator"), EFacilityType::Fabrication, EWorldBody::Ganymede, 5, 7},
        {TEXT("Callisto Logistics Hub"), EFacilityType::Logistics, EWorldBody::Callisto, 2, 4}
    };
}

TArray<FFacilityDescriptor> UGr0cFacilityCatalog::GenerateFacilities(const int64 MasterSeed) const
{
    TArray<FFacilityDescriptor> Result;
    FAstroPCG64 Rng(static_cast<uint64>(MasterSeed) ^ 0x5A5A5A5Au);

    for (const FGr0cFacilityDefinition& Def : Facilities)
    {
        const int32 Count = Def.MinCount + Rng.NextInt(FMath::Max(1, Def.MaxCount - Def.MinCount + 1));
        for (int32 Index = 0; Index < Count; ++Index)
        {
            FFacilityDescriptor Descriptor;
            Descriptor.Name = FString::Printf(TEXT("%s %d"), *Def.Name, Index + 1);
            Descriptor.Type = Def.Type;
            Descriptor.Parent = Def.Parent;
            Descriptor.WorkforceProxy = 50 + Rng.NextInt(200);
            Descriptor.Pos = FVector3d(Rng.NextDouble() * 1000.0, Rng.NextDouble() * 1000.0, Rng.NextDouble() * 200.0);
            Result.Add(Descriptor);
        }
    }

    return Result;
}
