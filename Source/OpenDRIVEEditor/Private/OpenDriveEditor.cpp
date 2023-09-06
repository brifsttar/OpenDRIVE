// Copyright Epic Games, Inc. All Rights Reserved.

#include "Public/OpenDRIVEEditor.h"
#include "OpenDriveAssetActions.h"
#include "AssetToolsModule.h"
#include "Public/EditorMode/OpenDriveEdModeTool.h"

IMPLEMENT_MODULE(FOpenDRIVEEditorModule, OpenDRIVEEditor)

#define LOCTEXT_NAMESPACE "FOpenDRIVEEditorModule"

TSharedRef<FWorkspaceItem> FOpenDRIVEEditorModule::MenuRoot =
FWorkspaceItem::NewGroup(FText::FromString("Menu Root"));

void FOpenDRIVEEditorModule::StartupModule() {

	if (!IsRunningCommandlet())
	{
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

		OpenDRIVEAssetTypeActions = MakeShareable(new FOpenDRIVEAssetActions);
		AssetTools.RegisterAssetTypeActions(OpenDRIVEAssetTypeActions.ToSharedRef());
	}

	IOpenDRIVEModuleInterface::StartupModule();
}

void FOpenDRIVEEditorModule::ShutdownModule() {
	if (FModuleManager::Get().IsModuleLoaded("AssetTools")) {
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		if (OpenDRIVEAssetTypeActions.IsValid()) {
			AssetTools.UnregisterAssetTypeActions(OpenDRIVEAssetTypeActions.ToSharedRef());
		}
	}

	IOpenDRIVEModuleInterface::ShutdownModule();
}

void FOpenDRIVEEditorModule::AddModuleListeners()
{
	ModuleListeners.Add(MakeShareable(new OpenDRIVEEdModeTool));
}

#undef LOCTEXT_NAMESPACE
