#include "EditorMode/Tools/OpenDriveUtilsTool.h"
#include "EditorMode/OpenDriveEditorLane.h"
#include "OpenDrivePosition.h"
#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "Engine/DecalActor.h"
#include "EditorMode/OpenDriveEditorMode.h"

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
	Properties->OnAlignActorWithLane.Unbind();
	USelection::SelectObjectEvent.Remove(OnActorSelectedHandle);
	UInteractiveTool::Shutdown(ShutdownType);
}

void UOpenDriveUtilsTool::OnActorSelected(UObject* selectedObject)
{
	AActor* selectedActor = Cast<AActor>(selectedObject);

	if (IsValid(selectedActor))
	{
		if (Properties->SelectedActor != nullptr)
		{
			Properties->SelectedActor->GetRootComponent()->TransformUpdated.Remove(Properties->ActorTransformInfoHandle);
		}
		Properties->SelectedActor = selectedActor;
		Properties->ActorTransformInfoHandle = selectedActor->GetRootComponent()->TransformUpdated.AddUObject(Properties, &UOpenDriveUtilsToolProperties::UpdateActorInfo);
		roadmanager::Position position = CoordTranslate::UeToOdr::FromTransfrom(selectedActor->GetActorTransform());
		Properties->S = position.GetS();
		Properties->T = position.GetT();
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

#pragma endregion

void UOpenDriveUtilsToolProperties::PostEditChangeProperty(FPropertyChangedEvent& e)
{
	Super::PostEditChangeProperty(e);
	FName PropertyName = (e.MemberProperty != NULL) ? e.MemberProperty->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UOpenDriveUtilsToolProperties, S) || PropertyName == GET_MEMBER_NAME_CHECKED(UOpenDriveUtilsToolProperties, T))
	{
		if (SelectedActor != nullptr)
		{
			UpdateActorTransform();
		}
	}
}

void UOpenDriveUtilsToolProperties::UpdateActorInfo(USceneComponent* SceneComponent, EUpdateTransformFlags UpdateTransformFlag, ETeleportType Teleport)
{
	if (SelectedActor != nullptr)
	{
		roadmanager::Position position = CoordTranslate::UeToOdr::FromTransfrom(SceneComponent->GetComponentTransform());
		S = position.GetS();
		T = position.GetT();
	}
}

void UOpenDriveUtilsToolProperties::UpdateActorTransform()
{
	UOpenDrivePosition* Position = NewObject<UOpenDrivePosition>();
	Position->SetTransform(SelectedActor->GetActorTransform());
	Position->SetT(T);
	Position->SetS(S);
	SelectedActor->SetActorTransform(Position->GetTransform());
}

#undef LOCTEXT_NAMESPACE

