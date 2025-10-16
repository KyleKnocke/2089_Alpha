#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WorldGenTypes.h"
#include "Gr0cFacilityCatalog.generated.h"

USTRUCT(BlueprintType)
struct FGr0cFacilityDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gr0c")
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gr0c")
    EFacilityType Type = EFacilityType::Mining;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gr0c")
    EWorldBody Parent = EWorldBody::Io;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gr0c")
    int32 MinCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gr0c")
    int32 MaxCount = 3;
};

UCLASS(BlueprintType)
class RPGDATA_API UGr0cFacilityCatalog : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UGr0cFacilityCatalog();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gr0c")
    TArray<FGr0cFacilityDefinition> Facilities;

    TArray<FFacilityDescriptor> GenerateFacilities(int64 MasterSeed) const;
};
