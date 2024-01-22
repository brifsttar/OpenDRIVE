// Copyright Epic Games, Inc. All Rights Reserved.

#include "OpenDRIVEAssetActions.h"
#include "EditorReimportHandler.h"


FText FOpenDRIVEAssetActions::GetName() const {
	return FText::FromString("OpenDRIVE");
}

FColor FOpenDRIVEAssetActions::GetTypeColor() const {
	return FColor::Blue;
}

UClass* FOpenDRIVEAssetActions::GetSupportedClass() const {
	return UOpenDriveAsset::StaticClass();
}

uint32 FOpenDRIVEAssetActions::GetCategories() {
	return EAssetTypeCategories::Misc;
}

void FOpenDRIVEAssetActions::GetActions(
	const TArray<UObject*>& InObjects,
	FMenuBuilder& MenuBuilder
) {
	FText ButtonLabel = FText::FromString("Reimport");
	FText ButtonToolTip = FText::FromString("Reimport the OpenDRIVE file from the source .xodr");

	auto OdrAssets = GetTypedWeakObjectPtrs<UOpenDriveAsset>(InObjects);

	auto TDelegateExecuteAction = FExecuteAction::CreateSP(
		this,
		&FOpenDRIVEAssetActions::ReimportOdrAsset,
		OdrAssets
	);

	auto UIAction = FUIAction(TDelegateExecuteAction);

	MenuBuilder.AddMenuEntry(ButtonLabel, ButtonToolTip, FSlateIcon(), UIAction);

	//

	FText ButtonLabel2 = FText::FromString("Export .xodr");
	FText ButtonToolTip2 = FText::FromString("Export the source .xodr file");

	auto OdrAssets2 = GetTypedWeakObjectPtrs<UOpenDriveAsset>(InObjects);

	auto TDelegateExecuteAction2 = FExecuteAction::CreateSP(
		this,
		&FOpenDRIVEAssetActions::ExportXODR,
		OdrAssets2
	);

	auto UIAction2 = FUIAction(TDelegateExecuteAction2);

	MenuBuilder.AddMenuEntry(ButtonLabel2, ButtonToolTip2, FSlateIcon(), UIAction2);
}

void FOpenDRIVEAssetActions::ReimportOdrAsset(TArray<TWeakObjectPtr<UOpenDriveAsset>> OdrAssets) {
	for (auto OdrAssetWeak : OdrAssets) {
		UOpenDriveAsset* OdrAsset = OdrAssetWeak.Get();

		if (IsValid(OdrAsset)) {
			FReimportManager::Instance()->Reimport(OdrAsset, true);
		}
	}
}

void FOpenDRIVEAssetActions::ExportXODR(TArray<TWeakObjectPtr<UOpenDriveAsset>> OdrAssets)
{
	for (auto OdrAssetWeak : OdrAssets) {

		UOpenDriveAsset* OdrAsset = OdrAssetWeak.Get();

		if (IsValid(OdrAsset)) {

			FFileHelper::SaveStringToFile(OdrAsset->XodrContent,
				*(FPaths::ProjectSavedDir() + "Export/xodr/" + OdrAsset->GetName() + ".xodr")
			);

		}
	}
}

