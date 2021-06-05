// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SHotPatcherCookMapFolders.h"
#include "FLibAssetManageHelperEx.h"
#include "SProjectCookMapListRow.h"
#include "Widgets/Input/SHyperlink.h"
#include "Widgets/Layout/SSeparator.h"
#include "FlibPatchParserHelper.h"
#include "Kismet/KismetSystemLibrary.h"

#define LOCTEXT_NAMESPACE "SHotPatcherCookMapFolders"

void SHotPatcherCookMapFolders::Construct(const FArguments& InArgs, TSharedPtr<FHotPatcherCookModel> InCookModel)
{

	mCookModel = InCookModel;
	mCookModel->OnRequestCookMapFolders.BindRaw(this, &SHotPatcherCookMapFolders::HandleRequestCookMapFolders);
	CreateFolderListView();

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			[
				SettingsView->AsShared()
			]
		]
	];
	CookMapFoldersSetting = UCookMapFoldersSetting::Get();
	SettingsView->SetObject(CookMapFoldersSetting);
}



TSharedPtr<FJsonObject> SHotPatcherCookMapFolders::SerializeAsJson() const
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TArray<TSharedPtr<FJsonValue>> CookMapFolderJsonValueList;

	for (const auto& Filter : GetCookMapFoldersSetting()->GetCookMapFolders())
	{
		FString FilterPath = Filter.Path;
		CookMapFolderJsonValueList.Add(MakeShareable(new FJsonValueString(FilterPath)));
	}
	JsonObject->SetArrayField(TEXT("MapFolders"), CookMapFolderJsonValueList);
	return JsonObject;
}

void SHotPatcherCookMapFolders::DeSerializeFromJsonObj(TSharedPtr<FJsonObject>const & InJsonObject)
{
	TArray<TSharedPtr<FJsonValue>> CookMapFolderJsonValueList = InJsonObject->GetArrayField(TEXT("CookMapFolders"));

	for (const auto& MapFolderJsonValue : CookMapFolderJsonValueList)
	{
		FDirectoryPath FilterDirPath;
		FilterDirPath.Path = MapFolderJsonValue->AsString();
		GetCookMapFoldersSetting()->GetCookMapFolders().Add(FilterDirPath);
	}

}

FString SHotPatcherCookMapFolders::GetSerializeName()const
{
	return TEXT("MapFolders");
}


void SHotPatcherCookMapFolders::Reset()
{
	GetCookMapFoldersSetting()->GetCookMapFolders().Reset();
}


UCookMapFoldersSetting* SHotPatcherCookMapFolders::GetCookMapFoldersSetting() const
{
	return CookMapFoldersSetting;
}

TArray<FDirectoryPath> SHotPatcherCookMapFolders::GetCookMapFolders() const
{
	if (GetCookMapFoldersSetting())
	{
		return GetCookMapFoldersSetting()->GetCookMapFolders();
	}
	else
	{
		return TArray<FDirectoryPath>{};
	}
}


TArray<FString> SHotPatcherCookMapFolders::GetCookMapAbsFolders()
{
	TArray<FString> CookMapFolderAbsPathList;
	for (const auto& CookMapRelativeFolder : GetCookMapFolders())
	{
		FString AbsPath;
		if (UFLibAssetManageHelperEx::ConvRelativeDirToAbsDir(CookMapRelativeFolder.Path, AbsPath))
		{
			CookMapFolderAbsPathList.AddUnique(AbsPath);
		}
	}
	return CookMapFolderAbsPathList;
}

void SHotPatcherCookMapFolders::HandleRequestCookMapFolders(TArray<FDirectoryPath>& OutCookDir)
{
	OutCookDir = GetCookMapFolders();
}

void SHotPatcherCookMapFolders::CreateFolderListView()
{
	// Create a property view
	FPropertyEditorModule& EditModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bUpdatesFromSelection = true;
	DetailsViewArgs.bLockable = true;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::ComponentsAndActorsUseNameArea;
	DetailsViewArgs.bCustomNameAreaLocation = false;
	DetailsViewArgs.bCustomFilterAreaLocation = true;
	DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Hide;

	SettingsView = EditModule.CreateDetailView(DetailsViewArgs);
}

#undef LOCTEXT_NAMESPACE
