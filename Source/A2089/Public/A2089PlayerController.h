#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "A2089PlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UUserWidget;
class UWGDebugWidget;

/**
 * Player controller that wires input to the world generation subsystem and debug overlay.
 */
UCLASS()
class A2089_API AA2089PlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AA2089PlayerController();

    virtual void SetupInputComponent() override;
    virtual void BeginPlay() override;

    /** Toggles the debug overlay visibility. */
    void ToggleDebugOverlay();

private:
    /** Spawn or reuse a debug widget instance. */
    void EnsureDebugWidget();

    /** Callback for the toggle debug input action. */
    void HandleToggleDebug(const FInputActionValue& Value);

    /** Debug widget instance. */
    TWeakObjectPtr<UWGDebugWidget> DebugWidget;

    /** Cached widget class resolved from the UI.Debug module. */
    UPROPERTY()
    TSubclassOf<UUserWidget> DebugWidgetClass;

    /** Enhanced Input mapping context used by this controller. */
    UPROPERTY()
    TObjectPtr<UInputMappingContext> MappingContext;

    /** Toggle action handle. */
    UPROPERTY()
    TObjectPtr<UInputAction> ToggleAction;
};
