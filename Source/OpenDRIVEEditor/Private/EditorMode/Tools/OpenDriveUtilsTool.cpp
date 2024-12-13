#include "EditorMode/Tools/OpenDriveUtilsTool.h"
#include "OpenDrivePosition.h"
#include "InteractiveToolManager.h"
#include "EditorModeManager.h"
#include "OpenDriveUtils.h"
#include "EditorMode/OpenDriveEditorMode.h"

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
	
	Properties = NewObject<UOpenDriveUtilsToolProperties>(this); 
	AddToolPropertySource(Properties);
	
	Properties->OnAlignActorWithLane.BindLambda([this]()
	{
		if (GetEditorMode())
		{
			GetEditorMode()->AlignActorWithLane();
		}
	});

	Properties->OnLaneChange.BindLambda([this](const int NewLaneId)
	{
		if (GetEditorMode())
		{
			GetEditorMode()->ChangeActorLane(NewLaneId);
		}
	});

	Properties->OnRepeatAlongRoad.BindLambda([this] (const float Step, const bool bAlignWithLaneDirection)
	{
		if (GetEditorMode())
		{
			GetEditorMode()->RepeatAlongRoad(Step, bAlignWithLaneDirection);
		}
	});
	
	Properties->OnUpdateActorTransform.BindUObject(this, &UOpenDriveUtilsTool::UpdateActorTransform);
	
	SelectionChangedHandle = GetEditorMode()->OnActorSelectionChanged.AddLambda([this](AActor* SelectedActor)
	{
		Properties->SelectedActor = SelectedActor;
		Properties->UpdateLaneInfo();
		if (Properties->SelectedActor != nullptr)
		{
			Properties->ActorTransformInfoHandle.Reset();
			Properties->ActorTransformInfoHandle = Properties->SelectedActor->GetRootComponent()->TransformUpdated.AddLambda([this](USceneComponent* SceneComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
			{
				Properties->UpdateLaneInfo();
			});
		}
	});

	Properties->SelectedActor = GetEditorMode()->GetSelectedActor();
}

void UOpenDriveUtilsTool::Shutdown(EToolShutdownType ShutdownType)
{
	Properties->OnAlignActorWithLane.Unbind();
	Properties->OnUpdateActorTransform.Unbind();
	Properties->OnLaneChange.Unbind();
	Properties->OnRepeatAlongRoad.Unbind();
	Properties->ActorTransformInfoHandle.Reset();

	if (GetEditorMode())
	{
		GetEditorMode()->OnActorSelectionChanged.Remove(SelectionChangedHandle);
	}
	
	UInteractiveTool::Shutdown(ShutdownType);
}

UOpenDriveEditorMode* UOpenDriveUtilsTool::GetEditorMode() const
{
	if (GLevelEditorModeTools().IsModeActive(UOpenDriveEditorMode::EM_OpenDriveEditorModeId))
	{
		return static_cast<UOpenDriveEditorMode*>(GLevelEditorModeTools().GetActiveScriptableMode(UOpenDriveEditorMode::EM_OpenDriveEditorModeId));
	}
	return nullptr;
}

void UOpenDriveUtilsTool::UpdateActorTransform() const
{
	if (Properties->SelectedActor == nullptr || GetEditorMode() == nullptr) return;
	
	UOpenDrivePosition* OpenDrivePosition = NewObject<UOpenDrivePosition>();
	OpenDrivePosition->SetTrackPosition(roadmanager::Position::Position(Properties->RoadId, Properties->S, Properties->T));
	FTransform OpenDriveTransform = OpenDrivePosition->GetTransform();
	const FTransform FinalTransform = FOpenDriveUtils::OdrToUE(Properties->SelectedActor, OpenDriveTransform, NoAlignment, GetEditorMode()->bOverrideHeight);
	Properties->SelectedActor->SetActorTransform(FinalTransform);
	GetEditorMode()->ResetGizmo();
}

void UOpenDriveUtilsToolProperties::PostEditChangeProperty(FPropertyChangedEvent& e)
{
	Super::PostEditChangeProperty(e);
	
	if (SelectedActor == nullptr) return;
	
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

void UOpenDriveUtilsToolProperties::UpdateLaneInfo()
{
	if (SelectedActor == nullptr) return;
	
	UOpenDrivePosition* Position = NewObject<UOpenDrivePosition>();
	Position->SetTransform(SelectedActor->GetActorTransform());
	RoadId = Position->GetRoadId();
	LaneId = Position->GetLaneId();
	S = UuToMeters(Position->GetS());
	T = UuToMeters(Position->GetRealT());
	
	if (const roadmanager::Road* Road = Position->OdrPosition().GetRoad(); Road != nullptr)
	{
		const roadmanager::LaneSection* LaneSection = Road->GetLaneSectionByS(S);
		
		const int32 Left = LaneSection->GetNUmberOfLanesLeft();
		const int32 Right = -LaneSection->GetNUmberOfLanesRight();
		const float LaneLenght = Road->GetLength();
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
	}
}

#undef LOCTEXT_NAMESPACE
