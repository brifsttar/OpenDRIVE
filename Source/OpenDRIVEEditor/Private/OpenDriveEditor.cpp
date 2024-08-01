// Copyright Epic Games, Inc. All Rights Reserved.

#include "OpenDRIVEEditor.h"
#include "OpenDriveAssetActions.h"
#include "AssetToolsModule.h"

//#include "EditorMode/OpenDriveEdModeTool.h"
#include "EditorMode/OpenDriveEditorModeCommands.h"
#include "OpenDriveCommands.h"
#include "Gizmo/FOpenDRIVEComponentVisualizer.h"
#include "EditorMode/OpenDriveEditorModeStyle.h"

IMPLEMENT_MODULE(FOpenDriveEditorModule, OpenDRIVEEditor)

#define LOCTEXT_NAMESPACE "FOpenDriveEditorModule"

TSharedRef<FWorkspaceItem> FOpenDriveEditorModule::MenuRoot =
FWorkspaceItem::NewGroup(FText::FromString("Menu Root"));

void FOpenDriveEditorModule::StartupModule() {

	if (!IsRunningCommandlet())
	{
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

		OpenDRIVEAssetTypeActions = MakeShareable(new FOpenDRIVEAssetActions);
		AssetTools.RegisterAssetTypeActions(OpenDRIVEAssetTypeActions.ToSharedRef());
	}

	IOpenDRIVEModuleInterface::StartupModule();
}

void FOpenDriveEditorModule::ShutdownModule() {
	if (FModuleManager::Get().IsModuleLoaded("AssetTools")) {
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		if (OpenDRIVEAssetTypeActions.IsValid()) {
			AssetTools.UnregisterAssetTypeActions(OpenDRIVEAssetTypeActions.ToSharedRef());
		}
	}

	IOpenDRIVEModuleInterface::ShutdownModule();
}

void FOpenDriveEditorModule::AddModuleListeners()
{
	ModuleListeners.Add(MakeShareable(new OpenDriveCommandsModuleListener));
	ModuleListeners.Add(MakeShareable(new FOpenDriveEditorModeCommands));
	ModuleListeners.Add(MakeShareable(new FOpenDriveEditorModeStyleSet));
	ModuleListeners.Add(MakeShareable(new FOpenDriveCVModuleListener));
}

#undef LOCTEXT_NAMESPACE
