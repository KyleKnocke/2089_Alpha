#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GodDrawSubsystem.generated.h"

/**
 * Debug drawing helpers for the God-Mode tools. Currently renders simple placeholders.
 */
UCLASS()
class DEVTOOLS_API UGodDrawSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /** Configure which overlays are active. */
    void SetDrawFlags(bool bArmadas, bool bGr0c, bool bNhi, bool bSectors);

    /** Draw debug overlays for the current camera position. */
    void Draw(const FVector& CameraLocation);

private:
    void DrawSectorGrid(const FVector& CameraLocation);

    bool bDrawArmadas;
    bool bDrawGr0c;
    bool bDrawNhi;
    bool bDrawSectors;
};

