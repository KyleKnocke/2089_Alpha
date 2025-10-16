#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FloatingOrigin.generated.h"

/** Helper for maintaining floating origin precision. */
UCLASS()
class WORLDGEN_API UFOHelper_FloatingOrigin : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "FloatingOrigin")
    static bool MaybeRebase(APlayerController* PlayerController, const FVector& NewLocalOrigin, double ThresholdMeters = 200000.0);
};
