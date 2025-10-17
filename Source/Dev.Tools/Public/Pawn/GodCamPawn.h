#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "GodCamPawn.generated.h"

class UBillboardComponent;
class UCameraComponent;
class UEnhancedInputComponent;
class UInputAction;
struct FInputActionInstance;
class UInputMappingContext;
class UGodOverlayWidget;
class UGodBookmarkSubsystem;
class UGodDrawSubsystem;
class UGodToolsSettings;

UENUM(BlueprintType)
enum class EGodSpeedTier : uint8
{
    Slow = 0,
    Normal,
    Fast,
    Ludicrous
};

USTRUCT(BlueprintType)
struct FGodBookmarkHandle
{
    GENERATED_BODY()

    UPROPERTY()
    int32 Index = INDEX_NONE;
};

/**
 * Free-fly inspector pawn used by the God-Mode dev tools.
 */
UCLASS(NotPlaceable)
class DEVTOOLS_API AGodCamPawn : public APawn
{
    GENERATED_BODY()

public:
    AGodCamPawn(const FObjectInitializer& ObjectInitializer);

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void PossessedBy(AController* NewController) override;
    virtual void UnPossessed() override;

    /** Set the movement tier directly. */
    void SetSpeedTier(EGodSpeedTier InTier);

    /** Adjust the scalar that multiplies the speed tier. */
    void SetSpeedScalar(float Scalar);

    /** Cycle through speed tiers in ascending order. */
    void CycleSpeedTier();

    /** Adjust speed scalar by delta. */
    void AdjustSpeedScalar(float DeltaScalar);

    /** Apply a new global time dilation. */
    void ApplyTimeScale(float NewScale);

    /** Toggle pause / frame-step mode. */
    void TogglePauseMode();

    /** Request a single-frame step while paused. */
    void RequestFrameStep();

    /** Bind the mapping context when possessed. */
    void AddMappingContext();

    /** Remove the mapping context when unpossessed. */
    void RemoveMappingContext();

    /** Teleport the camera to the current cursor hit target. */
    void TeleportToCursor();

    /** Set bookmark index with optional name. */
    void SetBookmark(int32 Index, const FString& OptionalName);

    /** Jump to bookmark index if available. */
    void GotoBookmark(int32 Index);

    /** Exec: change speed tier or scalar. */
    UFUNCTION(Exec)
    void GodSpeed(const FString& InValue);

    /** Exec: set time scale. */
    UFUNCTION(Exec)
    void GodTimeScale(float Scale);

    /** Exec: set bookmark. */
    UFUNCTION(Exec)
    void GodBookmarkSet(int32 Index, const FString& Name = TEXT(""));

    /** Exec: goto bookmark. */
    UFUNCTION(Exec)
    void GodBookmarkGoto(int32 Index);

    /** Exec: toggle overlay. */
    UFUNCTION(Exec)
    void GodOverlayToggle();

    /** Exec helper to toggle draw flags. */
    UFUNCTION(Exec)
    void GodToggleFlag(const FString& FlagName);

protected:
    void InitializeInputAssets();

    void MoveXY(const FInputActionValue& Value);
    void MoveZ(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void OnSpeedCycle(const FInputActionValue& Value);
    void OnSpeedUp(const FInputActionValue& Value);
    void OnSpeedDown(const FInputActionValue& Value);
    void OnToggleOverlay(const FInputActionValue& Value);
    void OnTogglePause(const FInputActionValue& Value);
    void OnTimeSlow(const FInputActionValue& Value);
    void OnTimeNormal(const FInputActionValue& Value);
    void OnTimeFast(const FInputActionValue& Value);
    void OnTimeLudicrous(const FInputActionValue& Value);
    void OnFrameStep(const FInputActionValue& Value);
    void OnPickUnderCursor(const FInputActionValue& Value);
    void OnTeleportToCursor(const FInputActionValue& Value);
    void OnBookmarkModifier(const FInputActionValue& Value);
    void OnBookmarkNumber(const FInputActionInstance& Instance);
    void OnJumpToCoords(const FInputActionValue& Value);
    void OnToggleArmadas(const FInputActionValue& Value);
    void OnToggleGr0c(const FInputActionValue& Value);
    void OnToggleNhi(const FInputActionValue& Value);
    void OnToggleSectors(const FInputActionValue& Value);

    void UpdateOverlay(float DeltaSeconds);

    void UpdateCursorTrace();

    void ApplyMovement(float DeltaSeconds);

    void UpdateTimeDilation(float DeltaSeconds);

    double GetBaseSpeed(EGodSpeedTier Tier) const;

    FVector GetForwardMovementVector() const;

    FVector GetRightMovementVector() const;

    FVector GetUpMovementVector() const;

private:
    UPROPERTY()
    USceneComponent* Root;

    UPROPERTY()
    UCameraComponent* Camera;

    UPROPERTY()
    UBillboardComponent* CursorMarker;

    UPROPERTY(Transient)
    UInputMappingContext* MappingContext;

    UPROPERTY(Transient)
    TMap<FName, UInputAction*> ActionMap;

    TWeakObjectPtr<UGodOverlayWidget> OverlayWidget;

    FVector2D MoveInputXY;
    float MoveInputZ;
    FVector2D LookInput;

    EGodSpeedTier CurrentTier;
    float SpeedScalar;
    float MouseSensitivity;
    bool bOverlayVisible;
    bool bPauseMode;
    float PendingFrameTime;

    float CachedTimeScale;

    bool bAltModifier;

    FHitResult CachedCursorHit;
    TWeakObjectPtr<AActor> HighlightedActor;

    TMap<const UInputAction*, int32> BookmarkIndices;

    bool bToggleArmadas;
    bool bToggleGr0c;
    bool bToggleNhi;
    bool bToggleSectors;
};

