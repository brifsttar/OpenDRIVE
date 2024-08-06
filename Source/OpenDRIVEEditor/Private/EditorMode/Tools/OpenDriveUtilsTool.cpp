#include "EditorMode/Tools/OpenDriveUtilsTool.h"
#include "EditorMode/OpenDriveEditorLane.h"

#include "OpenDrivePosition.h"

#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "Engine/DecalActor.h"

#define LOCTEXT_NAMESPACE "OpenDriveUtilsTool"

#pragma region OpenDriveUtilsToolBuilder
UInteractiveTool* UOpenDriveUtilsToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UOpenDriveUtilsTool* NewTool = NewObject<UOpenDriveUtilsTool>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}
#pragma endregion 

#pragma region UOpenDriveUtilsTool
void UOpenDriveUtilsTool::SetWorld(UWorld* World)
{
	TargetWorld = World;
}

void UOpenDriveUtilsTool::Setup()
{
	UInteractiveTool::Setup();

	/* Set ToolPropertySet */
	Properties = NewObject<UOpenDriveUtilsToolProperties>(this); 
	AddToolPropertySource(Properties);

	/* Bind events */
	Properties->OnAlignActorWithLane.BindUObject(this, &UOpenDriveUtilsTool::AlignActorWithLane);
	OnActorSelectedHandle = USelection::SelectObjectEvent.AddUObject(this, &UOpenDriveUtilsTool::OnActorSelected);
}

void UOpenDriveUtilsTool::Shutdown(EToolShutdownType ShutdownType)
{
	/* Unbind all events */
	Properties.Get()->OnAlignActorWithLane.Unbind();
	USelection::SelectObjectEvent.Remove(OnActorSelectedHandle);

	//DestroyGizmo();

	UInteractiveTool::Shutdown(ShutdownType);
}

void UOpenDriveUtilsTool::OnActorSelected(UObject* selectedObject)
{
	AActor* selectedActor = Cast<AActor>(selectedObject);

	if (IsValid(selectedActor))
	{
		Properties->SelectedActor = selectedActor;

		//USceneComponent* Root = selectedActor->GetRootComponent();
		//if (IsValid(Root))
		//{
		//	CreateGizmo(Root->GetComponentTransform(), Root);
		//}
	}
}

void UOpenDriveUtilsTool::AlignActorWithLane()
{
	UOpenDrivePosition* openDrivePosition = NewObject<UOpenDrivePosition>();
	FTransform actorTransform = Properties->SelectedActor->GetTransform();
	bool isDecal = Cast<ADecalActor>(Properties->SelectedActor) != nullptr;
	if (isDecal)
	{
		actorTransform.SetRotation(actorTransform.GetRotation() * FRotator(90, 180, -90).Quaternion());
	}
	openDrivePosition->SetTransform(actorTransform);
	openDrivePosition->AlignWithLaneCenter();
	FTransform newTransform = openDrivePosition->GetTransform();
	if (isDecal)
	{
		newTransform.SetRotation(newTransform.GetRotation() * FRotator(90, 180, 90).Quaternion());
	}
	Properties->SelectedActor->SetActorTransform(newTransform);
}

void UOpenDriveUtilsTool::CreateGizmo(FTransform InitialTransform, USceneComponent* AttachedComponent)
{
	ETransformGizmoSubElements SubElements = ETransformGizmoSubElements::TranslateAxisX | ETransformGizmoSubElements::TranslateAxisY;

	UCombinedTransformGizmo* NewGizmo = UE::TransformGizmoUtil::CreateCustomTransformGizmo(GetToolManager(), SubElements, this, *GizmoIdentifier);

	NewGizmo->bUseContextGizmoMode = false;
	NewGizmo->ActiveGizmoMode = EToolContextTransformGizmoMode::Translation;
	NewGizmo->bUseContextCoordinateSystem = false;
	NewGizmo->CurrentCoordinateSystem = EToolContextCoordinateSystem::Local;
	NewGizmo->bSnapToWorldGrid = false;
	NewGizmo->SetUpdateCoordSystemFunction([this, NewGizmo](UPrimitiveComponent* Component, EToolContextCoordinateSystem CoordinateSystem)
		{
			SetCustomCoordinateSystem(Component, CoordinateSystem);
		});

	UTransformProxy* GizmoProxy = NewObject<UTransformProxy>(this);
	GizmoProxy->SetTransform(InitialTransform);
	GizmoProxy->AddComponent(AttachedComponent);

	NewGizmo->SetActiveTarget(GizmoProxy, GetToolManager());

	Gizmo = NewGizmo;
}

void UOpenDriveUtilsTool::DestroyGizmo()
{
	if (Gizmo != nullptr)
	{
		GetToolManager()->GetPairedGizmoManager()->DestroyGizmo(Gizmo);
	}
}

void UOpenDriveUtilsTool::SetCustomCoordinateSystem(UPrimitiveComponent* Component, EToolContextCoordinateSystem TranCoordinateSystemsform)
{
	if (Component != nullptr)
	{
		UOpenDrivePosition* openDrivePosition = NewObject<UOpenDrivePosition>();
		openDrivePosition->SetTransform(Component->GetComponentTransform());
		openDrivePosition->AlignWithLaneCenter();
		FQuat rotation = openDrivePosition->GetTransform().GetRotation();
		Component->SetWorldRotation(rotation);
		Gizmo->ActiveTarget->SetTransform(Component->GetComponentTransform());
	}
}
#pragma endregion

#undef LOCTEXT_NAMESPACE
