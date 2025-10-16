#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "A2089GameMode.generated.h"

class AA2089PlayerController;

UENUM(BlueprintType)
enum class EGalaxyViewMode : uint8
{
    Galaxy,
    Sector,
    System
};

/**
 * Minimal game mode responsible for keeping track of galaxy/sector/system view state.
 */
UCLASS()
class A2089_API AA2089GameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AA2089GameMode();

    UFUNCTION(BlueprintCallable, Category = "View")
    void SetViewMode(EGalaxyViewMode NewMode);

    UFUNCTION(BlueprintCallable, Category = "View")
    EGalaxyViewMode GetViewMode() const { return CurrentMode; }

protected:
    virtual void BeginPlay() override;

private:
    /** Cached pointer to the player controller to coordinate transitions. */
    TWeakObjectPtr<AA2089PlayerController> CachedPlayerController;

    /** Currently active view mode. */
    UPROPERTY(VisibleAnywhere, Category = "View")
    EGalaxyViewMode CurrentMode;
};
