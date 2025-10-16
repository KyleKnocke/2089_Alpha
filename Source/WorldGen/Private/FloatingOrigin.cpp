#include "FloatingOrigin.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"

bool UFOHelper_FloatingOrigin::MaybeRebase(APlayerController* PlayerController, const FVector& NewLocalOrigin, const double ThresholdMeters)
{
    if (!PlayerController)
    {
        return false;
    }

    UWorld* World = PlayerController->GetWorld();
    if (!World)
    {
        return false;
    }

    const FVector Location = PlayerController->GetPawn() ? PlayerController->GetPawn()->GetActorLocation() : FVector::ZeroVector;
    if (Location.Size() < ThresholdMeters)
    {
        return false;
    }

    const FIntVector NewOrigin = FIntVector(FMath::RoundToInt(NewLocalOrigin.X), FMath::RoundToInt(NewLocalOrigin.Y), FMath::RoundToInt(NewLocalOrigin.Z));
    World->SetNewWorldOrigin(NewOrigin);
    return true;
}
