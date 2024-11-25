#include "EditorMode/OpenDriveEditorModeCommands.h"

#define LOCTEXT_NAMESPACE "OpenDriveEditorModeCommands"

FOpenDriveEditorModeCommands::FOpenDriveEditorModeCommands() :
	TCommands<FOpenDriveEditorModeCommands>
	("OpenDriveEditorMode",
	NSLOCTEXT("OpenDriveEditorMode", "OpenDriveEditorModeCommands", "OpenDrive Editor Mode"),
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
	UI_COMMAND(OpenDriveGizmoTool, "Gizmo", "OpenDrive's gizmo", EUserInterfaceActionType::ToggleButton, FInputChord(EKeys::G, false, true, true, false));
	ToolCommands.Add(OpenDriveGizmoTool);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FOpenDriveEditorModeCommands::GetCommands()
{
	return FOpenDriveEditorModeCommands::Get().Commands;
}

#undef LOCTEXT_NAMESPACE
