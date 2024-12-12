#include "EditorMode/Tools/OpenDriveUtilsTool.h"
#include "OpenDrivePosition.h"
#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "EditorModeManager.h"
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

void UOpenDriveUtilsTool::ChangeActorLane(const int32 Direction) const
{
	if (Properties->SelectedActor)
	{
		OpenDrivePosition->SetTransform(Properties->SelectedActor->GetActorTransform());
		OpenDrivePosition->SetLaneById(Direction);
		if (FVector::Distance(OpenDrivePosition->GetTransform().GetLocation(), Properties->SelectedActor->GetActorLocation()) > 10)
		{
			Properties->SelectedActor->SetActorTransform(OpenDrivePosition->GetTransform());
			AlignActorWithLane();
		}
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
	if (IsValid(SelectedActor))
	{
		const FName PropertyName = (e.MemberProperty != nullptr) ? e.MemberProperty->GetFName() : NAME_None;
		if ( PropertyName == GET_MEMBER_NAME_CHECKED(UOpenDriveUtilsToolProperties, LaneId))
		{
			OnLaneChange.Execute(LaneId);
		}
		if (PropertyName == GET_MEMBER_NAME_CHECKED(UOpenDriveUtilsToolProperties, S) || PropertyName == GET_MEMBER_NAME_CHECKED(UOpenDriveUtilsToolProperties, T))
		{
			OnUpdateActorTransform.Execute();
		}
	}
}

void UOpenDriveUtilsTool::UpdateActorTransform() const
{
	OpenDrivePosition->SetTrackPosition(roadmanager::Position::Position(Properties->RoadId, Properties->S, Properties->T));
	Properties->SelectedActor->SetActorTransform(OpenDrivePosition->GetTransform());
	GetEditorMode()->ActorSelectionChangeNotify();
}

void UOpenDriveUtilsToolProperties::UpdateActorInfo(USceneComponent* SceneComponent, EUpdateTransformFlags UpdateTransformFlag, ETeleportType Teleport)
{
	if (SelectedActor != nullptr)
	{
		UOpenDrivePosition* Position = NewObject<UOpenDrivePosition>();
		Position->SetTransform(SelectedActor->GetActorTransform());
		UpdateLaneInfo(SceneComponent);
	}
}

void UOpenDriveUtilsToolProperties::UpdateLaneInfo(const USceneComponent* SceneComponent)
{
	UOpenDrivePosition* Position = NewObject<UOpenDrivePosition>();
	Position->SetTransform(SelectedActor->GetActorTransform());
	if (const roadmanager::Road* Road = Position->OdrPosition().GetRoad(); Road != nullptr)
	{
		const roadmanager::LaneSection* LaneSection = Road->GetLaneSectionByS(S);
		
		const int32 Left = LaneSection->GetNUmberOfLanesLeft();
		const int32 Right = -LaneSection->GetNUmberOfLanesRight();
		const float LaneLenght =  LaneSection->GetLength();
		const float RightWidth = Road->GetWidth(UuToMeters(Position->GetS()), -1);
		const float LeftWidth = Road->GetWidth(UuToMeters(Position->GetS()), 1);
		
		FindFProperty<FProperty>(StaticClass(), "LaneId")->AppendMetaData(
			TMap<FName, FString>{
				{TEXT("ClampMin"), FString::FromInt(Right)},
				{TEXT("ClampMax"), FString::FromInt(Left)}
		});
		
		FindFProperty<FProperty>(StaticClass(), "S")->AppendMetaData(
			TMap<FName, FString>{
				{TEXT("ClampMin"), FString::SanitizeFloat(0+0.25)},
				{TEXT("ClampMax"), FString::SanitizeFloat(LaneLenght-0.25)}
		});
		
		FindFProperty<FProperty>(StaticClass(), "T")->AppendMetaData(
			TMap<FName, FString>{
				{TEXT("ClampMin"), FString::SanitizeFloat(-RightWidth)},
				{TEXT("ClampMax"), FString::SanitizeFloat(LeftWidth)}
		});

		RoadId = Position->GetRoadId();
		LaneId = Position->GetLaneId();
		S = UuToMeters(Position->GetS());
		T = UuToMeters(Position->GetRealT());
	}
}

#undef LOCTEXT_NAMESPACE
