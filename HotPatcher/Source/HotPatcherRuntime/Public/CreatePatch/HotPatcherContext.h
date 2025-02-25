﻿#pragma once

#include "HotPatcherSettingBase.h"
#include "FPatchVersionDiff.h"
#include "FHotPatcherVersion.h"
#include "FChunkInfo.h"
#include "FPakFileInfo.h"
#include "FExportReleaseSettings.h"
#include "HotPatcherSettingBase.h"

// engine
#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "HotPatcherContext.generated.h"

struct FChunkInfo;
struct FHotPatcherVersion;


DECLARE_MULTICAST_DELEGATE_TwoParams(FExportPakProcess,const FString&,const FString&);
DECLARE_MULTICAST_DELEGATE_OneParam(FExportPakShowMsg,const FString&);

USTRUCT(BlueprintType)
struct HOTPATCHERRUNTIME_API FHotPatcherContext
{
    GENERATED_USTRUCT_BODY()
    FHotPatcherContext()=default;
    virtual ~FHotPatcherContext(){}
    //virtual FHotPatcherSettingBase* GetSettingObject() { return (FHotPatcherSettingBase*)ContextSetting; }

public:
    FExportPakProcess OnPaking;
    FExportPakShowMsg OnShowMsg;
    struct FHotPatcherSettingBase* ContextSetting = nullptr;
    class UScopedSlowTaskContext* UnrealPakSlowTask = nullptr;
};

USTRUCT(BlueprintType)
struct HOTPATCHERRUNTIME_API FHotPatcherPatchContext:public FHotPatcherContext
{
    GENERATED_USTRUCT_BODY()
    FHotPatcherPatchContext()=default;
    virtual FExportPatchSettings* GetSettingObject() { return (FExportPatchSettings*)ContextSetting; }
    
    // base version content
    UPROPERTY(EditAnywhere)
    FHotPatcherVersion BaseVersion;
    // current project content
    UPROPERTY(EditAnywhere)
    FHotPatcherVersion CurrentVersion;

    // base version and current version different
    UPROPERTY(EditAnywhere)
    FPatchVersionDiff VersionDiff;

    // final new version content
    UPROPERTY(EditAnywhere)
    FHotPatcherVersion NewReleaseVersion;

    // generated current version chunk
    UPROPERTY(EditAnywhere)
    FChunkInfo NewVersionChunk;
    
    // chunk info
    UPROPERTY(EditAnywhere)
    TArray<FChunkInfo> PakChunks;

    UPROPERTY(EditAnywhere)
    TArray<FPakCommand> AdditionalFileToPak;
    
    // every pak file info
    // TArray<FPakFileProxy> PakFileProxys;
    FORCEINLINE uint32 GetPakFileNum()const 
    {
        TArray<FString> Keys;
        PakFilesInfoMap.GetKeys(Keys);
        return Keys.Num();
    }
    TMap<FString,TArray<FPakFileInfo>> PakFilesInfoMap;

};

USTRUCT(BlueprintType)
struct HOTPATCHERRUNTIME_API FHotPatcherReleaseContext:public FHotPatcherContext
{
    GENERATED_USTRUCT_BODY()
    FHotPatcherReleaseContext()=default;
    virtual FExportReleaseSettings* GetSettingObject() { return (FExportReleaseSettings*)ContextSetting; }

    UPROPERTY(BlueprintReadOnly)
    FHotPatcherVersion NewReleaseVersion;
    
};