#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GodBookmarkSubsystem.generated.h"

/** Data stored for each bookmark slot. */
USTRUCT()
struct FGodBookmarkEntry
{
    GENERATED_BODY()

    UPROPERTY()
    bool bValid = false;

    UPROPERTY()
    FString Name;

    UPROPERTY()
    FVector Location = FVector::ZeroVector;

    UPROPERTY()
    FRotator Rotation = FRotator::ZeroRotator;
};

/**
 * Stores inspector bookmarks for the God-Mode pawn.
 */
UCLASS()
class DEVTOOLS_API UGodBookmarkSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /** Store a bookmark in the specified slot. */
    void SetBookmark(int32 Index, const FVector& Location, const FRotator& Rotation, const FString& Name);

    /** Retrieve bookmark data. */
    bool GetBookmark(int32 Index, FVector& OutLocation, FRotator& OutRotation, FString* OutName = nullptr) const;

    /** Save bookmarks immediately. */
    void SaveBookmarks();

private:
    void LoadBookmarks();

    FString GetSavePath() const;

private:
    UPROPERTY()
    TArray<FGodBookmarkEntry> Bookmarks;
};

