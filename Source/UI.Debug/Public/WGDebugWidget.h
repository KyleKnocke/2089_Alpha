#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WGDebugWidget.generated.h"

class UTextBlock;

/** Debug overlay showing galaxy information. */
UCLASS()
class UIDEBUG_API UWGDebugWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
    FString BuildDebugText() const;

    UPROPERTY()
    TObjectPtr<UTextBlock> InfoText;
};
