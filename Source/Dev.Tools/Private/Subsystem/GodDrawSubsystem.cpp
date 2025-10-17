#include "Subsystem/GodDrawSubsystem.h"

#include "DrawDebugHelpers.h"

void UGodDrawSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    bDrawArmadas = false;
    bDrawGr0c = false;
    bDrawNhi = false;
    bDrawSectors = false;
}

void UGodDrawSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

void UGodDrawSubsystem::SetDrawFlags(bool bArmadas, bool bGr0cIn, bool bNhiIn, bool bSectorsIn)
{
    bDrawArmadas = bArmadas;
    bDrawGr0c = bGr0cIn;
    bDrawNhi = bNhiIn;
    bDrawSectors = bSectorsIn;
}

void UGodDrawSubsystem::Draw(const FVector& CameraLocation)
{
    if (bDrawSectors)
    {
        DrawSectorGrid(CameraLocation);
    }

    if (bDrawArmadas)
    {
        DrawDebugSphere(GetWorld(), CameraLocation + FVector(1000.f, 0.f, 0.f), 2000.f, 16, FColor::Cyan, false, -1.f, 0, 5.f);
    }

    if (bDrawGr0c)
    {
        DrawDebugBox(GetWorld(), CameraLocation + FVector(0.f, 1000.f, 0.f), FVector(500.f), FColor::Orange, false, -1.f, 0, 5.f);
    }

    if (bDrawNhi)
    {
        DrawDebugPoint(GetWorld(), CameraLocation + FVector(0.f, 0.f, 1000.f), 20.f, FColor::Green, false, -1.f);
    }
}

void UGodDrawSubsystem::DrawSectorGrid(const FVector& CameraLocation)
{
    const float GridSize = 100000.f;
    const int32 HalfCells = 5;

    for (int32 X = -HalfCells; X <= HalfCells; ++X)
    {
        const FVector Start = CameraLocation + FVector(X * GridSize, -HalfCells * GridSize, 0.f);
        const FVector End = CameraLocation + FVector(X * GridSize, HalfCells * GridSize, 0.f);
        DrawDebugLine(GetWorld(), Start, End, FColor::Yellow, false, -1.f, 0, 2.f);
    }

    for (int32 Y = -HalfCells; Y <= HalfCells; ++Y)
    {
        const FVector Start = CameraLocation + FVector(-HalfCells * GridSize, Y * GridSize, 0.f);
        const FVector End = CameraLocation + FVector(HalfCells * GridSize, Y * GridSize, 0.f);
        DrawDebugLine(GetWorld(), Start, End, FColor::Yellow, false, -1.f, 0, 2.f);
    }
}

