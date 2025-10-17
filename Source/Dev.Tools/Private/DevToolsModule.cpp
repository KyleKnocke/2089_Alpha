#include "DevToolsModule.h"

#include "Modules/ModuleManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Pawn/GodCamPawn.h"
#include "Settings/GodToolsSettings.h"
#include "Templates/UniquePtr.h"

#if !WITH_DEV_TOOLS
#define DEVTOOLS_ENABLED 0
#else
#define DEVTOOLS_ENABLED 1
#endif

#if DEVTOOLS_ENABLED
#include "HAL/IConsoleManager.h"
#endif

IMPLEMENT_MODULE(FDevToolsModule, DevTools)

namespace
{
#if DEVTOOLS_ENABLED
    TUniquePtr<FAutoConsoleCommandWithWorldAndArgs> GGodPossessCmd;
    TUniquePtr<FAutoConsoleCommandWithWorldAndArgs> GGodSpeedCmd;
    TUniquePtr<FAutoConsoleCommandWithWorldAndArgs> GGodTimeScaleCmd;
    TUniquePtr<FAutoConsoleCommandWithWorldAndArgs> GGodBookmarkSetCmd;
    TUniquePtr<FAutoConsoleCommandWithWorldAndArgs> GGodBookmarkGotoCmd;
    TUniquePtr<FAutoConsoleCommandWithWorldAndArgs> GGodToggleCmd;

    AGodCamPawn* GetOrSpawnGodPawn(UWorld* World)
    {
        if (!World)
        {
            return nullptr;
        }

        if (const UGodToolsSettings* Settings = UGodToolsSettings::Get())
        {
            if (!Settings->AreToolsEnabled())
            {
                return nullptr;
            }
        }

        APlayerController* PC = World->GetFirstPlayerController();
        if (!PC)
        {
            return nullptr;
        }

        if (AGodCamPawn* Existing = Cast<AGodCamPawn>(PC->GetPawn()))
        {
            return Existing;
        }

        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        Params.Owner = PC;

        FVector SpawnLocation = PC->GetFocalLocation();
        FRotator SpawnRotation = PC->GetControlRotation();

        AGodCamPawn* NewPawn = World->SpawnActor<AGodCamPawn>(SpawnLocation, SpawnRotation, Params);
        if (NewPawn)
        {
            PC->Possess(NewPawn);
        }
        return NewPawn;
    }

    AGodCamPawn* GetGodPawn(UWorld* World)
    {
        if (!World)
        {
            return nullptr;
        }

        APlayerController* PC = World->GetFirstPlayerController();
        if (!PC)
        {
            return nullptr;
        }

        return Cast<AGodCamPawn>(PC->GetPawn());
    }

    void HandleGodSpeed(const TArray<FString>& Args, UWorld* World)
    {
        if (AGodCamPawn* Pawn = GetGodPawn(World))
        {
            if (Args.Num() > 0)
            {
                Pawn->GodSpeed(Args[0]);
            }
        }
    }

    void HandleGodTimeScale(const TArray<FString>& Args, UWorld* World)
    {
        if (AGodCamPawn* Pawn = GetGodPawn(World))
        {
            if (Args.Num() > 0)
            {
                Pawn->GodTimeScale(FCString::Atof(*Args[0]));
            }
        }
    }

    void HandleGodBookmarkSet(const TArray<FString>& Args, UWorld* World)
    {
        if (AGodCamPawn* Pawn = GetGodPawn(World))
        {
            if (Args.Num() >= 1)
            {
                const int32 Index = FCString::Atoi(*Args[0]);
                FString Name = Args.Num() > 1 ? Args[1] : FString();
                Pawn->GodBookmarkSet(Index, Name);
            }
        }
    }

    void HandleGodBookmarkGoto(const TArray<FString>& Args, UWorld* World)
    {
        if (AGodCamPawn* Pawn = GetGodPawn(World))
        {
            if (Args.Num() >= 1)
            {
                const int32 Index = FCString::Atoi(*Args[0]);
                Pawn->GodBookmarkGoto(Index);
            }
        }
    }

    void HandleGodToggle(const TArray<FString>& Args, UWorld* World)
    {
        if (AGodCamPawn* Pawn = GetGodPawn(World))
        {
            if (Args.Num() >= 1)
            {
                Pawn->GodToggleFlag(Args[0]);
            }
        }
    }
#endif
}

void FDevToolsModule::StartupModule()
{
#if DEVTOOLS_ENABLED
    GGodPossessCmd = MakeUnique<FAutoConsoleCommandWithWorldAndArgs>(TEXT("god.Possess"), TEXT("Spawn or possess the God camera pawn."),
        FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
        {
            GetOrSpawnGodPawn(World);
        }));

    GGodSpeedCmd = MakeUnique<FAutoConsoleCommandWithWorldAndArgs>(TEXT("god.Speed"), TEXT("Set God pawn speed tier or scalar."),
        FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&HandleGodSpeed));

    GGodTimeScaleCmd = MakeUnique<FAutoConsoleCommandWithWorldAndArgs>(TEXT("god.TimeScale"), TEXT("Set global time dilation."),
        FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&HandleGodTimeScale));

    GGodBookmarkSetCmd = MakeUnique<FAutoConsoleCommandWithWorldAndArgs>(TEXT("god.Bookmark.Set"), TEXT("Set a God bookmark."),
        FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&HandleGodBookmarkSet));

    GGodBookmarkGotoCmd = MakeUnique<FAutoConsoleCommandWithWorldAndArgs>(TEXT("god.Bookmark.Goto"), TEXT("Goto a God bookmark."),
        FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&HandleGodBookmarkGoto));

    GGodToggleCmd = MakeUnique<FAutoConsoleCommandWithWorldAndArgs>(TEXT("god.Toggle"), TEXT("Toggle overlay flags."),
        FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&HandleGodToggle));
#endif
}

void FDevToolsModule::ShutdownModule()
{
#if DEVTOOLS_ENABLED
    GGodToggleCmd.Reset();
    GGodBookmarkGotoCmd.Reset();
    GGodBookmarkSetCmd.Reset();
    GGodTimeScaleCmd.Reset();
    GGodSpeedCmd.Reset();
    GGodPossessCmd.Reset();
#endif
}

