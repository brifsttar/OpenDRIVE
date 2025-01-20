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

void UOpenDriveUtilsTool::OnAlignActorWithLane()
{
	if (GetEditorMode())
	{
		GetEditorMode()->AlignActorWithLane();
	}
}

void UOpenDriveUtilsTool::OnLaneChange(int NewLaneId)
{
	if (GetEditorMode())
	{
		GetEditorMode()->ChangeActorLane(NewLaneId);
	}
}

void UOpenDriveUtilsTool::OnRepeatAlongRoad(float Step, bool bAlignWithLaneDirection)
{
	if (GetEditorMode())
	{
		GetEditorMode()->RepeatAlongRoad(Step, bAlignWithLaneDirection);
	}
}

void UOpenDriveUtilsTool::ActorSelectionChanged(AActor* Actor)
{
	Properties->SelectedActor = Actor;
		
	if (Properties->SelectedActor != nullptr)
	{
		Properties->ActorTransformInfoHandle.Reset();
		Properties->ActorTransformInfoHandle = Properties->SelectedActor->GetRootComponent()->TransformUpdated.AddUObject(this, &UOpenDriveUtilsTool::OnActorTransformChanged);
	}
	else
	{
		Properties->ActorTransformInfoHandle.Reset();
	}
}

void UOpenDriveUtilsTool::OnActorTransformChanged(USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	Properties->UpdateLaneInfo();
}

void UOpenDriveUtilsTool::Setup()
{
	UInteractiveTool::Setup();
	Properties = NewObject<UOpenDriveUtilsToolProperties>(this); 
	AddToolPropertySource(Properties);
	Properties->OnAlignActorWithLane.BindUObject(this, &UOpenDriveUtilsTool::OnAlignActorWithLane);
	Properties->OnLaneChange.BindUObject(this, &UOpenDriveUtilsTool::OnLaneChange);
	Properties->OnRepeatAlongRoad.BindUObject(this, &UOpenDriveUtilsTool::OnRepeatAlongRoad);
	Properties->OnUpdateActorTransform.BindUObject(this, &UOpenDriveUtilsTool::UpdateActorTransform);
	SelectionChangedHandle = GetEditorMode()->OnActorSelectionChanged.AddUObject(this, &UOpenDriveUtilsTool::ActorSelectionChanged);
	Properties->SelectedActor = GetEditorMode()->GetSelectedActor();
}

void UOpenDriveUtilsTool::Shutdown(EToolShutdownType ShutdownType)
{
	Properties->OnAlignActorWithLane.Unbind();
	Properties->OnUpdateActorTransform.Unbind();
	Properties->OnLaneChange.Unbind();
	Properties->OnRepeatAlongRoad.Unbind();
	Properties->ActorTransformInfoHandle.Reset();
	Properties->SelectedActor = nullptr;

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

void UOpenDriveUtilsTool::UpdateActorTransform(EOpenDriveTranslateType TranslateType) const
{
	if (
		Properties->SelectedActor == nullptr ||
		Properties->OpenDrivePosition == nullptr ||
		GetEditorMode() == nullptr
	) {
		return;
	}
	
	UOpenDrivePosition* OpenDrivePosition = Properties->OpenDrivePosition;
	double S = TranslateType == EOpenDriveTranslateType::TranslateOnS ? Properties->S : OpenDrivePosition->OdrPosition().GetS();
	double T = TranslateType == EOpenDriveTranslateType::TranslateOnT ? Properties->T : OpenDrivePosition->OdrPosition().GetT();
	OpenDrivePosition->UpdateTrackPosition(Properties->RoadId, MetersToUu(S), MetersToUu(T));
	FTransform OpenDriveTransform = OpenDrivePosition->GetTransform();
	Properties->LastKnownTransform = FOpenDriveUtils::OdrToUE(Properties->SelectedActor, OpenDriveTransform, NoAlignment, GetEditorMode()->bOverrideHeight);
	Properties->SelectedActor->SetActorTransform(Properties->LastKnownTransform);
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
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UOpenDriveUtilsToolProperties, S))
	{
		OnUpdateActorTransform.Execute(EOpenDriveTranslateType::TranslateOnS);
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UOpenDriveUtilsToolProperties, T)) {
		OnUpdateActorTransform.Execute(EOpenDriveTranslateType::TranslateOnT);
	}
}

void UOpenDriveUtilsToolProperties::UpdateLaneInfo()
{
	if (SelectedActor == nullptr) {
		return;
	}
	if (OpenDrivePosition == nullptr) {
		OpenDrivePosition = NewObject<UOpenDrivePosition>();
	}
	if (!SelectedActor->GetActorTransform().Equals(LastKnownTransform)) {
		LastKnownTransform = SelectedActor->GetActorTransform();
		OpenDrivePosition->SetTransform(LastKnownTransform);
	}

	RoadId = OpenDrivePosition->GetRoadId();
	LaneId = OpenDrivePosition->GetLaneId();
	S = UuToMeters(OpenDrivePosition->GetS());
	T = UuToMeters(OpenDrivePosition->GetRealT());
	
	if (const roadmanager::Road* Road = OpenDrivePosition->OdrPosition().GetRoad(); Road != nullptr)
	{
		const roadmanager::LaneSection* LaneSection = Road->GetLaneSectionByS(S);
		
		const int32 Left = LaneSection->GetNUmberOfLanesLeft();
		const int32 Right = -LaneSection->GetNUmberOfLanesRight();
		const float LaneLenght = Road->GetLength();
		const float RightWidth = Road->GetWidth(UuToMeters(OpenDrivePosition->GetS()), -1);
		const float LeftWidth = Road->GetWidth(UuToMeters(OpenDrivePosition->GetS()), 1);
		
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
