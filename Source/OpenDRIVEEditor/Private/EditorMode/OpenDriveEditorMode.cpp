#include "EditorMode/OpenDriveEditorMode.h"
#include "ContextObjectStore.h"
#include "EdModeInteractiveToolsContext.h"
#include "Selection.h"
#include "BaseGizmos/GizmoViewContext.h"
#include "EditorMode/OpenDriveEditorModeToolkit.h"
#include "EditorMode/OpenDriveEditorModeCommands.h"
#include "EditorMode/OpenDriveEditorModeStyle.h"
#include "EditorMode/Tools/OpenDriveVisualizerTool.h"
#include "EditorMode/Tools/OpenDriveUtilsTool.h"
#include "Tools/Gizmo/SubGizmos/OpenDriveAlignToLaneGizmo.h"
#include "Tools/Gizmo/OpenDriveGizmo.h"
#include "Tools/Gizmo/SubGizmos/OpenDriveTranslationGizmo.h"

DEFINE_LOG_CATEGORY(LogOpenDriveEditorMode);

#define LOCTEXT_NAMESPACE "OpenDriveEditorMode"

const FEditorModeID UOpenDriveEditorMode::EM_OpenDriveEditorModeId = TEXT("EM_OpenDrive");

const FString UOpenDriveEditorMode::OpenDriveVisualizerToolName = TEXT("Odr_VisualizationTool");
const FString UOpenDriveEditorMode::OpenDriveUtilsToolName = TEXT("Odr_UtilsTool");

const FString UOpenDriveEditorMode::OpenDriveChangeLaneBuilderIdentifier = TEXT("OpenDriveChangeLane_Builder");
const FString UOpenDriveEditorMode::OpenDriveMoveAlongLaneBuilderIdentifier = TEXT("OpenDriveMoveAlong_Builder");
const FString UOpenDriveEditorMode::OpenDriveGizmoBuilderIdentifier = TEXT("OpenDriveGizmo_Builder");

const FString UOpenDriveEditorMode::OpenDriveGizmoIdentifier = TEXT("OpenDriveGizmo");

UOpenDriveEditorMode::UOpenDriveEditorMode()
{
	Info = FEditorModeInfo(
		UOpenDriveEditorMode::EM_OpenDriveEditorModeId,
		LOCTEXT("OpenDrive", "OpenDrive"),
		FSlateIcon(FOpenDriveEditorModeStyleSet::GetStyleSetName(), "OpenDriveEditorModeIcon", "OpenDriveEditorModeIcon.Small"),
		true);
}

void UOpenDriveEditorMode::Enter()
{
	UEdMode::Enter();
	
	const FOpenDriveEditorModeCommands& ToolCommands = FOpenDriveEditorModeCommands::Get();

	OpenDrivePosition = NewObject<UOpenDrivePosition>();

	/* Register tools */
	RegisterTool(ToolCommands.OpenDriveVisualizerTool, OpenDriveVisualizerToolName, NewObject<UOpenDriveVisualizerToolBuilder>(this),EToolsContextScope::EdMode);
	RegisterTool(ToolCommands.OpenDriveUtilsTool, OpenDriveUtilsToolName, NewObject<UOpenDriveUtilsToolBuilder>(this),EToolsContextScope::EdMode);

	/* Set the default active tool (currently visualizer) */
	GetToolManager()->SelectActiveToolType(EToolSide::Left, OpenDriveVisualizerToolName);
	GetToolManager()->ActivateTool(EToolSide::Left);

	InitializeOpenDriveGizmo();

	UE_LOG(LogOpenDriveEditorMode, Log, TEXT("OpenDriveEditorMode entered."));
}

void UOpenDriveEditorMode::Exit()
{
	DeInitializeOpenDriveGizmo();
	
	UE_LOG(LogOpenDriveEditorMode, Log, TEXT("OpenDriveEditorMode exited."));
	
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

void UOpenDriveEditorMode::ActorSelectionChangeNotify()
{
	if (USelection* Selection = GEditor->GetSelectedActors())
	{
		TArray<AActor*> SelectedActors;
		Selection->GetSelectedObjects<AActor>(SelectedActors);
		if (SelectedActors.Num() > 0)
		{
			UTransformProxy* TransformProxy = NewObject<UTransformProxy>(this);
			TransformProxy->SetTransform(SelectedActors[0]->GetRootComponent()->GetComponentTransform());
			TransformProxy->AddComponent(SelectedActors[0]->GetRootComponent());
			OpenDriveGizmo->SetActiveTarget(TransformProxy, GetToolManager());
			OpenDriveGizmo->SetVisibility(true);
		}
		else
		{
			OpenDriveGizmo->ClearActiveTarget();
			OpenDriveGizmo->SetVisibility(false);
		}
	}
}

void UOpenDriveEditorMode::InitializeOpenDriveGizmo()
{
	UE::TransformGizmoUtil::RegisterTransformGizmoContextObject(GetInteractiveToolsContext());

	if (UGizmoViewContext* GizmoViewContext = GetToolManager()->GetContextObjectStore()->FindContext<UGizmoViewContext>(); !GizmoViewContext)
	{
		GizmoViewContext = NewObject<UGizmoViewContext>();
		GetToolManager()->GetContextObjectStore()->AddContextObject(GizmoViewContext);
	}
	
	UOpenDriveAlignToLaneGizmoBuilder* OdrSwitchLaneGizmoBuilder = NewObject<UOpenDriveAlignToLaneGizmoBuilder>();
	GetToolManager()->GetPairedGizmoManager()->RegisterGizmoType(*OpenDriveChangeLaneBuilderIdentifier, OdrSwitchLaneGizmoBuilder);
	
	UOpenDriveTranslationGizmoBuilder* OdrMoveAlongGizmoBuilder = NewObject<UOpenDriveTranslationGizmoBuilder>();
	GetToolManager()->GetPairedGizmoManager()->RegisterGizmoType(*OpenDriveMoveAlongLaneBuilderIdentifier, OdrMoveAlongGizmoBuilder);

	UOpenDriveGizmoBuilder* GizmoBuilder = NewObject<UOpenDriveGizmoBuilder>();
	GizmoBuilder->LaneChangeGizmoBuilderIdentifier = OpenDriveChangeLaneBuilderIdentifier;
	GizmoBuilder->MoveAlongLaneGizmoBuilderIdentifier = OpenDriveMoveAlongLaneBuilderIdentifier;
	GetToolManager()->GetPairedGizmoManager()->RegisterGizmoType(*OpenDriveGizmoBuilderIdentifier, GizmoBuilder);

	OpenDriveGizmo = Cast<UOpenDriveGizmo>(GetToolManager()->GetPairedGizmoManager()->CreateGizmo(*OpenDriveGizmoBuilderIdentifier, OpenDriveGizmoIdentifier));
	OpenDriveGizmo->SetVisibility(false);
	ActorSelectionChangeNotify();
}

void UOpenDriveEditorMode::DeInitializeOpenDriveGizmo() const
{
	if (UInteractiveGizmo* Gizmo = GetToolManager()->GetPairedGizmoManager()->FindGizmoByInstanceIdentifier(OpenDriveGizmoIdentifier))
	{
		GetToolManager()->GetPairedGizmoManager()->DestroyGizmo(Gizmo);
	}
	
	GetToolManager()->GetPairedGizmoManager()->DeregisterGizmoType(*OpenDriveGizmoBuilderIdentifier);
	GetToolManager()->GetPairedGizmoManager()->DeregisterGizmoType(*OpenDriveChangeLaneBuilderIdentifier);
	
	UE::TransformGizmoUtil::DeregisterTransformGizmoContextObject(GetInteractiveToolsContext());
}

#undef LOCTEXT_NAMESPACE
