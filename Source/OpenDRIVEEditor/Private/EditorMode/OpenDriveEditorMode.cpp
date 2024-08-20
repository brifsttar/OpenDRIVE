/* Editor mode, tookit, commands and style */
#include "EditorMode/OpenDriveEditorMode.h"
#include "EditorMode/OpenDriveEditorModeToolkit.h"
#include "EditorMode/OpenDriveEditorModeCommands.h"
#include "EditorMode/OpenDriveEditorModeStyle.h"

/* Tools */
#include "EditorMode/Tools/OpenDriveVisualizerTool.h"
#include "EditorMode/Tools/OpenDriveUtilsTool.h"
#include "EditorMode/Tools/OpenDriveGizmo.h"

#include "Kismet/GameplayStatics.h"
#include "Selection.h"

#define LOCTEXT_NAMESPACE "OpenDriveEditorMode"

const FEditorModeID UOpenDriveEditorMode::EM_OpenDriveEditorModeId = TEXT("EM_OpenDrive");

FString UOpenDriveEditorMode::OpenDriveVisualizerToolName = TEXT("Odr_VisualizationTool");
FString UOpenDriveEditorMode::OpenDriveUtilsToolName = TEXT("Odr_UtilsTool");

FString UOpenDriveEditorMode::GizmoIdentifier = TEXT("OpenDriveGizmo");
FString UOpenDriveEditorMode::GizmoBuilderIdentifier = TEXT("OpenDriveGizmoBuilder");
FString UOpenDriveEditorMode::GizmoAxisBuilderIdentifier = TEXT("OpenDriveAxisGizmoBuilder");

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

	RegisterGizmoBuilder();

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
	UnregisterGizmoBuilder();
	DestroyGizmo();
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
	USelection* SelectedActors = GEditor->GetSelectedActors();
	TArray<AActor*> SelectedActorArray;
	
	for (FSelectionIterator It(GEditor->GetSelectedActorIterator()); It; ++It)
	{
		AActor* SelectedActor = Cast<AActor>(*It);
		if (IsValid(SelectedActor))
		{
			DestroyGizmo();
			CreateGizmo(SelectedActor->GetActorTransform(), SelectedActor->GetRootComponent());
			return;
		}
	}
}

void UOpenDriveEditorMode::RegisterGizmoBuilder()
{
	UE::TransformGizmoUtil::RegisterTransformGizmoContextObject(GetInteractiveToolsContext());

	UGizmoViewContext* GizmoViewContext = GetToolManager()->GetContextObjectStore()->FindContext<UGizmoViewContext>();
	if (!GizmoViewContext)
	{
		GizmoViewContext = NewObject<UGizmoViewContext>();
		GetToolManager()->GetContextObjectStore()->AddContextObject(GizmoViewContext);
	}

	UOpenDriveAxisGizmoBuilder* CustomAxisGizmoBuilder = NewObject<UOpenDriveAxisGizmoBuilder>();
	GetToolManager()->GetPairedGizmoManager()->RegisterGizmoType(*GizmoAxisBuilderIdentifier, CustomAxisGizmoBuilder);
	UOpenDriveGizmoBuilder::RegisterGizmoBuilder(GetToolManager()->GetPairedGizmoManager(), GizmoViewContext, GizmoAxisBuilderIdentifier, GizmoBuilderIdentifier);
}

void UOpenDriveEditorMode::UnregisterGizmoBuilder()
{
	GetToolManager()->GetPairedGizmoManager()->DeregisterGizmoType(*GizmoBuilderIdentifier);
	UE::TransformGizmoUtil::DeregisterTransformGizmoContextObject(GetInteractiveToolsContext());
}

void UOpenDriveEditorMode::CreateGizmo(FTransform InitialTransform, USceneComponent* AttachedComponent)
{
	UOpenDriveGizmo* NewGizmo = UOpenDriveGizmo::CreateGizmo(GetToolManager()->GetPairedGizmoManager(), GizmoBuilderIdentifier, GizmoIdentifier);
	UTransformProxy* GizmoProxy = NewObject<UTransformProxy>(this);
	GizmoProxy->bRotatePerObject = true;
	GizmoProxy->SetTransform(InitialTransform);
	GizmoProxy->AddComponent(AttachedComponent);
	NewGizmo->SetActiveTarget(GizmoProxy, GetToolManager());
}

void UOpenDriveEditorMode::DestroyGizmo()
{
	UInteractiveGizmo* Gizmo = GetToolManager()->GetPairedGizmoManager()->FindGizmoByInstanceIdentifier(GizmoIdentifier);

	if (Gizmo)
	{
		GetToolManager()->GetPairedGizmoManager()->DestroyGizmo(Gizmo);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Gizmo not found."));
	}
}

void UOpenDriveEditorMode::OnActorSelected(UObject* selectedObject)
{
	AActor* selectedActor = Cast<AActor>(selectedObject);

	if (IsValid(selectedActor))
	{
		USceneComponent* Root = selectedActor->GetRootComponent();
		if (IsValid(Root))
		{
			CreateGizmo(Root->GetComponentTransform(), Root);
		}
	}
}



#undef LOCTEXT_NAMESPACE
