#include "EditorMode/OpenDriveEditorModeCommands.h"

#define LOCTEXT_NAMESPACE "OpenDriveEditorModeCommands"

FOpenDriveEditorModeCommands::FOpenDriveEditorModeCommands() :
	TCommands<FOpenDriveEditorModeCommands>
	("OpenDriveEditorModeCommands",
	NSLOCTEXT("OpenDriveEditorModeCommands", "OpenDriveEditorModeCommands", "OpenDRIVE Editor Commands"),
	NAME_None,
	FAppStyle::GetAppStyleSetName()
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

	/* Register our tool commands here */
	UI_COMMAND(OpenDriveVisualizerTool, "Viewer", "Draws your currently used .xodr file into the level", EUserInterfaceActionType::Button, FInputChord());
	ToolCommands.Add(OpenDriveVisualizerTool);
	UI_COMMAND(OpenDriveUtilsTool, "Actors", "OpenDrive's actors utility functions", EUserInterfaceActionType::Button, FInputChord());
	ToolCommands.Add(OpenDriveUtilsTool);
	UI_COMMAND(OpenDriveGizmoTool, "OpenDRIVE Editor Mode", "Shortcut to OpenDRIVE Editor Mode", EUserInterfaceActionType::ToggleButton, FInputChord(EKeys::G, false, true, true, false));
	ToolCommands.Add(OpenDriveGizmoTool);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FOpenDriveEditorModeCommands::GetCommands()
{
	return FOpenDriveEditorModeCommands::Get().Commands;
}

#undef LOCTEXT_NAMESPACE
