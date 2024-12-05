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

	OpenDrivePosition = NewObject<UOpenDrivePosition>(this);
	
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

void UOpenDriveUtilsTool::OnActorSelected(UObject* SelectedObject)
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
	}
}

void UOpenDriveUtilsTool::AlignActorWithLane() const
{
	FTransform ActorTransform = Properties->SelectedActor->GetTransform();
	const bool bIsDecal = Cast<ADecalActor>(Properties->SelectedActor) != nullptr;
	if (bIsDecal)
	{
		ActorTransform.SetRotation(ActorTransform.GetRotation() * FRotator(90, 180, -90).Quaternion());
	}
	OpenDrivePosition->SetTransform(ActorTransform);
	OpenDrivePosition->AlignWithLaneCenter();
	FTransform NewTransform = OpenDrivePosition->GetTransform();
	if (bIsDecal)
	{
		NewTransform.SetRotation(NewTransform.GetRotation() * FRotator(90, 180, 90).Quaternion());
	}
	Properties->SelectedActor->SetActorTransform(NewTransform);
	GetEditorMode()->ActorSelectionChangeNotify();
}
														
void UOpenDriveUtilsTool::UpdateActorTransform() const
{		
	OpenDrivePosition->SetTransform(Properties->SelectedActor->GetActorTransform());
	OpenDrivePosition->SetT(Properties->T);
	OpenDrivePosition->SetS(Properties->S);
	Properties->SelectedActor->SetActorTransform(OpenDrivePosition->GetTransform());

	GetEditorMode()->ActorSelectionChangeNotify();
}

void UOpenDriveUtilsTool::ChangeActorLane(const int32 Direction) const
{
	if (Properties->SelectedActor)
	{
		OpenDrivePosition->SetTransform(Properties->SelectedActor->GetActorTransform());
		OpenDrivePosition->SetLaneById(Direction);
		Properties->SelectedActor->SetActorTransform(OpenDrivePosition->GetTransform());
		AlignActorWithLane();
	}
}

void UOpenDriveUtilsTool::RepeatAlongRoad(const float Step) const
{
	if (Step == 0)
	{
		UE_LOG(LogOpenDriveEditorMode, Warning, TEXT("Step is 0, return..."));
		return;
	}

	auto ModifyTransformForDecal = [](const bool bIsDecal, FTransform Transform)
	{
		if (bIsDecal)
		{
			Transform.SetRotation(Transform.GetRotation() * FRotator(90, 180, -90).Quaternion());
		}
		return Transform;
	};

	AActor* SelectedActor = Properties->SelectedActor;
	const bool bIsDecal = Cast<ADecalActor>(SelectedActor) != nullptr;
	FTransform ActorTransform = SelectedActor->GetActorTransform();
	ActorTransform = ModifyTransformForDecal(bIsDecal, ActorTransform);
	FString FolderPath = SelectedActor->GetFolderPath().ToString();
	FolderPath.Append("/RoadDuplicates");
	const FString ActorLabel = SelectedActor->GetActorLabel();

	UOpenDrivePosition* Position = NewObject<UOpenDrivePosition>();
	Position->SetTransform(ActorTransform);

	UEditorActorSubsystem* ActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();

	int i = 0;
	do
	{
		i++;
		if (Position->MoveAlongS(Step))
		{
			AActor* NewActor = ActorSubsystem->DuplicateActor(SelectedActor, GetWorld(), FVector::ZeroVector);
			NewActor->SetActorTransform(ModifyTransformForDecal(bIsDecal, Position->GetTransform()));
			NewActor->SetFolderPath(FName(FolderPath));
			NewActor->SetActorLabel(ActorLabel + "_" + FString::FromInt(i));
		}
	} 
	while (Position->GetNextJunctionDistance() < Step);
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
	const FName PropertyName = (e.MemberProperty != nullptr) ? e.MemberProperty->GetFName() : NAME_None;
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
		UOpenDrivePosition* Position = NewObject<UOpenDrivePosition>();
		Position->SetTransform(SelectedActor->GetActorTransform());
		S = Position->GetS();
	}
}

void UOpenDriveUtilsToolProperties::UpdateLaneInfo(const USceneComponent* SceneComponent)
{
	const roadmanager::Position Position = CoordTranslate::UeToOdr::FromTransfrom(SceneComponent->GetComponentTransform());
	if (const roadmanager::Road* Road = Position.GetRoad(); Road != nullptr)
	{
		FProperty* Property = FindFProperty<FProperty>(UOpenDriveUtilsToolProperties::StaticClass(), "LaneId");
		const roadmanager::LaneSection* LaneSection = Road->GetLaneSectionByS(S);
		const int Left = LaneSection->GetNUmberOfLanesLeft();
		const int Right = -LaneSection->GetNUmberOfLanesRight();
		Property->AppendMetaData(
			TMap<FName, FString>{
				{TEXT("ClampMin"), FString::FromInt(Right)},
				{TEXT("ClampMax"), FString::FromInt(Left)}
		});
		LaneId = Position.GetLaneId();
	}
}

#undef LOCTEXT_NAMESPACE
