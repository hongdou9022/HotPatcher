// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Model/FHotPatcherCookModel.h"
#include "ThreadUtils/FProcWorkerThread.hpp"

#include "ICookerItemInterface.h"
#include "MissionNotificationProxy.h"
#include "SHotPatcherCookedPlatforms.h"
#include "SHotPatcherCookMaps.h"
#include "SHotPatcherCookSetting.h"
// ZJ_Change_Start
#include "SHotPatcherCookMapFolders.h"
// ZJ_Change_End
#include "SHotPatcherCookSpecifyCookFilter.h"


DECLARE_LOG_CATEGORY_EXTERN(LogCookPage, Log, All);

/**
 * Implements the profile page for the session launcher wizard.
 */
class SProjectCookPage
	: public SCompoundWidget,public ICookerItemInterface
{
public:

	SLATE_BEGIN_ARGS(SProjectCookPage) { }
	SLATE_END_ARGS()

public:

	/**
	 * Constructs the widget.
	 *
	 * @param InArgs The Slate argument list.
	 */
	void Construct(	const FArguments& InArgs,TSharedPtr<FHotPatcherCookModel> InCookModel);
	
public:
	virtual TSharedPtr<FJsonObject> SerializeAsJson()const override;
	virtual void DeSerializeFromJsonObj(TSharedPtr<FJsonObject>const & InJsonObject)override;
	virtual FString GetSerializeName()const override;
	virtual void Reset() override;
protected:
	FReply DoImportConfig();
	FReply DoExportConfig()const;
	FReply DoResetConfig();

	bool CanExecuteCook()const;
	void RunCookProc(const FString& InBinPath, const FString& InCommand)const;
	FReply RunCook()const;

protected:
	TArray<FString> GetDefaultCookParams()const;
	void CancelCookMission();

	TArray<ICookerItemInterface*> GetSerializableItems()const
	{
		TArray<ICookerItemInterface*> List;
		List.Add(Platforms.Get());
		List.Add(CookMaps.Get());
		// ZJ_Change_Start
		List.Add(CookMapFolders.Get());
		// ZJ_Change_End
		List.Add(CookFilters.Get());
		List.Add(CookSettings.Get());
		return List;
	}

	FString SerializeAsString()const;

private:
	UMissionNotificationProxy* MissionNotifyProay;
	bool InCooking=false;
	/** The pending progress message */
	TWeakPtr<SNotificationItem> PendingProgressPtr;
	TSharedPtr<FHotPatcherCookModel> mCookModel;
	mutable TSharedPtr<FProcWorkerThread> mCookProcWorkingThread;
	TSharedPtr<SHotPatcherCookedPlatforms> Platforms;
	TSharedPtr<SHotPatcherCookMaps> CookMaps;
	// ZJ_Change_Start
	TSharedPtr<SHotPatcherCookMapFolders> CookMapFolders;
	// ZJ_Change_End
	TSharedPtr<SHotPatcherCookSpecifyCookFilter> CookFilters;
	TSharedPtr<SHotPatcherCookSetting> CookSettings;

};
