#include "WGDebugWidget.h"

#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WorldGenSubsystem.h"
#include "Blueprint/WidgetTree.h"

namespace
{
    constexpr double SectorSizeLy = 1000.0;
}

void UWGDebugWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!WidgetTree->RootWidget)
    {
        UVerticalBox* VBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("RootBox"));
        WidgetTree->RootWidget = VBox;
        InfoText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("InfoText"));
        InfoText->SetJustification(ETextJustify::Left);
        InfoText->SetAutoWrapText(true);
        VBox->AddChildToVerticalBox(InfoText);
    }
}

void UWGDebugWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (InfoText)
    {
        InfoText->SetText(FText::FromString(BuildDebugText()));
    }
}

FString UWGDebugWidget::BuildDebugText() const
{
    const UWorld* World = GetWorld();
    if (!World)
    {
        return TEXT("No world");
    }

    const UGameInstance* GameInstance = World->GetGameInstance();
    const UWorldGenSubsystem* Subsystem = GameInstance ? GameInstance->GetSubsystem<UWorldGenSubsystem>() : nullptr;
    if (!Subsystem)
    {
        return TEXT("WorldGen subsystem offline");
    }

    const APlayerController* PlayerController = GetOwningPlayer();
    const APawn* Pawn = PlayerController ? PlayerController->GetPawn() : nullptr;
    const FVector PawnLocation = Pawn ? Pawn->GetActorLocation() : FVector::ZeroVector;

    const FVector3d GalacticPos(PawnLocation.X / 1000.0, PawnLocation.Y / 1000.0, PawnLocation.Z / 1000.0);
    const FIntPoint SectorCoords(FMath::FloorToInt(GalacticPos.X / SectorSizeLy), FMath::FloorToInt(GalacticPos.Y / SectorSizeLy));

    FString Output = FString::Printf(TEXT("Seed: %lld\nSector: (%d, %d)\nGal Pos (ly): %.2f, %.2f, %.2f"),
        Subsystem->GetMasterSeed(),
        SectorCoords.X,
        SectorCoords.Y,
        GalacticPos.X,
        GalacticPos.Y,
        GalacticPos.Z);

    const TArray<FArmadaDescriptor> NearbyArmadas = Subsystem->GetNearestArmadas(GalacticPos, 100.0);
    if (NearbyArmadas.Num() > 0)
    {
        const FArmadaDescriptor& Closest = NearbyArmadas[0];
        const double Distance = FVector3d::Distance(Closest.Centroid, GalacticPos);
        Output += FString::Printf(TEXT("\nNearest Armada: %s (%.1f ly, %d fleets)"), *Closest.Faction.ToString(), Distance, Closest.FleetCount);
    }
    else
    {
        Output += TEXT("\nNearest Armada: None within 100 ly");
    }

    const TArray<FNhiNodeDescriptor> EarthNodes = Subsystem->GetNhiNodes(EWorldBody::Earth);
    Output += FString::Printf(TEXT("\nEarth NHI Nodes: %d"), EarthNodes.Num());

    const TArray<FNhiNodeDescriptor> MoonNodes = Subsystem->GetNhiNodes(EWorldBody::Moon);
    Output += FString::Printf(TEXT("\nMoon NHI Nodes: %d"), MoonNodes.Num());

    return Output;
}
