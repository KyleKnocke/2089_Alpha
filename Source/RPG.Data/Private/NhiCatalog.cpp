#include "NhiCatalog.h"

#include "AstroRNG.h"

UNhiCatalog::UNhiCatalog()
{
    FNhiBodyConfig EarthConfig;
    EarthConfig.Body = EWorldBody::Earth;
    EarthConfig.Types = {ENhiType::SensorMesh, ENhiType::AcousticPhantom, ENhiType::MycelialMat};
    EarthConfig.MinCount = 8;
    EarthConfig.MaxCount = 20;
    EarthConfig.BaseActivity = 0.6f;

    FNhiBodyConfig MoonConfig;
    MoonConfig.Body = EWorldBody::Moon;
    MoonConfig.Types = {ENhiType::SubsurfaceNode};
    MoonConfig.MinCount = 3;
    MoonConfig.MaxCount = 7;
    MoonConfig.BaseActivity = 0.8f;

    Bodies = {EarthConfig, MoonConfig};
}

TMap<EWorldBody, TArray<FNhiNodeDescriptor>> UNhiCatalog::GenerateNodes(const int64 MasterSeed) const
{
    TMap<EWorldBody, TArray<FNhiNodeDescriptor>> Result;

    for (const FNhiBodyConfig& Config : Bodies)
    {
        FAstroPCG64 Rng(AstroHash(static_cast<uint64>(MasterSeed) ^ 0xBEEF'CAFEull, static_cast<uint64>(Config.Body)));
        const int32 Count = Config.MinCount + Rng.NextInt(FMath::Max(1, Config.MaxCount - Config.MinCount + 1));
        TArray<FNhiNodeDescriptor> Nodes;
        Nodes.Reserve(Count);

        for (int32 Index = 0; Index < Count; ++Index)
        {
            const int32 TypeIndex = Config.Types.Num() > 0 ? Rng.NextInt(Config.Types.Num()) : 0;

            FNhiNodeDescriptor Node;
            Node.Parent = Config.Body;
            Node.Type = Config.Types.IsValidIndex(TypeIndex) ? Config.Types[TypeIndex] : ENhiType::SensorMesh;
            Node.Activity = Config.BaseActivity + static_cast<float>(Rng.NextDouble() * 0.4 - 0.2);
            Node.Pos = FVector3d(Rng.NextDouble() * 1000.0, Rng.NextDouble() * 1000.0, -Rng.NextDouble() * 200.0);
            Nodes.Add(Node);
        }

        Result.Add(Config.Body, Nodes);
    }

    return Result;
}
