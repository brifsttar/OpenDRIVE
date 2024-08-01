#include "EditorMode/OpenDriveEditorModeCommands.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "OpenDriveEditorModeCommands"

FOpenDriveEditorModeCommands::FOpenDriveEditorModeCommands() 
	: TCommands<FOpenDriveEditorModeCommands>("OpenDriveEditorMode",
		NSLOCTEXT("OpenDriveEditorMode", "OpenDriveEditorModeCommands", "OpenDrive Editor Mode"),
		NAME_None,
		FEditorStyle::GetStyleSetName()
	)
{}

void FOpenDriveEditorModeCommands::OnStartupModule()
{
	FOpenDriveEditorModeCommands::Register();
}

void FOpenDriveEditorModeCommands::OnShutdownModule()
{
	FOpenDriveEditorModeCommands::Unregister();
}

void FOpenDriveEditorModeCommands::RegisterCommands()
{
	TArray<TSharedPtr<FUICommandInfo>>& ToolCommands = Commands.FindOrAdd(NAME_Default);

	UI_COMMAND(OpenDriveVisualizerTool, "Viewer", "Draws your currently used xodr file into the level", EUserInterfaceActionType::Button, FInputChord());
	ToolCommands.Add(OpenDriveVisualizerTool);

	UI_COMMAND(OpenDriveUtilsTool, "Utilities", "404", EUserInterfaceActionType::Button, FInputChord());
	ToolCommands.Add(OpenDriveUtilsTool);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FOpenDriveEditorModeCommands::GetCommands()
{
	return FOpenDriveEditorModeCommands::Get().Commands;
}

#undef LOCTEXT_NAMESPACE
