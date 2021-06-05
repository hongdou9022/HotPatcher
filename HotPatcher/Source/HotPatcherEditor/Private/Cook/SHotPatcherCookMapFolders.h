// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Model/FHotPatcherCookModel.h"
#include "Templates/SharedPointer.h"
#include "FlibPatchParserHelper.h"

// engine header
#include "IDetailsView.h"
#include "Templates/SharedPointer.h"
#include "PropertyEditorModule.h"
#include "Kismet/KismetSystemLibrary.h"

// project header
#include "ICookerItemInterface.h"
#include "CookMapFoldersSetting.h"

/**
 * Implements the cooked Maps panel.
 */
class SHotPatcherCookMapFolders
	: public SCompoundWidget,public ICookerItemInterface
{
public:

	SLATE_BEGIN_ARGS(SHotPatcherCookMapFolders) { }
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

public:
	UCookMapFoldersSetting* GetCookMapFoldersSetting()const;
	TArray<FDirectoryPath> GetCookMapFolders()const;
	TArray<FString> GetCookMapAbsFolders();
	void HandleRequestCookMapFolders(TArray<FDirectoryPath>& OutCookDir);
protected:
	void CreateFolderListView();

private:
	/** Settings view ui element ptr */
	TSharedPtr<IDetailsView> SettingsView;

	UCookMapFoldersSetting* CookMapFoldersSetting;

	TSharedPtr<FHotPatcherCookModel> mCookModel;
};

