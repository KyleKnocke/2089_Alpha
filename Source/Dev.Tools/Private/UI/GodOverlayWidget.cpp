#include "UI/GodOverlayWidget.h"

#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Pawn/GodCamPawn.h"

UGodOverlayWidget::UGodOverlayWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , SpeedText(nullptr)
    , TimeText(nullptr)
    , PositionText(nullptr)
    , CursorText(nullptr)
    , TogglesText(nullptr)
{
    SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UGodOverlayWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!WidgetTree)
    {
        return;
    }

    UVerticalBox* RootBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("RootBox"));
    WidgetTree->RootWidget = RootBox;

    auto MakeLabel = [this, RootBox](const TCHAR* Name, const TCHAR* Label) -> UTextBlock*
    {
        UTextBlock* Block = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), Name);
        Block->SetText(FText::FromString(Label));
        Block->SetJustification(ETextJustify::Left);
        RootBox->AddChild(Block);
        return Block;
    };

    SpeedText = MakeLabel(TEXT("SpeedText"), TEXT("Speed: --"));
    TimeText = MakeLabel(TEXT("TimeText"), TEXT("Time: --"));
    PositionText = MakeLabel(TEXT("PositionText"), TEXT("Position: --"));
    CursorText = MakeLabel(TEXT("CursorText"), TEXT("Cursor: --"));
    TogglesText = MakeLabel(TEXT("TogglesText"), TEXT("Toggles: --"));
}

void UGodOverlayWidget::SetSpeedInfo(EGodSpeedTier Tier, float Scalar)
{
    const TCHAR* TierName = TEXT("Normal");
    switch (Tier)
    {
    case EGodSpeedTier::Slow:
        TierName = TEXT("Slow");
        break;
    case EGodSpeedTier::Fast:
        TierName = TEXT("Fast");
        break;
    case EGodSpeedTier::Ludicrous:
        TierName = TEXT("Ludicrous");
        break;
    case EGodSpeedTier::Normal:
    default:
        TierName = TEXT("Normal");
        break;
    }

    FString Text = FString::Printf(TEXT("Speed: %s x%.2f"), TierName, Scalar);
    UpdateTextBlock(SpeedText, Text);
}

void UGodOverlayWidget::SetTimeScale(float Scale)
{
    FString Text = FString::Printf(TEXT("Time: %0.2fx"), Scale);
    UpdateTextBlock(TimeText, Text);
}

void UGodOverlayWidget::SetPositionInfo(const FVector& LocationCm)
{
    const FVector LocationKm = LocationCm / 100000.f; // convert to km for readability
    FString Text = FString::Printf(TEXT("Position: [%.2f, %.2f, %.2f] km"), LocationKm.X, LocationKm.Y, LocationKm.Z);
    UpdateTextBlock(PositionText, Text);
}

void UGodOverlayWidget::SetOverlayVisible(bool bVisible)
{
    SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
}

void UGodOverlayWidget::SetCursorInfo(AActor* Actor, float Distance)
{
    if (Actor)
    {
        FString Text = FString::Printf(TEXT("Cursor: %s (%.0f cm)"), *Actor->GetName(), Distance);
        UpdateTextBlock(CursorText, Text);
    }
    else
    {
        UpdateTextBlock(CursorText, TEXT("Cursor: --"));
    }
}

void UGodOverlayWidget::SetDrawToggles(bool bArmadas, bool bGr0c, bool bNhi, bool bSectors)
{
    FString Text = FString::Printf(TEXT("Toggles: Armadas=%s Gr0c=%s NHI=%s Sectors=%s"),
        bArmadas ? TEXT("On") : TEXT("Off"),
        bGr0c ? TEXT("On") : TEXT("Off"),
        bNhi ? TEXT("On") : TEXT("Off"),
        bSectors ? TEXT("On") : TEXT("Off"));
    UpdateTextBlock(TogglesText, Text);
}

void UGodOverlayWidget::UpdateTextBlock(UTextBlock* Block, const FString& InText)
{
    if (Block)
    {
        Block->SetText(FText::FromString(InText));
    }
}

