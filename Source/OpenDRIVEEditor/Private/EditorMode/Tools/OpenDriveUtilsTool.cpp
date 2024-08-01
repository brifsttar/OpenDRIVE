#include "EditorMode/Tools/OpenDriveUtilsTool.h"
#include "EditorMode/OpenDriveEditorLane.h"

#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "OpenDriveUtilsTool"

UInteractiveTool* UOpenDriveUtilsToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UOpenDriveUtilsTool* NewTool = NewObject<UOpenDriveUtilsTool>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}

UOpenDriveUtilsToolProperties::UOpenDriveUtilsToolProperties()
{}

void UOpenDriveUtilsToolProperties::AlignWithLane()
{
	OnAlignActorWithLane.Execute();
}

UOpenDriveUtilsTool::UOpenDriveUtilsTool()
{}

void UOpenDriveUtilsTool::SetWorld(UWorld* World)
{
	TargetWorld = World;
}

void UOpenDriveUtilsTool::Setup()
{
	UInteractiveTool::Setup();

	/* Create ToolProperties */
	Properties = NewObject<UOpenDriveUtilsToolProperties>(this);

	AddToolPropertySource(Properties);

	OnActorSelectedHandle = USelection::SelectObjectEvent.AddUObject(this, &UOpenDriveUtilsTool::OnActorSelected);
}

void UOpenDriveUtilsTool::Shutdown(EToolShutdownType ShutdownType)
{
	Properties.Get()->OnAlignActorWithLane.Unbind();
	USelection::SelectObjectEvent.Remove(OnActorSelectedHandle);

	DestroyGizmo();

	UInteractiveTool::Shutdown(ShutdownType);
}

void UOpenDriveUtilsTool::OnGizmoTransformStateChanged(FTransform CurrentTransform)
{

}

void UOpenDriveUtilsTool::CreateGizmo(FTransform InitialTransform, USceneComponent* AttachedComponent)
{
	ETransformGizmoSubElements SubElements = ETransformGizmoSubElements::TranslatePlaneXY;

	UCombinedTransformGizmo* NewGizmo = UE::TransformGizmoUtil::CreateCustomTransformGizmo(GetToolManager(), SubElements, this, *GizmoIdentifier);

	NewGizmo->bUseContextGizmoMode = false;
	NewGizmo->ActiveGizmoMode = EToolContextTransformGizmoMode::Translation;

	NewGizmo->bUseContextCoordinateSystem = false;
	NewGizmo->CurrentCoordinateSystem = EToolContextCoordinateSystem::World;

	NewGizmo->bSnapToWorldGrid = true;

	UTransformProxy* GizmoProxy = NewObject<UTransformProxy>(this);
	GizmoProxy->SetTransform(InitialTransform);
	NewGizmo->SetActiveTarget(GizmoProxy, GetToolManager());

	GizmoProxy->OnTransformChanged.AddWeakLambda(this, [this](UTransformProxy*, FTransform NewTransform)
	{
		OnGizmoTransformStateChanged(NewTransform);
	});

	GizmoProxy->AddComponent(AttachedComponent);

	Gizmo = NewGizmo;
}

void UOpenDriveUtilsTool::DestroyGizmo()
{
	if (Gizmo != nullptr)
	{
		GetToolManager()->GetPairedGizmoManager()->DestroyGizmo(Gizmo);
	}
}

void UOpenDriveUtilsTool::OnActorSelected(UObject* selectedObject)
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

