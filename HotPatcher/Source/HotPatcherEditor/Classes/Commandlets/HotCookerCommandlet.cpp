#include "HotCookerCommandlet.h"
#include "ThreadUtils/FProcWorkerThread.hpp"
#include "FCookerConfig.h"
#include "FlibPatchParserHelper.h"
#include "HotPatcherEditor.h"

// engine header
#include "CoreMinimal.h"
#include "Misc/FileHelper.h"
#include "Misc/CommandLine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/Paths.h"

#define COOKER_CONFIG_PARAM_NAME TEXT("-config=")

DEFINE_LOG_CATEGORY(LogHotCookerCommandlet);

TSharedPtr<FProcWorkerThread> CookerProc;

// ZJ_Change_Start: 命令行设置CookPlatform
#define COOK_PLATFORMS TEXT("SetCookPlatforms")
TArray<FString> ParserCookPlatforms(const FString& Commandline)
{
	TArray<FString> result;
	TMap<FString, FString> KeyValues = UFlibPatchParserHelper::GetCommandLineParamsMap(Commandline);
	if(KeyValues.Find(COOK_PLATFORMS))
	{
		FString AddPakListInfo = *KeyValues.Find(COOK_PLATFORMS);
		TArray<FString> PlatformLists;
		AddPakListInfo.ParseIntoArray(PlatformLists,TEXT(","));

		for(auto& PlatformName:PlatformLists)
		{
			ETargetPlatform Platform = ETargetPlatform::None;
			UFlibPatchParserHelper::GetEnumValueByName(PlatformName,Platform);
			if(Platform != ETargetPlatform::None)
			{
				result.AddUnique(PlatformName);
			}
		}
	}
	return result;
}
// ZJ_Change_End: 命令行设置CookPlatform

int32 UHotCookerCommandlet::Main(const FString& Params)
{
	UE_LOG(LogHotCookerCommandlet, Display, TEXT("UHotCookerCommandlet::Main"));
	FString config_path;
	bool bStatus = FParse::Value(*Params, *FString(COOKER_CONFIG_PARAM_NAME).ToLower(), config_path);
	if (!bStatus)
	{
		UE_LOG(LogHotCookerCommandlet, Error, TEXT("not -config=xxxx.json params."));
		return -1;
	}

	if (!FPaths::FileExists(config_path))
	{
		UE_LOG(LogHotCookerCommandlet, Error, TEXT("cofnig file %s not exists."), *config_path);
		return -1;
	}

	FString JsonContent;
	if (FFileHelper::LoadFileToString(JsonContent, *config_path))
	{
		UE_LOG(LogHotCookerCommandlet, Display, TEXT("%s"), *JsonContent);
		FCookerConfig CookConfig;
		UFlibPatchParserHelper::TDeserializeJsonStringAsStruct(JsonContent,CookConfig);
		
		TMap<FString, FString> KeyValues = UFlibPatchParserHelper::GetCommandLineParamsMap(Params);
		UFlibPatchParserHelper::ReplaceProperty(CookConfig, KeyValues);
		
		// ZJ_Change_Start: 命令行设置CookPlatform
		TArray<FString> CookPlatforms = ParserCookPlatforms(Params);
		if (CookPlatforms.Num() > 0)
		{
			CookConfig.CookPlatforms = CookPlatforms;
		}
		// ZJ_Change_End: 命令行设置CookPlatform

		if (CookConfig.bCookAllMap)
		{
			CookConfig.CookMaps = UFlibPatchParserHelper::GetAvailableMaps(UKismetSystemLibrary::GetProjectDirectory(), ENABLE_COOK_ENGINE_MAP, ENABLE_COOK_PLUGIN_MAP, true);
		}
		// ZJ_Change_Start: 添加MapFolder中的Map
		else
		{
			TArray<FString> AssignFolderMaps = UFlibPatchParserHelper::GetAssignFolderMaps(CookConfig.CookMapFolders, true);
			for (auto& Map : AssignFolderMaps)
			{
				if (!CookConfig.CookMaps.Contains(Map))
				{
					CookConfig.CookMaps.Emplace(Map);
				}
			}
		}
		// ZJ_Change_End: 添加MapFolder中的Map

		FString CookCommand;
		UFlibPatchParserHelper::GetCookProcCommandParams(CookConfig, CookCommand);

		UE_LOG(LogHotCookerCommandlet, Display, TEXT("CookCommand:%s %s"), *CookConfig.EngineBin,*CookCommand);

		if (FPaths::FileExists(CookConfig.EngineBin) && FPaths::FileExists(CookConfig.ProjectPath))
		{
			CookerProc = MakeShareable(new FProcWorkerThread(TEXT("CookThread"), CookConfig.EngineBin, CookCommand));
			CookerProc->ProcOutputMsgDelegate.AddStatic(&::ReceiveOutputMsg);

			CookerProc->Execute();
			CookerProc->Join();
		}
	}
	if(FParse::Param(FCommandLine::Get(), TEXT("wait")))
	{
		system("pause");
	}
	return 0;
}
