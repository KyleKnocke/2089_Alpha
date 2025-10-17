#include "Settings/GodToolsSettings.h"

#include "Misc/App.h"

UGodToolsSettings::UGodToolsSettings(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
#if WITH_EDITOR
    bEnableInEditor = true;
#else
    bEnableInEditor = false;
#endif
    bEnableInGame = false;
}

bool UGodToolsSettings::AreToolsEnabled() const
{
#if WITH_EDITOR
    if (GIsEditor)
    {
        return bEnableInEditor;
    }
#endif
    return bEnableInGame;
}

const UGodToolsSettings* UGodToolsSettings::Get()
{
    return GetDefault<UGodToolsSettings>();
}

