#include "HotCookerExCommandlet.h"
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

DEFINE_LOG_CATEGORY(LogHotCookerExCommandlet);

TSharedPtr<FProcWorkerThread> CookerExProc;

#define COOKEX_PLATFORMS TEXT("SetCookPlatforms")
TArray<FString> ParserCookExPlatforms(const FString& Commandline)
{
	TArray<FString> result;
	TMap<FString, FString> KeyValues = UFlibPatchParserHelper::GetCommandLineParamsMap(Commandline);
	if(KeyValues.Find(COOKEX_PLATFORMS))
	{
		FString AddPakListInfo = *KeyValues.Find(COOKEX_PLATFORMS);
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

int32 UHotCookerExCommandlet::Main(const FString& Params)
{
	UE_LOG(LogHotCookerExCommandlet, Display, TEXT("UHotCookerExCommandlet::Main"));
	FString config_path;
	bool bStatus = FParse::Value(*Params, *FString(COOKER_CONFIG_PARAM_NAME).ToLower(), config_path);
	if (!bStatus)
	{
		UE_LOG(LogHotCookerExCommandlet, Error, TEXT("not -config=xxxx.json params."));
		return -1;
	}

	if (!FPaths::FileExists(config_path))
	{
		UE_LOG(LogHotCookerExCommandlet, Error, TEXT("cofnig file %s not exists."), *config_path);
		return -1;
	}

	FString JsonContent;
	if (FFileHelper::LoadFileToString(JsonContent, *config_path))
	{
		UE_LOG(LogHotCookerExCommandlet, Display, TEXT("%s"), *JsonContent);
		FCookerConfig CookConfig;
		UFlibPatchParserHelper::TDeserializeJsonStringAsStruct(JsonContent,CookConfig);
		
		TMap<FString, FString> KeyValues = UFlibPatchParserHelper::GetCommandLineParamsMap(Params);
		UFlibPatchParserHelper::ReplaceProperty(CookConfig, KeyValues);

		TArray<FString> CookPlatforms = ParserCookExPlatforms(Params);
		if (CookPlatforms.Num() > 0)
		{
			CookConfig.CookPlatforms = CookPlatforms;
		}
		
		if (CookConfig.bCookAllMap)
		{
			CookConfig.CookMaps = UFlibPatchParserHelper::GetAvailableMaps(UKismetSystemLibrary::GetProjectDirectory(), ENABLE_COOK_ENGINE_MAP, ENABLE_COOK_PLUGIN_MAP, true);
		}
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

		FString CookCommand;
		UFlibPatchParserHelper::GetCookProcExCommandParams(CookConfig, CookCommand);

		CookConfig.EngineBin = CookConfig.EngineBin.Replace(TEXT("Binaries/Win64/UE4Editor-Cmd.exe"), TEXT("Build/BatchFiles/RunUAT.bat"));

		UE_LOG(LogHotCookerExCommandlet, Display, TEXT("CookCommand:%s %s"), *CookConfig.EngineBin, *CookCommand);

		if (FPaths::FileExists(CookConfig.EngineBin) && FPaths::FileExists(CookConfig.ProjectPath))
		{
			CookerExProc = MakeShareable(new FProcWorkerThread(TEXT("CookThread"), CookConfig.EngineBin, CookCommand));
			CookerExProc->ProcOutputMsgDelegate.AddStatic(&::ReceiveOutputMsg);

			CookerExProc->Execute();
			CookerExProc->Join();
		}
	}
	if(FParse::Param(FCommandLine::Get(), TEXT("wait")))
	{
		system("pause");
	}
	return 0;
}
