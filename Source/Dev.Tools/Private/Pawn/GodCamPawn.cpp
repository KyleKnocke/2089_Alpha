#include "Pawn/GodCamPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/SceneComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputActionInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystem/GodBookmarkSubsystem.h"
#include "Subsystem/GodDrawSubsystem.h"
#include "UI/GodOverlayWidget.h"

namespace
{
    constexpr double SlowSpeedCmPerSec = 50.0;          // 0.5 m/s
    constexpr double NormalSpeedCmPerSec = 1000.0;      // 10 m/s
    constexpr double FastSpeedCmPerSec = 10000.0;       // 100 m/s
    constexpr double LudicrousSpeedCmPerSec = 1000000.0; // 10,000 m/s

    constexpr float MinSpeedScalar = 0.01f;
    constexpr float MaxSpeedScalar = 100.0f;
}

AGodCamPawn::AGodCamPawn(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , MoveInputXY(FVector2D::ZeroVector)
    , MoveInputZ(0.f)
    , LookInput(FVector2D::ZeroVector)
    , CurrentTier(EGodSpeedTier::Normal)
    , SpeedScalar(1.f)
    , MouseSensitivity(0.08f)
    , bOverlayVisible(true)
    , bPauseMode(false)
    , PendingFrameTime(0.f)
    , CachedTimeScale(1.f)
    , bAltModifier(false)
    , bToggleArmadas(false)
    , bToggleGr0c(false)
    , bToggleNhi(false)
    , bToggleSectors(false)
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup = TG_PrePhysics;

    Root = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));
    SetRootComponent(Root);

    Camera = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("Camera"));
    Camera->SetupAttachment(Root);

    CursorMarker = ObjectInitializer.CreateDefaultSubobject<UBillboardComponent>(this, TEXT("CursorMarker"));
    CursorMarker->SetupAttachment(Root);
    CursorMarker->SetHiddenInGame(true);

    InitializeInputAssets();
}

void AGodCamPawn::InitializeInputAssets()
{
    if (MappingContext)
    {
        return;
    }

    MappingContext = CreateDefaultSubobject<UInputMappingContext>(TEXT("IMC_GODCAM"));
    MappingContext->Priority = 100;

    auto CreateAction = [this](const FName& Name, EInputActionValueType ValueType, bool bTriggerWhenPaused = true) -> UInputAction*
    {
        UInputAction* Action = CreateDefaultSubobject<UInputAction>(*Name.ToString());
        Action->ValueType = ValueType;
        Action->bTriggerWhenPaused = bTriggerWhenPaused;
        ActionMap.Add(Name, Action);
        return Action;
    };

    UInputAction* MoveXYAction = CreateAction(TEXT("IA_MoveXY"), EInputActionValueType::Axis2D);
    UInputAction* MoveZAction = CreateAction(TEXT("IA_MoveZ"), EInputActionValueType::Axis1D);
    UInputAction* LookAction = CreateAction(TEXT("IA_Look"), EInputActionValueType::Axis2D);
    UInputAction* SpeedCycleAction = CreateAction(TEXT("IA_SpeedCycle"), EInputActionValueType::Boolean);
    UInputAction* SpeedUpAction = CreateAction(TEXT("IA_SpeedUp"), EInputActionValueType::Boolean);
    UInputAction* SpeedDownAction = CreateAction(TEXT("IA_SpeedDown"), EInputActionValueType::Boolean);
    UInputAction* ToggleOverlayAction = CreateAction(TEXT("IA_ToggleOverlay"), EInputActionValueType::Boolean);
    UInputAction* TogglePauseAction = CreateAction(TEXT("IA_Pause"), EInputActionValueType::Boolean);
    UInputAction* TimeSlowAction = CreateAction(TEXT("IA_TimeSlow"), EInputActionValueType::Boolean);
    UInputAction* TimeNormalAction = CreateAction(TEXT("IA_TimeNormal"), EInputActionValueType::Boolean);
    UInputAction* TimeFastAction = CreateAction(TEXT("IA_TimeFast"), EInputActionValueType::Boolean);
    UInputAction* TimeLudicrousAction = CreateAction(TEXT("IA_TimeLudicrous"), EInputActionValueType::Boolean);
    UInputAction* FrameStepAction = CreateAction(TEXT("IA_FrameStep"), EInputActionValueType::Boolean);
    UInputAction* PickAction = CreateAction(TEXT("IA_PickUnderCursor"), EInputActionValueType::Boolean);
    UInputAction* TeleportAction = CreateAction(TEXT("IA_TeleportToCursor"), EInputActionValueType::Boolean);
    UInputAction* BookmarkAltAction = CreateAction(TEXT("IA_BookmarkSet"), EInputActionValueType::Boolean);
    for (int32 Index = 0; Index <= 9; ++Index)
    {
        const FName ActionName = *FString::Printf(TEXT("IA_BookmarkGoto_%d"), Index);
        if (UInputAction* Action = CreateAction(ActionName, EInputActionValueType::Boolean))
        {
            BookmarkIndices.Add(Action, Index);
        }
    }
    UInputAction* JumpCoordsAction = CreateAction(TEXT("IA_JumpToCoords"), EInputActionValueType::Boolean);
    UInputAction* ToggleArmadasAction = CreateAction(TEXT("IA_ToggleArmadas"), EInputActionValueType::Boolean);
    UInputAction* ToggleGr0cAction = CreateAction(TEXT("IA_ToggleGr0c"), EInputActionValueType::Boolean);
    UInputAction* ToggleNhiAction = CreateAction(TEXT("IA_ToggleNHI"), EInputActionValueType::Boolean);
    UInputAction* ToggleSectorsAction = CreateAction(TEXT("IA_ToggleSectors"), EInputActionValueType::Boolean);

    if (MoveXYAction)
    {
        MappingContext->MapKey(MoveXYAction, EKeys::W).Scale = FVector(0.f, 1.f, 0.f);
        MappingContext->MapKey(MoveXYAction, EKeys::S).Scale = FVector(0.f, -1.f, 0.f);
        MappingContext->MapKey(MoveXYAction, EKeys::D).Scale = FVector(1.f, 0.f, 0.f);
        MappingContext->MapKey(MoveXYAction, EKeys::A).Scale = FVector(-1.f, 0.f, 0.f);
    }

    if (MoveZAction)
    {
        MappingContext->MapKey(MoveZAction, EKeys::SpaceBar).Scale = FVector(1.f, 0.f, 0.f);
        MappingContext->MapKey(MoveZAction, EKeys::LeftControl).Scale = FVector(-1.f, 0.f, 0.f);
        MappingContext->MapKey(MoveZAction, EKeys::RightControl).Scale = FVector(-1.f, 0.f, 0.f);
    }

    if (LookAction)
    {
        MappingContext->MapKey(LookAction, EKeys::MouseX).Scale = FVector(1.f, 0.f, 0.f);
        MappingContext->MapKey(LookAction, EKeys::MouseY).Scale = FVector(0.f, -1.f, 0.f);
    }

    if (SpeedCycleAction)
    {
        MappingContext->MapKey(SpeedCycleAction, EKeys::LeftShift);
        MappingContext->MapKey(SpeedCycleAction, EKeys::RightShift);
    }

    if (SpeedUpAction)
    {
        MappingContext->MapKey(SpeedUpAction, EKeys::MouseScrollUp);
    }

    if (SpeedDownAction)
    {
        MappingContext->MapKey(SpeedDownAction, EKeys::MouseScrollDown);
    }

    if (ToggleOverlayAction)
    {
        MappingContext->MapKey(ToggleOverlayAction, EKeys::F1);
    }

    if (TogglePauseAction)
    {
        MappingContext->MapKey(TogglePauseAction, EKeys::Zero);
    }

    if (TimeSlowAction)
    {
        MappingContext->MapKey(TimeSlowAction, EKeys::One);
    }
    if (TimeNormalAction)
    {
        MappingContext->MapKey(TimeNormalAction, EKeys::Two);
    }
    if (TimeFastAction)
    {
        MappingContext->MapKey(TimeFastAction, EKeys::Three);
    }
    if (TimeLudicrousAction)
    {
        MappingContext->MapKey(TimeLudicrousAction, EKeys::Four);
    }

    if (FrameStepAction)
    {
        MappingContext->MapKey(FrameStepAction, EKeys::Period);
    }

    if (PickAction)
    {
        MappingContext->MapKey(PickAction, EKeys::LeftMouseButton);
    }
    if (TeleportAction)
    {
        MappingContext->MapKey(TeleportAction, EKeys::MiddleMouseButton);
    }

    if (BookmarkAltAction)
    {
        MappingContext->MapKey(BookmarkAltAction, EKeys::LeftAlt);
        MappingContext->MapKey(BookmarkAltAction, EKeys::RightAlt);
    }

    const TPair<FName, FKey> BookmarkKeyPairs[] = {
        {TEXT("IA_BookmarkGoto_1"), EKeys::One},
        {TEXT("IA_BookmarkGoto_2"), EKeys::Two},
        {TEXT("IA_BookmarkGoto_3"), EKeys::Three},
        {TEXT("IA_BookmarkGoto_4"), EKeys::Four},
        {TEXT("IA_BookmarkGoto_5"), EKeys::Five},
        {TEXT("IA_BookmarkGoto_6"), EKeys::Six},
        {TEXT("IA_BookmarkGoto_7"), EKeys::Seven},
        {TEXT("IA_BookmarkGoto_8"), EKeys::Eight},
        {TEXT("IA_BookmarkGoto_9"), EKeys::Nine},
        {TEXT("IA_BookmarkGoto_0"), EKeys::Zero}
    };

    for (const TPair<FName, FKey>& Pair : BookmarkKeyPairs)
    {
        if (UInputAction* const* ActionPtr = ActionMap.Find(Pair.Key))
        {
            if (UInputAction* Action = *ActionPtr)
            {
                MappingContext->MapKey(Action, Pair.Value);
            }
        }
    }

    if (JumpCoordsAction)
    {
        MappingContext->MapKey(JumpCoordsAction, EKeys::J);
    }

    if (ToggleArmadasAction)
    {
        MappingContext->MapKey(ToggleArmadasAction, EKeys::F5);
    }
    if (ToggleGr0cAction)
    {
        MappingContext->MapKey(ToggleGr0cAction, EKeys::F6);
    }
    if (ToggleNhiAction)
    {
        MappingContext->MapKey(ToggleNhiAction, EKeys::F7);
    }
    if (ToggleSectorsAction)
    {
        MappingContext->MapKey(ToggleSectorsAction, EKeys::F8);
    }
}

void AGodCamPawn::BeginPlay()
{
    Super::BeginPlay();
}

void AGodCamPawn::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    ApplyMovement(DeltaSeconds);
    UpdateCursorTrace();
    UpdateTimeDilation(DeltaSeconds);
    UpdateOverlay(DeltaSeconds);

    if (UWorld* World = GetWorld())
    {
        if (UGodDrawSubsystem* DrawSubsystem = World->GetSubsystem<UGodDrawSubsystem>())
        {
            DrawSubsystem->SetDrawFlags(bToggleArmadas, bToggleGr0c, bToggleNhi, bToggleSectors);
            DrawSubsystem->Draw(GetActorLocation());
        }
    }
}

void AGodCamPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (!EnhancedInput)
    {
        return;
    }

    auto BindAction = [this, EnhancedInput](const FName& Name, ETriggerEvent Trigger, void (AGodCamPawn::*Method)(const FInputActionValue&))
    {
        if (UInputAction* Action = ActionMap.FindRef(Name))
        {
            EnhancedInput->BindAction(Action, Trigger, this, Method);
        }
    };

    BindAction(TEXT("IA_MoveXY"), ETriggerEvent::Triggered, &AGodCamPawn::MoveXY);
    BindAction(TEXT("IA_MoveXY"), ETriggerEvent::Completed, &AGodCamPawn::MoveXY);
    BindAction(TEXT("IA_MoveZ"), ETriggerEvent::Triggered, &AGodCamPawn::MoveZ);
    BindAction(TEXT("IA_MoveZ"), ETriggerEvent::Completed, &AGodCamPawn::MoveZ);
    BindAction(TEXT("IA_Look"), ETriggerEvent::Triggered, &AGodCamPawn::Look);
    BindAction(TEXT("IA_SpeedCycle"), ETriggerEvent::Started, &AGodCamPawn::OnSpeedCycle);
    BindAction(TEXT("IA_SpeedUp"), ETriggerEvent::Started, &AGodCamPawn::OnSpeedUp);
    BindAction(TEXT("IA_SpeedDown"), ETriggerEvent::Started, &AGodCamPawn::OnSpeedDown);
    BindAction(TEXT("IA_ToggleOverlay"), ETriggerEvent::Started, &AGodCamPawn::OnToggleOverlay);
    BindAction(TEXT("IA_Pause"), ETriggerEvent::Started, &AGodCamPawn::OnTogglePause);
    BindAction(TEXT("IA_TimeSlow"), ETriggerEvent::Started, &AGodCamPawn::OnTimeSlow);
    BindAction(TEXT("IA_TimeNormal"), ETriggerEvent::Started, &AGodCamPawn::OnTimeNormal);
    BindAction(TEXT("IA_TimeFast"), ETriggerEvent::Started, &AGodCamPawn::OnTimeFast);
    BindAction(TEXT("IA_TimeLudicrous"), ETriggerEvent::Started, &AGodCamPawn::OnTimeLudicrous);
    BindAction(TEXT("IA_FrameStep"), ETriggerEvent::Started, &AGodCamPawn::OnFrameStep);
    BindAction(TEXT("IA_PickUnderCursor"), ETriggerEvent::Started, &AGodCamPawn::OnPickUnderCursor);
    BindAction(TEXT("IA_TeleportToCursor"), ETriggerEvent::Started, &AGodCamPawn::OnTeleportToCursor);
    BindAction(TEXT("IA_BookmarkSet"), ETriggerEvent::Started, &AGodCamPawn::OnBookmarkModifier);
    BindAction(TEXT("IA_BookmarkSet"), ETriggerEvent::Completed, &AGodCamPawn::OnBookmarkModifier);

    for (const TPair<const UInputAction*, int32>& Pair : BookmarkIndices)
    {
        if (UInputAction* Action = const_cast<UInputAction*>(Pair.Key))
        {
            EnhancedInput->BindAction(Action, ETriggerEvent::Started, this, &AGodCamPawn::OnBookmarkNumber);
        }
    }

    BindAction(TEXT("IA_JumpToCoords"), ETriggerEvent::Started, &AGodCamPawn::OnJumpToCoords);
    BindAction(TEXT("IA_ToggleArmadas"), ETriggerEvent::Started, &AGodCamPawn::OnToggleArmadas);
    BindAction(TEXT("IA_ToggleGr0c"), ETriggerEvent::Started, &AGodCamPawn::OnToggleGr0c);
    BindAction(TEXT("IA_ToggleNHI"), ETriggerEvent::Started, &AGodCamPawn::OnToggleNhi);
    BindAction(TEXT("IA_ToggleSectors"), ETriggerEvent::Started, &AGodCamPawn::OnToggleSectors);
}

void AGodCamPawn::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    AddMappingContext();
}

void AGodCamPawn::UnPossessed()
{
    RemoveMappingContext();

    Super::UnPossessed();
}

void AGodCamPawn::AddMappingContext()
{
    if (!MappingContext)
    {
        return;
    }

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (ULocalPlayer* LP = PC->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                Subsystem->AddMappingContext(MappingContext, 100);
            }
        }
    }
}

void AGodCamPawn::RemoveMappingContext()
{
    if (!MappingContext)
    {
        return;
    }

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (ULocalPlayer* LP = PC->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                Subsystem->RemoveMappingContext(MappingContext);
            }
        }
    }
}

void AGodCamPawn::SetSpeedTier(EGodSpeedTier InTier)
{
    CurrentTier = InTier;
}

void AGodCamPawn::SetSpeedScalar(float Scalar)
{
    SpeedScalar = FMath::Clamp(Scalar, MinSpeedScalar, MaxSpeedScalar);
}

void AGodCamPawn::CycleSpeedTier()
{
    const int32 NextIndex = (static_cast<int32>(CurrentTier) + 1) % 4;
    CurrentTier = static_cast<EGodSpeedTier>(NextIndex);
}

void AGodCamPawn::AdjustSpeedScalar(float DeltaScalar)
{
    SetSpeedScalar(SpeedScalar + DeltaScalar);
}

void AGodCamPawn::ApplyTimeScale(float NewScale)
{
    CachedTimeScale = NewScale;
    if (UWorld* World = GetWorld())
    {
        UGameplayStatics::SetGlobalTimeDilation(World, FMath::Max(0.f, NewScale));
    }
}

void AGodCamPawn::TogglePauseMode()
{
    bPauseMode = !bPauseMode;
    ApplyTimeScale(bPauseMode ? 0.f : 1.f);
}

void AGodCamPawn::RequestFrameStep()
{
    if (!bPauseMode)
    {
        return;
    }

    PendingFrameTime = 1.f / 60.f;
    if (UWorld* World = GetWorld())
    {
        UGameplayStatics::SetGlobalTimeDilation(World, 1.f);
    }
}

void AGodCamPawn::TeleportToCursor()
{
    UpdateCursorTrace();

    FVector TargetLocation = GetActorLocation() + GetActorForwardVector() * 100000.f;
    if (CachedCursorHit.bBlockingHit)
    {
        TargetLocation = CachedCursorHit.ImpactPoint + CachedCursorHit.ImpactNormal * 500.f;
    }

    SetActorLocation(TargetLocation);
}

void AGodCamPawn::SetBookmark(int32 Index, const FString& OptionalName)
{
    if (Index < 0 || Index > 9)
    {
        return;
    }

    if (UWorld* World = GetWorld())
    {
        if (UGodBookmarkSubsystem* Subsystem = World->GetSubsystem<UGodBookmarkSubsystem>())
        {
            Subsystem->SetBookmark(Index, GetActorLocation(), GetActorRotation(), OptionalName);
        }
    }
}

void AGodCamPawn::GotoBookmark(int32 Index)
{
    if (Index < 0 || Index > 9)
    {
        return;
    }

    if (UWorld* World = GetWorld())
    {
        if (UGodBookmarkSubsystem* Subsystem = World->GetSubsystem<UGodBookmarkSubsystem>())
        {
            FVector Location;
            FRotator Rotation;
            if (Subsystem->GetBookmark(Index, Location, Rotation))
            {
                SetActorLocationAndRotation(Location, Rotation);
            }
        }
    }
}

void AGodCamPawn::GodSpeed(const FString& InValue)
{
    if (InValue.Equals(TEXT("slow"), ESearchCase::IgnoreCase))
    {
        SetSpeedTier(EGodSpeedTier::Slow);
    }
    else if (InValue.Equals(TEXT("normal"), ESearchCase::IgnoreCase))
    {
        SetSpeedTier(EGodSpeedTier::Normal);
    }
    else if (InValue.Equals(TEXT("fast"), ESearchCase::IgnoreCase))
    {
        SetSpeedTier(EGodSpeedTier::Fast);
    }
    else if (InValue.Equals(TEXT("ludicrous"), ESearchCase::IgnoreCase))
    {
        SetSpeedTier(EGodSpeedTier::Ludicrous);
    }
    else
    {
        const float Scalar = FCString::Atof(*InValue);
        if (Scalar > 0.f)
        {
            SetSpeedScalar(Scalar);
        }
    }
}

void AGodCamPawn::GodTimeScale(float Scale)
{
    ApplyTimeScale(Scale);
}

void AGodCamPawn::GodBookmarkSet(int32 Index, const FString& Name)
{
    SetBookmark(Index, Name);
}

void AGodCamPawn::GodBookmarkGoto(int32 Index)
{
    GotoBookmark(Index);
}

void AGodCamPawn::GodOverlayToggle()
{
    bOverlayVisible = !bOverlayVisible;
    if (OverlayWidget.IsValid())
    {
        if (bOverlayVisible)
        {
            OverlayWidget->AddToViewport(200);
        }
        else
        {
            OverlayWidget->RemoveFromParent();
        }
    }
}

void AGodCamPawn::GodToggleFlag(const FString& FlagName)
{
    if (FlagName.Equals(TEXT("Armadas"), ESearchCase::IgnoreCase))
    {
        bToggleArmadas = !bToggleArmadas;
    }
    else if (FlagName.Equals(TEXT("Gr0c"), ESearchCase::IgnoreCase))
    {
        bToggleGr0c = !bToggleGr0c;
    }
    else if (FlagName.Equals(TEXT("NHI"), ESearchCase::IgnoreCase))
    {
        bToggleNhi = !bToggleNhi;
    }
    else if (FlagName.Equals(TEXT("Sectors"), ESearchCase::IgnoreCase))
    {
        bToggleSectors = !bToggleSectors;
    }
}

void AGodCamPawn::MoveXY(const FInputActionValue& Value)
{
    MoveInputXY = Value.Get<FVector2D>();
}

void AGodCamPawn::MoveZ(const FInputActionValue& Value)
{
    MoveInputZ = Value.Get<float>();
}

void AGodCamPawn::Look(const FInputActionValue& Value)
{
    LookInput = Value.Get<FVector2D>();
}

void AGodCamPawn::OnSpeedCycle(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        CycleSpeedTier();
    }
}

void AGodCamPawn::OnSpeedUp(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        AdjustSpeedScalar(0.1f);
    }
}

void AGodCamPawn::OnSpeedDown(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        AdjustSpeedScalar(-0.1f);
    }
}

void AGodCamPawn::OnToggleOverlay(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        GodOverlayToggle();
    }
}

void AGodCamPawn::OnTogglePause(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        TogglePauseMode();
    }
}

void AGodCamPawn::OnTimeSlow(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        ApplyTimeScale(0.1f);
        bPauseMode = FMath::IsNearlyZero(0.1f);
    }
}

void AGodCamPawn::OnTimeNormal(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        bPauseMode = false;
        ApplyTimeScale(1.f);
    }
}

void AGodCamPawn::OnTimeFast(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        bPauseMode = false;
        ApplyTimeScale(5.f);
    }
}

void AGodCamPawn::OnTimeLudicrous(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        bPauseMode = false;
        ApplyTimeScale(20.f);
    }
}

void AGodCamPawn::OnFrameStep(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        RequestFrameStep();
    }
}

void AGodCamPawn::OnPickUnderCursor(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        UpdateCursorTrace();
        HighlightedActor = CachedCursorHit.GetActor();
    }
}

void AGodCamPawn::OnTeleportToCursor(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        TeleportToCursor();
    }
}

void AGodCamPawn::OnBookmarkModifier(const FInputActionValue& Value)
{
    bAltModifier = Value.Get<bool>();
}

void AGodCamPawn::OnBookmarkNumber(const FInputActionInstance& Instance)
{
    if (!Instance.GetValue().Get<bool>())
    {
        return;
    }

    const UInputAction* Source = Instance.GetSourceAction();
    if (!Source)
    {
        return;
    }

    if (const int32* IndexPtr = BookmarkIndices.Find(Source))
    {
        const int32 Index = *IndexPtr;
        if (bAltModifier)
        {
            SetBookmark(Index, FString::Printf(TEXT("Bookmark %d"), Index));
        }
        else
        {
            GotoBookmark(Index);
        }
    }
}

void AGodCamPawn::OnJumpToCoords(const FInputActionValue& Value)
{
    if (!Value.Get<bool>())
    {
        return;
    }

    // Placeholder for modal dialog integration.
}

void AGodCamPawn::OnToggleArmadas(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        bToggleArmadas = !bToggleArmadas;
    }
}

void AGodCamPawn::OnToggleGr0c(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        bToggleGr0c = !bToggleGr0c;
    }
}

void AGodCamPawn::OnToggleNhi(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        bToggleNhi = !bToggleNhi;
    }
}

void AGodCamPawn::OnToggleSectors(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        bToggleSectors = !bToggleSectors;
    }
}

void AGodCamPawn::ApplyMovement(float DeltaSeconds)
{
    if (LookInput.SizeSquared() > KINDA_SMALL_NUMBER)
    {
        FRotator NewRot = GetActorRotation();
        NewRot.Yaw += LookInput.X * MouseSensitivity;
        NewRot.Pitch = FMath::Clamp(NewRot.Pitch + LookInput.Y * MouseSensitivity, -89.9f, 89.9f);
        SetActorRotation(NewRot);
    }

    FVector MoveDirection = FVector::ZeroVector;
    MoveDirection += GetForwardMovementVector() * MoveInputXY.Y;
    MoveDirection += GetRightMovementVector() * MoveInputXY.X;
    MoveDirection += GetUpMovementVector() * MoveInputZ;

    if (!MoveDirection.IsNearlyZero())
    {
        const double Speed = GetBaseSpeed(CurrentTier) * SpeedScalar;
        AddActorWorldOffset(MoveDirection * Speed * DeltaSeconds, false);
    }
}

void AGodCamPawn::UpdateCursorTrace()
{
    CachedCursorHit = FHitResult();

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        PC->GetHitResultUnderCursor(TraceTypeQuery1, false, CachedCursorHit);

        if (CursorMarker)
        {
            if (CachedCursorHit.bBlockingHit)
            {
                CursorMarker->SetHiddenInGame(false);
                CursorMarker->SetWorldLocation(CachedCursorHit.Location);
            }
            else
            {
                CursorMarker->SetHiddenInGame(true);
            }
        }
    }
}

void AGodCamPawn::UpdateTimeDilation(float DeltaSeconds)
{
    if (PendingFrameTime > 0.f)
    {
        PendingFrameTime -= DeltaSeconds;
        if (PendingFrameTime <= 0.f)
        {
            PendingFrameTime = 0.f;
            if (bPauseMode)
            {
                ApplyTimeScale(0.f);
            }
        }
    }
}

void AGodCamPawn::UpdateOverlay(float DeltaSeconds)
{
    (void)DeltaSeconds;

    if (!OverlayWidget.IsValid())
    {
        if (APlayerController* PC = Cast<APlayerController>(Controller))
        {
            if (UGodOverlayWidget* Widget = CreateWidget<UGodOverlayWidget>(PC, UGodOverlayWidget::StaticClass()))
            {
                OverlayWidget = Widget;
                if (bOverlayVisible)
                {
                    Widget->AddToViewport(200);
                }
            }
        }
    }

    if (OverlayWidget.IsValid())
    {
        OverlayWidget->SetSpeedInfo(CurrentTier, SpeedScalar);
        OverlayWidget->SetTimeScale(CachedTimeScale);
        OverlayWidget->SetPositionInfo(GetActorLocation());
        OverlayWidget->SetOverlayVisible(bOverlayVisible);
        OverlayWidget->SetCursorInfo(CachedCursorHit.GetActor(), CachedCursorHit.Distance);
        OverlayWidget->SetDrawToggles(bToggleArmadas, bToggleGr0c, bToggleNhi, bToggleSectors);
    }
}

double AGodCamPawn::GetBaseSpeed(EGodSpeedTier Tier) const
{
    switch (Tier)
    {
    case EGodSpeedTier::Slow:
        return SlowSpeedCmPerSec;
    case EGodSpeedTier::Fast:
        return FastSpeedCmPerSec;
    case EGodSpeedTier::Ludicrous:
        return LudicrousSpeedCmPerSec;
    case EGodSpeedTier::Normal:
    default:
        return NormalSpeedCmPerSec;
    }
}

FVector AGodCamPawn::GetForwardMovementVector() const
{
    return GetActorForwardVector();
}

FVector AGodCamPawn::GetRightMovementVector() const
{
    return GetActorRightVector();
}

FVector AGodCamPawn::GetUpMovementVector() const
{
    return FVector::UpVector;
}

