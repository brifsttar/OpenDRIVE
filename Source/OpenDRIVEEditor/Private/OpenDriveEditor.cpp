// Copyright Epic Games, Inc. All Rights Reserved.

#include "Public/OpenDRIVEEditor.h"
#include "OpenDriveAssetActions.h"
#include "AssetToolsModule.h"

#define LOCTEXT_NAMESPACE "FOpenDRIVEEditorModule"

void FOpenDRIVEEditorModule::StartupModule() {
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	OpenDRIVEAssetTypeActions = MakeShareable(new FOpenDRIVEAssetActions);
	AssetTools.RegisterAssetTypeActions(OpenDRIVEAssetTypeActions.ToSharedRef());
}

void FOpenDRIVEEditorModule::ShutdownModule() {
	if (FModuleManager::Get().IsModuleLoaded("AssetTools")) {
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		if (OpenDRIVEAssetTypeActions.IsValid()) {
			AssetTools.UnregisterAssetTypeActions(OpenDRIVEAssetTypeActions.ToSharedRef());
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOpenDRIVEEditorModule, OpenDRIVEEditor)
