#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WorldGenTypes.h"
#include "NhiCatalog.generated.h"

USTRUCT(BlueprintType)
struct FNhiBodyConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NHI")
    EWorldBody Body = EWorldBody::Earth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NHI")
    TArray<ENhiType> Types;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NHI")
    int32 MinCount = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NHI")
    int32 MaxCount = 8;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NHI")
    float BaseActivity = 0.5f;
};

UCLASS(BlueprintType)
class RPGDATA_API UNhiCatalog : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UNhiCatalog();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NHI")
    TArray<FNhiBodyConfig> Bodies;

    TMap<EWorldBody, TArray<FNhiNodeDescriptor>> GenerateNodes(int64 MasterSeed) const;
};
