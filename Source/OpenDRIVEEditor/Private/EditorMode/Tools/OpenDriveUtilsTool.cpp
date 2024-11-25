#include "EditorMode/Tools/OpenDriveUtilsTool.h"
#include "OpenDrivePosition.h"
#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "CollisionQueryParams.h"
#include "EditorModeManager.h"
#include "Engine/World.h"
#include "Engine/DecalActor.h"
#include "EditorMode/OpenDriveEditorMode.h"
#include "Subsystems/EditorActorSubsystem.h"

#define LOCTEXT_NAMESPACE "OpenDriveUtilsTool"

UInteractiveTool* UOpenDriveUtilsToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UOpenDriveUtilsTool* NewTool = NewObject<UOpenDriveUtilsTool>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}

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
	Properties->OnUpdateActorTransform.BindUObject(this, &UOpenDriveUtilsTool::UpdateActorTransform);
	Properties->OnLaneChange.BindUObject(this, &UOpenDriveUtilsTool::ChangeActorLane);
	Properties->OnRepeatAlongRoad.BindUObject(this, &UOpenDriveUtilsTool::RepeatAlongRoad);

	OnActorSelectedHandle = USelection::SelectionChangedEvent.AddUObject(this, &UOpenDriveUtilsTool::OnActorSelected);
	
	OnActorSelected(nullptr);	
}

void UOpenDriveUtilsTool::Shutdown(EToolShutdownType ShutdownType)
{
	/* Unbind all events */
	Properties->OnAlignActorWithLane.Unbind();
	Properties->OnUpdateActorTransform.Unbind();
	Properties->OnLaneChange.Unbind();
	Properties->OnRepeatAlongRoad.Unbind();

	USelection::SelectionChangedEvent.Remove(OnActorSelectedHandle);

	UInteractiveTool::Shutdown(ShutdownType);
}

void UOpenDriveUtilsTool::OnActorSelected(UObject* selectedObject)
{
	USelection* SelectedActors = GEditor->GetSelectedActors();
	TArray<AActor*> SelectedActorArray;

	if (SelectedActors)
	{
		SelectedActors->GetSelectedObjects<AActor>(SelectedActorArray);
	}

	if (SelectedActorArray.Num() == 0)
	{
		Properties->SelectedActor = nullptr;
	}
	else
	{
		if (IsValid(Properties->SelectedActor))
		{
			Properties->SelectedActor->GetRootComponent()->TransformUpdated.Remove(Properties->ActorTransformInfoHandle);
		}
		Properties->SelectedActor = SelectedActorArray[0];
		Properties->ActorTransformInfoHandle = Properties->SelectedActor->GetRootComponent()->TransformUpdated.AddUObject(Properties, &UOpenDriveUtilsToolProperties::UpdateActorInfo);
		Properties->UpdateActorInfo(Properties->SelectedActor->GetRootComponent(), EUpdateTransformFlags::None, ETeleportType::None);
		Properties->UpdateLaneInfo(Properties->SelectedActor->GetRootComponent());
		AlignActorWithLane();
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
														
void UOpenDriveUtilsTool::UpdateActorTransform()
{		
	UOpenDrivePosition* Position = NewObject<UOpenDrivePosition>();
	Position->SetTransform(Properties->SelectedActor->GetActorTransform());
	Position->SetT(Properties->T);
	Position->SetS(Properties->S);
	Properties->SelectedActor->SetActorTransform(Position->GetTransform());

	FEditorViewportClient* ViewportClient = (FEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
	ViewportClient->Viewport->Invalidate();
}

void UOpenDriveUtilsTool::ChangeActorLane(int32 Direction)
{
	if (Properties->SelectedActor)
	{
		UOpenDrivePosition* Position = NewObject<UOpenDrivePosition>();
		Position->SetTransform(Properties->SelectedActor->GetActorTransform());
		Position->SetLaneById(Direction);
		Properties->SelectedActor->SetActorTransform(Position->GetTransform());
		AlignActorWithLane();
	}
}

void UOpenDriveUtilsTool::RepeatAlongRoad(float step)
{
	if (step == 0)
	{
		UE_LOG(LogOpenDriveEditorMode, Warning, TEXT("Step is 0, return..."));
		return;
	}

	auto ModifyTransformForDecal = [](bool bIsDecal, FTransform Transform)
	{
		if (bIsDecal)
		{
			Transform.SetRotation(Transform.GetRotation() * FRotator(90, 180, -90).Quaternion());
		}
		return Transform;
	};

	AActor* SelectedActor = Properties->SelectedActor;
	bool bIsDecal = Cast<ADecalActor>(SelectedActor) != nullptr;
	FTransform ActorTransform = SelectedActor->GetActorTransform();
	ActorTransform = ModifyTransformForDecal(bIsDecal, ActorTransform);
	FString FolderPath = SelectedActor->GetFolderPath().ToString();
	FolderPath.Append("/RoadDuplicates");
	FString ActorLabel = SelectedActor->GetActorLabel();

	UOpenDrivePosition* Position = NewObject<UOpenDrivePosition>();
	Position->SetTransform(ActorTransform);

	UEditorActorSubsystem* ActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();

	int i = 0;
	do
	{
		i++;
		if (Position->MoveAlongS(step))
		{
			AActor* NewActor = ActorSubsystem->DuplicateActor(SelectedActor, GetWorld(), FVector::ZeroVector);
			NewActor->SetActorTransform(ModifyTransformForDecal(bIsDecal, Position->GetTransform()));
			NewActor->SetFolderPath(FName(FolderPath));
			NewActor->SetActorLabel(ActorLabel + "_" + FString::FromInt(i));
		}
	} 
	while (Position->GetNextJunctionDistance() < step);
}

UOpenDriveEditorMode* UOpenDriveUtilsTool::GetEditorMode() const
{
	if (GLevelEditorModeTools().IsModeActive(UOpenDriveEditorMode::EM_OpenDriveEditorModeId))
	{
		return static_cast<UOpenDriveEditorMode*>(GLevelEditorModeTools().GetActiveScriptableMode(UOpenDriveEditorMode::EM_OpenDriveEditorModeId));
	}
	return nullptr;
}

void UOpenDriveUtilsToolProperties::PostEditChangeProperty(FPropertyChangedEvent& e)
{
	Super::PostEditChangeProperty(e);
	FName PropertyName = (e.MemberProperty != NULL) ? e.MemberProperty->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UOpenDriveUtilsToolProperties, S) || PropertyName == GET_MEMBER_NAME_CHECKED(UOpenDriveUtilsToolProperties, T))
	{
		if (IsValid(SelectedActor))
		{
			OnUpdateActorTransform.Execute();
		}
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UOpenDriveUtilsToolProperties, LaneId))
	{
		OnLaneChange.Execute(LaneId);
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

void UOpenDriveUtilsToolProperties::UpdateLaneInfo(USceneComponent* SceneComponent)
{
	roadmanager::Position position = CoordTranslate::UeToOdr::FromTransfrom(SceneComponent->GetComponentTransform());
	roadmanager::Road* road = position.GetRoad();
	if (road != nullptr)
	{
		FProperty* Property = FindFProperty<FProperty>(UOpenDriveUtilsToolProperties::StaticClass(), "LaneId");
		roadmanager::LaneSection* laneSection = road->GetLaneSectionByS(S);
		int left = laneSection->GetNUmberOfLanesLeft();
		int right = -laneSection->GetNUmberOfLanesRight();
		Property->AppendMetaData(
			TMap<FName, FString>{
				{TEXT("ClampMin"), FString::FromInt(right)},
				{TEXT("ClampMax"), FString::FromInt(left)}
		});
		LaneId = position.GetLaneId();
	}
}

#undef LOCTEXT_NAMESPACE
