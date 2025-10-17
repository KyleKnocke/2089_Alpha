#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GodToolsSettings.generated.h"

/**
 * Configuration for enabling the God-Mode dev tools module.
 */
UCLASS(Config=EditorPerProjectUserSettings, DefaultConfig)
class DEVTOOLS_API UGodToolsSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UGodToolsSettings(const FObjectInitializer& ObjectInitializer);

    /** Returns whether God tools should be enabled in the current context. */
    bool AreToolsEnabled() const;

    /** Static accessor. */
    static const UGodToolsSettings* Get();

#if WITH_EDITOR
    virtual FName GetCategoryName() const override { return TEXT("2089"); }
#endif

    /** Whether tools are enabled when running inside the editor. */
    UPROPERTY(Config, EditAnywhere, Category="God Mode")
    bool bEnableInEditor;

    /** Whether tools are enabled in non-editor builds (Development). */
    UPROPERTY(Config, EditAnywhere, Category="God Mode")
    bool bEnableInGame;
};

