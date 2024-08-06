/* Editor mode, tookit, commands and style */
#include "EditorMode/OpenDriveEditorMode.h"
#include "EditorMode/OpenDriveEditorModeToolkit.h"
#include "EditorMode/OpenDriveEditorModeCommands.h"
#include "EditorMode/OpenDriveEditorModeStyle.h"

/* Tools */
#include "EditorMode/Tools/OpenDriveVisualizerTool.h"
#include "EditorMode/Tools/OpenDriveUtilsTool.h"

#include "EdModeInteractiveToolsContext.h"
#include "InteractiveToolManager.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "OpenDriveEditorMode"

const FEditorModeID UOpenDriveEditorMode::EM_OpenDriveEditorModeId = TEXT("EM_OpenDrive");
FString UOpenDriveEditorMode::OpenDriveVisualizerToolName = TEXT("Odr_VisualizationTool");
FString UOpenDriveEditorMode::OpenDriveUtilsToolName = TEXT("Odr_UtilsTool");

UOpenDriveEditorMode::UOpenDriveEditorMode()
{
	Info = FEditorModeInfo(
		UOpenDriveEditorMode::EM_OpenDriveEditorModeId,
		LOCTEXT("OpenDrive", "OpenDrive"),
		FSlateIcon(FOpenDriveEditorModeStyleSet::GetStyleSetName(), "OpenDriveEditorModeIcon", "OpenDriveEditorModeIcon.Small"),
		true);
}

UOpenDriveEditorMode::~UOpenDriveEditorMode() {}

void UOpenDriveEditorMode::Enter()
{
	UEdMode::Enter();

	const FOpenDriveEditorModeCommands& ToolCommands = FOpenDriveEditorModeCommands::Get();

	/* Register tools */
	RegisterTool(ToolCommands.OpenDriveVisualizerTool, OpenDriveVisualizerToolName, NewObject<UOpenDriveVisualizerToolBuilder>(this),EToolsContextScope::EdMode);
	RegisterTool(ToolCommands.OpenDriveUtilsTool, OpenDriveUtilsToolName, NewObject<UOpenDriveUtilsToolBuilder>(this),EToolsContextScope::EdMode);

	/* Set the default active tool (currently visualizer) */
	GetToolManager()->SelectActiveToolType(EToolSide::Left, OpenDriveVisualizerToolName);
	GetToolManager()->ActivateTool(EToolSide::Left);
}

void UOpenDriveEditorMode::Exit()
{
	UEdMode::Exit();
}

void UOpenDriveEditorMode::CreateToolkit()
{
	/* Create OpenDriveEditorModeToolkit */
	Toolkit = MakeShareable(new FOpenDriveEditorModeToolkit);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> UOpenDriveEditorMode::GetModeCommands() const
{
	return FOpenDriveEditorModeCommands::Get().GetCommands();
}

#undef LOCTEXT_NAMESPACE
