#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GodOverlayWidget.generated.h"

class UTextBlock;
class UVerticalBox;
class UHorizontalBox;
class AActor;
enum class EGodSpeedTier : uint8;

/**
 * Minimal overlay widget for God-Mode pawn diagnostic readouts.
 */
UCLASS()
class DEVTOOLS_API UGodOverlayWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UGodOverlayWidget(const FObjectInitializer& ObjectInitializer);

    virtual void NativeConstruct() override;

    /** Display speed tier and scalar. */
    void SetSpeedInfo(EGodSpeedTier Tier, float Scalar);

    /** Display the current global time dilation. */
    void SetTimeScale(float Scale);

    /** Display current position and sector info. */
    void SetPositionInfo(const FVector& LocationCm);

    /** Show or hide the overlay. */
    void SetOverlayVisible(bool bVisible);

    /** Show info about the object under cursor. */
    void SetCursorInfo(AActor* Actor, float Distance);

    /** Show toggles for debug draw features. */
    void SetDrawToggles(bool bArmadas, bool bGr0c, bool bNhi, bool bSectors);

protected:
    void UpdateTextBlock(UTextBlock* Block, const FString& InText);

private:
    UPROPERTY(Transient)
    UTextBlock* SpeedText;

    UPROPERTY(Transient)
    UTextBlock* TimeText;

    UPROPERTY(Transient)
    UTextBlock* PositionText;

    UPROPERTY(Transient)
    UTextBlock* CursorText;

    UPROPERTY(Transient)
    UTextBlock* TogglesText;
};

