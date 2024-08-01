#include "EditorMode/OpenDriveEditorMode.h"
#include "EditorMode/OpenDriveEditorModeToolkit.h"
#include "EditorMode/OpenDriveEditorModeCommands.h"
#include "EditorMode/OpenDriveEditorModeStyle.h"

#include "EdModeInteractiveToolsContext.h"
#include "InteractiveToolManager.h"

//Tools
#include "EditorMode/Tools/OpenDriveVisualizerTool.h"
#include "EditorMode/Tools/OpenDriveUtilsTool.h"

#define LOCTEXT_NAMESPACE "OpenDriveEditorMode"

const FEditorModeID UOpenDriveEditorMode::EM_OpenDriveEditorModeId = TEXT("EM_OpenDrive");

FString UOpenDriveEditorMode::OpenDriveVisualizerToolName = TEXT("Odr_VisualizationTool");
FString UOpenDriveEditorMode::OpenDriveUtilsToolName = TEXT("Odr_UtilsTool");

UOpenDriveEditorMode::UOpenDriveEditorMode()
{
	FModuleManager::Get().LoadModule("EditorStyle");

	Info = FEditorModeInfo(
		UOpenDriveEditorMode::EM_OpenDriveEditorModeId,
		LOCTEXT("OpenDrive", "OpenDrive"),
		FSlateIcon(FOpenDriveEditorModeStyleSet::GetStyleSetName(), "OpenDriveEditorModeIcon", "OpenDriveEditorModeIcon.Small"),
		true,
		1);
}

UOpenDriveEditorMode::~UOpenDriveEditorMode()
{}

void UOpenDriveEditorMode::Enter()
{
	UEdMode::Enter();

	UE::TransformGizmoUtil::RegisterTransformGizmoContextObject(GetInteractiveToolsContext());

	const FOpenDriveEditorModeCommands& ToolCommands = FOpenDriveEditorModeCommands::Get();

	RegisterTool(ToolCommands.OpenDriveVisualizerTool, OpenDriveVisualizerToolName, NewObject<UOpenDriveVisualizerToolBuilder>(this),EToolsContextScope::EdMode);
	RegisterTool(ToolCommands.OpenDriveUtilsTool, OpenDriveUtilsToolName, NewObject<UOpenDriveUtilsToolBuilder>(this),EToolsContextScope::EdMode);


	GetToolManager()->SelectActiveToolType(EToolSide::Left, OpenDriveVisualizerToolName);
	GetToolManager()->ActivateTool(EToolSide::Left);
}

void UOpenDriveEditorMode::Exit()
{
	UE::TransformGizmoUtil::DeregisterTransformGizmoContextObject(GetInteractiveToolsContext());

	UE::TransformGizmoUtil::DeregisterTransformGizmoContextObject(GetInteractiveToolsContext());

	UEdMode::Exit();
}

void UOpenDriveEditorMode::ActorSelectionChangeNotify()
{

}

void UOpenDriveEditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FOpenDriveEditorModeToolkit);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> UOpenDriveEditorMode::GetModeCommands() const
{
	return FOpenDriveEditorModeCommands::Get().GetCommands();
}

#undef LOCTEXT_NAMESPACE
