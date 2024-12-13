#include "EditorMode/OpenDriveEditorModeCommands.h"

#include "RoadManager.hpp"
#include "EditorMode/OpenDriveEditorModeStyle.h"

#define LOCTEXT_NAMESPACE "OpenDriveEditorModeCommands"

FOpenDriveEditorModeCommands::FOpenDriveEditorModeCommands() :
	TCommands<FOpenDriveEditorModeCommands>
	("OpenDriveEditorModeCommands",
	NSLOCTEXT("OpenDriveEditorModeCommands", "OpenDriveEditorModeCommands", "OpenDRIVE Editor Commands"),
	NAME_None,
	FOpenDriveEditorModeStyleSet::GetStyleSetName()
	)
{}

void FOpenDriveEditorModeCommands::OnStartupModule()
{
	Register();
}

void FOpenDriveEditorModeCommands::OnShutdownModule()
{
	Unregister();
}

void FOpenDriveEditorModeCommands::RegisterCommands()
{
	TArray<TSharedPtr<FUICommandInfo>>& ToolCommands = Commands.FindOrAdd(NAME_Default);
	/* Register our tool commands here */
	UI_COMMAND(OpenDriveVisualizerTool, "Viewer", "Draws the current level OpenDrive lanes in world",
		EUserInterfaceActionType::Button,
		FInputChord());
	ToolCommands.Add(OpenDriveVisualizerTool);
	
	UI_COMMAND(OpenDriveUtilsTool, "Actors", "OpenDrive's actors utility functions",
		EUserInterfaceActionType::Button,
		FInputChord());
	ToolCommands.Add(OpenDriveUtilsTool);
	
	UI_COMMAND(OpenDriveSwitchToEditorMode, "OpenDRIVE Editor Mode", "Shortcut to OpenDRIVE Editor Mode",
		EUserInterfaceActionType::ToggleButton,
		FInputChord(EKeys::G, false, true, true, false));
	
	UI_COMMAND(OpenDriveAutoAlignToLane, "Gizmo auto align to lane", "Enables/Disables auto align with lane for OpenDrive gizmo",
		EUserInterfaceActionType::ToggleButton,
		FInputChord());

	UI_COMMAND(OpenDriveOverrideActorHeight, "Override Actor height", "Enables this to override Actor's height with OpenDrive's height while translating with Gizmo",
	EUserInterfaceActionType::ToggleButton,
	FInputChord());
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FOpenDriveEditorModeCommands::GetCommands()
{
	return Get().Commands;
}

#undef LOCTEXT_NAMESPACE
