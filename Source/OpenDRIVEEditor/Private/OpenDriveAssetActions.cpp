#include "OpenDRIVEAssetActions.h"
#include "EditorReimportHandler.h"

void FOpenDriveAssetActions::OnStartupModule()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	AssetTools.RegisterAssetTypeActions(this->AsShared());
}

void FOpenDriveAssetActions::OnShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools")) 
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		AssetTools.UnregisterAssetTypeActions(this->AsShared());
	}
}

FText FOpenDriveAssetActions::GetName() const {
	return FText::FromString("OpenDRIVE");
}

FColor FOpenDriveAssetActions::GetTypeColor() const {
	return FColor::Blue;
}

UClass* FOpenDriveAssetActions::GetSupportedClass() const {
	return UOpenDriveAsset::StaticClass();
}

uint32 FOpenDriveAssetActions::GetCategories() {
	return EAssetTypeCategories::Misc;
}

void FOpenDriveAssetActions::GetActions(
	const TArray<UObject*>& InObjects,
	FMenuBuilder& MenuBuilder
) {
	FText ButtonLabel = FText::FromString("Reimport");
	FText ButtonToolTip = FText::FromString("Reimport the OpenDRIVE file from the source .xodr");

	auto OdrAssets = GetTypedWeakObjectPtrs<UOpenDriveAsset>(InObjects);

	auto TDelegateExecuteAction = FExecuteAction::CreateSP(
		this,
		&FOpenDriveAssetActions::ReimportOdrAsset,
		OdrAssets
	);

	auto UIAction = FUIAction(TDelegateExecuteAction);

	MenuBuilder.AddMenuEntry(ButtonLabel, ButtonToolTip, FSlateIcon(), UIAction);
}

void FOpenDriveAssetActions::ReimportOdrAsset(TArray<TWeakObjectPtr<UOpenDriveAsset>> OdrAssets) {
	for (auto OdrAssetWeak : OdrAssets) {
		UOpenDriveAsset* OdrAsset = OdrAssetWeak.Get();

		if (IsValid(OdrAsset)) {
			FReimportManager::Instance()->Reimport(OdrAsset, true);
		}
	}
}
