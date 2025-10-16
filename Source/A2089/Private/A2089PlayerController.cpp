#include "A2089PlayerController.h"

#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "UI/Debug/WGDebugWidget.h"
#include "WorldGenSubsystem.h"

#define LOCTEXT_NAMESPACE "AA2089PlayerController"

AA2089PlayerController::AA2089PlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
}

void AA2089PlayerController::BeginPlay()
{
    Super::BeginPlay();

    EnsureDebugWidget();

    if (MappingContext)
    {
        if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                Subsystem->AddMappingContext(MappingContext, 0);
            }
        }
    }
}

void AA2089PlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
    if (!EnhancedInput)
    {
        return;
    }

    if (!ToggleAction)
    {
        ToggleAction = NewObject<UInputAction>(this, TEXT("ToggleDebugAction"));
        ToggleAction->ValueType = EInputActionValueType::Boolean;
    }

    if (!MappingContext)
    {
        MappingContext = NewObject<UInputMappingContext>(this, TEXT("A2089Mapping"));
        MappingContext->MapKey(ToggleAction, EKeys::F1);
    }

    EnhancedInput->BindAction(ToggleAction, ETriggerEvent::Started, this, &AA2089PlayerController::HandleToggleDebug);
}

void AA2089PlayerController::ToggleDebugOverlay()
{
    EnsureDebugWidget();

    if (DebugWidget.IsValid())
    {
        UUserWidget* Widget = DebugWidget.Get();
        const bool bVisible = Widget->IsInViewport();
        if (bVisible)
        {
            Widget->RemoveFromParent();
        }
        else
        {
            Widget->AddToViewport(100);
        }
    }
}

void AA2089PlayerController::EnsureDebugWidget()
{
    if (!DebugWidgetClass)
    {
        DebugWidgetClass = LoadClass<UUserWidget>(nullptr, TEXT("Class'/Script/UIDebug.WGDebugWidget'"));
        if (!DebugWidgetClass)
        {
            return;
        }
    }

    if (!DebugWidget.IsValid() && DebugWidgetClass)
    {
        DebugWidget = CreateWidget<UWGDebugWidget>(this, DebugWidgetClass);
        if (DebugWidget.IsValid())
        {
            DebugWidget->AddToViewport(100);
        }
    }
}

void AA2089PlayerController::HandleToggleDebug(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        ToggleDebugOverlay();
    }
}

#undef LOCTEXT_NAMESPACE
