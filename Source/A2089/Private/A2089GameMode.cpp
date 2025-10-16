#include "A2089GameMode.h"

#include "A2089PlayerController.h"
#include "Engine/World.h"

AA2089GameMode::AA2089GameMode()
{
    PlayerControllerClass = AA2089PlayerController::StaticClass();
    CurrentMode = EGalaxyViewMode::Galaxy;
}

void AA2089GameMode::BeginPlay()
{
    Super::BeginPlay();

    CachedPlayerController = Cast<AA2089PlayerController>(GetWorld()->GetFirstPlayerController());
}

void AA2089GameMode::SetViewMode(const EGalaxyViewMode NewMode)
{
    if (CurrentMode == NewMode)
    {
        return;
    }

    CurrentMode = NewMode;

    if (CachedPlayerController.IsValid())
    {
        CachedPlayerController->ToggleDebugOverlay();
    }
}
