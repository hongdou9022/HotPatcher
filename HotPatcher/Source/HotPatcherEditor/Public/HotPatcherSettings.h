#pragma once
#include "ETargetPlatform.h"
#include "HotPatcherEditor.h"

#include "CoreMinimal.h"
#include "Kismet/KismetTextLibrary.h"
#include "HotPatcherSettings.generated.h"
#define LOCTEXT_NAMESPACE "UHotPatcherSettings"

UCLASS(config = Game, defaultconfig)
class HOTPATCHEREDITOR_API UHotPatcherSettings:public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, config, Category = "Editor")
    bool bWhiteListCookInEditor;
    UPROPERTY(EditAnywhere, config, Category = "Editor")
    TArray<ETargetPlatform> PlatformWhitelists;
    UPROPERTY(EditAnywhere, config, Category = "Editor")
    FString TempPakDir = TEXT("Saved/HotPatcher/Paks");
    
    UPROPERTY(EditAnywhere, config, Category = "Editor|IoStore")
    bool bIoStore = false;
    UPROPERTY(EditAnywhere, config, Category = "Editor|IoStore")
    bool bAllowBulkDataInIoStore = false;
    UPROPERTY(EditAnywhere, config, Category = "Editor|IoStore")
    bool bStorageIoStorePakList = false;
    UPROPERTY(EditAnywhere, config, Category = "Editor|IoStore")
    TArray<FString> IoStorePakListOptions;
    UPROPERTY(EditAnywhere, config, Category = "Editor|IoStore")
    TArray<FString> IoStoreCommandletOptions;
    UPROPERTY(EditAnywhere, config, Category = "Editor|UnrealPak")
    bool bStorageUnrealPakList = false;
    UPROPERTY(EditAnywhere, config, Category = "Editor|UnrealPak")
    TArray<FString> UnrealPakListOptions;
    UPROPERTY(EditAnywhere, config, Category = "Editor|UnrealPak")
    TArray<FString> UnrealPakCommandletOptions;
    

    
};


#undef LOCTEXT_NAMESPACE