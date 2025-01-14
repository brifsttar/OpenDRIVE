#include "EditorMode/OpenDriveEditorMode.h"
#include "ContextObjectStore.h"
#include "EdModeInteractiveToolsContext.h"
#include "OpenDriveFuncLib.h"
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
#include "OpenDriveUtils.h"

DEFINE_LOG_CATEGORY(LogOpenDriveEditorMode);

#define LOCTEXT_NAMESPACE "OpenDriveEditorMode"

const FEditorModeID UOpenDriveEditorMode::EM_OpenDriveEditorModeId = TEXT("EM_OpenDrive");

const FString UOpenDriveEditorMode::OpenDriveVisualizerToolName = TEXT("Odr_VisualizationTool");
const FString UOpenDriveEditorMode::OpenDriveUtilsToolName = TEXT("Odr_UtilsTool");

const FString UOpenDriveEditorMode::OpenDriveChangeLaneBuilderIdentifier = TEXT("OpenDriveChangeLane_Builder");
const FString UOpenDriveEditorMode::OpenDriveMoveAlongLaneBuilderIdentifier = TEXT("OpenDriveMoveAlong_Builder");
const FString UOpenDriveEditorMode::OpenDriveGizmoBuilderIdentifier = TEXT("OpenDriveGizmo_Builder");

const FString UOpenDriveEditorMode::OpenDriveGizmoIdentifier = TEXT("OpenDriveGizmo");

UOpenDriveEditorMode::UOpenDriveEditorMode(): bAutoAlignWithLane(false)
{
	Info = FEditorModeInfo(
				EM_OpenDriveEditorModeId,
		LOCTEXT("OpenDrive", "OpenDrive"),
		FSlateIcon(
				FOpenDriveEditorModeStyleSet::GetStyleSetName(),
	"OpenDriveEditorModeCommands.OpenDriveSwitchToEditorMode",
"OpenDriveEditorModeCommands.OpenDriveSwitchToEditorMode.Small"
					),
		true);
}

void UOpenDriveEditorMode::Enter()
{
	UEdMode::Enter();
	
	const FOpenDriveEditorModeCommands& ToolCommands = FOpenDriveEditorModeCommands::Get();
	
	/* Register tools */
	RegisterTool(ToolCommands.OpenDriveVisualizerTool, OpenDriveVisualizerToolName, NewObject<UOpenDriveVisualizerToolBuilder>(this),EToolsContextScope::EdMode);
	RegisterTool(ToolCommands.OpenDriveUtilsTool, OpenDriveUtilsToolName, NewObject<UOpenDriveUtilsToolBuilder>(this),EToolsContextScope::EdMode);

	/* Set the default active tool (currently visualizer) */
	GetToolManager()->SelectActiveToolType(EToolSide::Left, OpenDriveUtilsToolName);
	GetToolManager()->ActivateTool(EToolSide::Left);

	InitializeOpenDriveGizmo();

	UE_LOG(LogOpenDriveEditorMode, Log, TEXT("OpenDriveEditorMode entered."));
}

void UOpenDriveEditorMode::Exit()
{
	DeInitializeOpenDriveGizmo();
	SelectedActor = nullptr;
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
			OpenDriveGizmo->AutoAlignWithLane(bAutoAlignWithLane);
			OpenDriveGizmo->SetOverrideHeight(bOverrideHeight);
			SelectedActor = SelectedActors[0];
		}
		else
		{
			OpenDriveGizmo->ClearActiveTarget();
			OpenDriveGizmo->SetVisibility(false);
			SelectedActor = nullptr;
		}
		OnActorSelectionChanged.Broadcast(SelectedActor);
	}
}

void UOpenDriveEditorMode::ToggleAutoAlignWithLane()
{
	bAutoAlignWithLane = !bAutoAlignWithLane;
	if (IsValid(OpenDriveGizmo))
	{
		OpenDriveGizmo->AutoAlignWithLane(bAutoAlignWithLane);
	}
}

void UOpenDriveEditorMode::ToggleOverrideHeight()
{
	bOverrideHeight = !bOverrideHeight;
	if (IsValid(OpenDriveGizmo))
	{
		OpenDriveGizmo->SetOverrideHeight(bOverrideHeight);
	}
}

void UOpenDriveEditorMode::ResetGizmo() const
{
	if (OpenDriveGizmo && SelectedActor)
	{
		OpenDriveGizmo->ResetGizmo(SelectedActor->GetRootComponent()->GetComponentTransform());
	}
}

void UOpenDriveEditorMode::AlignActorWithLane()
{
	if (SelectedActor == nullptr) return;
	
	const FTransform InitialTransform = FOpenDriveUtils::UEToOdr(SelectedActor);
	UOpenDrivePosition* OpenDrivePosition = NewObject<UOpenDrivePosition>(this);
	OpenDrivePosition->SetTransform(InitialTransform);
	OpenDrivePosition->AlignWithLaneCenter();
	FTransform OpenDriveTransform = OpenDrivePosition->GetTransform();
	const FTransform FinalTransform = FOpenDriveUtils::OdrToUE(SelectedActor, OpenDriveTransform, AlignToLane, bOverrideHeight);
	SelectedActor->SetActorTransform(FinalTransform);
	ResetGizmo();
}

void UOpenDriveEditorMode::ChangeActorLane(const int NewLaneId)
{
	if (SelectedActor == nullptr) return;
	
	const FTransform InitialTransform = FOpenDriveUtils::UEToOdr(SelectedActor);
	UOpenDrivePosition* OpenDrivePosition = NewObject<UOpenDrivePosition>(this);
	OpenDrivePosition->SetTransform(InitialTransform);
	OpenDrivePosition->SetLaneById(NewLaneId);
	OpenDrivePosition->AlignWithLaneCenter();
	FTransform OpenDriveTransform = OpenDrivePosition->GetTransform();
	const FTransform FinalTransform = FOpenDriveUtils::OdrToUE(SelectedActor, OpenDriveTransform, AlignButKeepDirection, bOverrideHeight);
	SelectedActor->SetActorTransform(FinalTransform);
	ResetGizmo();
}

void UOpenDriveEditorMode::RepeatAlongRoad(const float Step, const bool bAlignWithLaneDirection)
{
	const FTransform InitialTransform = FOpenDriveUtils::UEToOdr(SelectedActor);
	UOpenDrivePosition* OpenDrivePosition = NewObject<UOpenDrivePosition>(this);
	OpenDrivePosition->SetTransform(InitialTransform);
	const int StartingRoadId = OpenDrivePosition->GetRoadId();
	const EAlignmentMethod AlignToLaneDirection = bAlignWithLaneDirection ? AlignToLane : NoAlignment;
	do
	{
		OpenDrivePosition->MoveAlongS(Step);
		AActor* NewActor = UOpenDriveFuncLib::CloneActor(SelectedActor);
		FTransform OpenDriveTransform = OpenDrivePosition->GetTransform();
		const FTransform FinalTransform = FOpenDriveUtils::OdrToUE(NewActor, OpenDriveTransform, AlignToLaneDirection);
		NewActor->SetActorTransform(FinalTransform);
		NewActor->SetFolderPath("/RoadDuplicates");
	}
	while (StartingRoadId == OpenDrivePosition->GetRoadId());
	
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

void UOpenDriveEditorMode::DeInitializeOpenDriveGizmo()
{
	if (UInteractiveGizmo* Gizmo = GetToolManager()->GetPairedGizmoManager()->FindGizmoByInstanceIdentifier(OpenDriveGizmoIdentifier))
	{
		GetToolManager()->GetPairedGizmoManager()->DestroyGizmo(Gizmo);
		OpenDriveGizmo = nullptr;
	}

	GetToolManager()->GetPairedGizmoManager()->DeregisterGizmoType(*OpenDriveGizmoBuilderIdentifier);
	GetToolManager()->GetPairedGizmoManager()->DeregisterGizmoType(*OpenDriveChangeLaneBuilderIdentifier);
	
	UE::TransformGizmoUtil::DeregisterTransformGizmoContextObject(GetInteractiveToolsContext());
}

#undef LOCTEXT_NAMESPACE
