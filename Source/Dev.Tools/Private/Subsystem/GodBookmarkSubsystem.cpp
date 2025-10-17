#include "Subsystem/GodBookmarkSubsystem.h"

#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"

namespace
{
    constexpr int32 MaxBookmarks = 10;
}

void UGodBookmarkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    Bookmarks.SetNum(MaxBookmarks);
    LoadBookmarks();
}

void UGodBookmarkSubsystem::Deinitialize()
{
    SaveBookmarks();
    Super::Deinitialize();
}

void UGodBookmarkSubsystem::SetBookmark(int32 Index, const FVector& Location, const FRotator& Rotation, const FString& Name)
{
    if (!Bookmarks.IsValidIndex(Index))
    {
        return;
    }

    FGodBookmarkEntry& Entry = Bookmarks[Index];
    Entry.bValid = true;
    Entry.Location = Location;
    Entry.Rotation = Rotation;
    Entry.Name = Name;

    SaveBookmarks();
}

bool UGodBookmarkSubsystem::GetBookmark(int32 Index, FVector& OutLocation, FRotator& OutRotation, FString* OutName) const
{
    if (!Bookmarks.IsValidIndex(Index))
    {
        return false;
    }

    const FGodBookmarkEntry& Entry = Bookmarks[Index];
    if (!Entry.bValid)
    {
        return false;
    }

    OutLocation = Entry.Location;
    OutRotation = Entry.Rotation;
    if (OutName)
    {
        *OutName = Entry.Name;
    }

    return true;
}

void UGodBookmarkSubsystem::SaveBookmarks()
{
    FString SavePath = GetSavePath();
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(SavePath), true);
    TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
    TArray<TSharedPtr<FJsonValue>> JsonBookmarks;

    for (int32 Index = 0; Index < Bookmarks.Num(); ++Index)
    {
        const FGodBookmarkEntry& Entry = Bookmarks[Index];
        TSharedRef<FJsonObject> JsonEntry = MakeShared<FJsonObject>();
        JsonEntry->SetBoolField(TEXT("valid"), Entry.bValid);
        JsonEntry->SetStringField(TEXT("name"), Entry.Name);
        JsonEntry->SetNumberField(TEXT("x"), Entry.Location.X);
        JsonEntry->SetNumberField(TEXT("y"), Entry.Location.Y);
        JsonEntry->SetNumberField(TEXT("z"), Entry.Location.Z);
        JsonEntry->SetNumberField(TEXT("pitch"), Entry.Rotation.Pitch);
        JsonEntry->SetNumberField(TEXT("yaw"), Entry.Rotation.Yaw);
        JsonEntry->SetNumberField(TEXT("roll"), Entry.Rotation.Roll);
        JsonBookmarks.Add(MakeShared<FJsonValueObject>(JsonEntry));
    }

    Root->SetArrayField(TEXT("bookmarks"), JsonBookmarks);

    FString JsonOutput;
    const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonOutput);
    FJsonSerializer::Serialize(Root, Writer);
    FFileHelper::SaveStringToFile(JsonOutput, *SavePath);
}

void UGodBookmarkSubsystem::LoadBookmarks()
{
    FString SavePath = GetSavePath();
    FString Contents;
    if (!FPaths::FileExists(SavePath) || !FFileHelper::LoadFileToString(Contents, *SavePath))
    {
        return;
    }

    TSharedPtr<FJsonObject> Root;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Contents);
    if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
    {
        return;
    }

    const TArray<TSharedPtr<FJsonValue>>* JsonBookmarks = nullptr;
    if (!Root->TryGetArrayField(TEXT("bookmarks"), JsonBookmarks))
    {
        return;
    }

    const int32 Count = FMath::Min(JsonBookmarks->Num(), Bookmarks.Num());
    for (int32 Index = 0; Index < Count; ++Index)
    {
        const TSharedPtr<FJsonValue>& Value = (*JsonBookmarks)[Index];
        const TSharedPtr<FJsonObject>* JsonEntryPtr;
        if (Value->TryGetObject(JsonEntryPtr) && JsonEntryPtr && JsonEntryPtr->IsValid())
        {
            const TSharedPtr<FJsonObject>& JsonEntry = *JsonEntryPtr;
            FGodBookmarkEntry& Entry = Bookmarks[Index];
            Entry.bValid = JsonEntry->GetBoolField(TEXT("valid"));
            Entry.Name = JsonEntry->GetStringField(TEXT("name"));
            Entry.Location.X = JsonEntry->GetNumberField(TEXT("x"));
            Entry.Location.Y = JsonEntry->GetNumberField(TEXT("y"));
            Entry.Location.Z = JsonEntry->GetNumberField(TEXT("z"));
            Entry.Rotation.Pitch = JsonEntry->GetNumberField(TEXT("pitch"));
            Entry.Rotation.Yaw = JsonEntry->GetNumberField(TEXT("yaw"));
            Entry.Rotation.Roll = JsonEntry->GetNumberField(TEXT("roll"));
        }
    }
}

FString UGodBookmarkSubsystem::GetSavePath() const
{
    return FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Config/Dev/GodBookmarks.json"));
}

