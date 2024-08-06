#include "OpenDRIVEEditor.h"
#include "AssetToolsModule.h"

/* tools, mode, commands, actions, etc... */
#include "EditorMode/OpenDriveEditorModeCommands.h"
#include "EditorMode/OpenDriveEditorModeStyle.h"
#include "OpenDriveAssetActions.h"

IMPLEMENT_MODULE(FOpenDriveEditorModule, OpenDRIVEEditor)

#define LOCTEXT_NAMESPACE "FOpenDriveEditorModule"

TSharedRef<FWorkspaceItem> FOpenDriveEditorModule::MenuRoot =
FWorkspaceItem::NewGroup(FText::FromString("Menu Root"));

void FOpenDriveEditorModule::StartupModule() 
{
	IOpenDRIVEModuleInterface::StartupModule();
}

void FOpenDriveEditorModule::ShutdownModule() 
{
	IOpenDRIVEModuleInterface::ShutdownModule();
}

void FOpenDriveEditorModule::AddModuleListeners()
{
	ModuleListeners.Add(MakeShareable(new FOpenDriveEditorModeStyleSet));
	ModuleListeners.Add(MakeShareable(new FOpenDriveEditorModeCommands));
	ModuleListeners.Add(MakeShareable(new FOpenDriveAssetActions));
}

#undef LOCTEXT_NAMESPACE
