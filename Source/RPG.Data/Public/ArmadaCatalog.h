#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WorldGenTypes.h"
#include "ArmadaCatalog.generated.h"

USTRUCT(BlueprintType)
struct FFleetSizeRange
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armada")
    int32 Min = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armada")
    int32 Max = 10;
};

USTRUCT(BlueprintType)
struct FArmadaFactionDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armada")
    FName Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armada")
    FString Doctrine;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armada")
    FString TechBias;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armada")
    int32 Seed = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armada")
    FFleetSizeRange FleetSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armada")
    FIntPoint HomeSector = FIntPoint::ZeroValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armada")
    int32 StrengthPerFleet = 10;
};

UCLASS(BlueprintType)
class RPGDATA_API UArmadaCatalog : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UArmadaCatalog();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Armada")
    TArray<FArmadaFactionDefinition> Factions;
};
