#include "EditorMode/Tools/OpenDriveVisualizerTool.h"
#include "EditorMode/OpenDriveEditorLane.h"
#include "EditorMode/OpenDriveEditorLane.h"
#include "OpenDriveSolver.h"

#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"

#define LOCTEXT_NAMESPACE "OpenDriveVisualizerTool"

#pragma region OpenDriveVisualizerToolBuilder
UInteractiveTool* UOpenDriveVisualizerToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UOpenDriveVisualizerTool* NewTool = NewObject<UOpenDriveVisualizerTool>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}
#pragma endregion

#pragma region UOpenDriveVisualizerTool
void UOpenDriveVisualizerTool::SetWorld(UWorld* World)
{
	TargetWorld = World;
}

void UOpenDriveVisualizerTool::Setup()
{
	UInteractiveTool::Setup();
	
	/* Create ToolProperties */
	Properties = NewObject<UOpenDriveVisualizerToolProperties>(this);

	/* Bind events to function */
	Properties.Get()->OnGenerateVisualization.BindUObject(this, &UOpenDriveVisualizerTool::Generate);
	Properties.Get()->OnChangeLaneVisibility.BindUObject(this, &UOpenDriveVisualizerTool::HideShowLanes);
	OnActorSelectedHandle = USelection::SelectObjectEvent.AddUObject(this, &UOpenDriveVisualizerTool::OnActorSelected);

	/* Add properties */
	AddToolPropertySource(Properties);

	/* Calls generate when entering tool */
	Generate(Properties.Get()->RoadOffset, Properties.Get()->Step);
}

void UOpenDriveVisualizerTool::Shutdown(EToolShutdownType ShutdownType)
{
	/* Unbind all events */
	Properties.Get()->OnGenerateVisualization.Unbind();
	Properties.Get()->OnChangeLaneVisibility.Unbind();
	USelection::SelectObjectEvent.Remove(OnActorSelectedHandle);

	/* Remove All lanes in outliner */
	DeleteAllLanes();

	UInteractiveTool::Shutdown(ShutdownType);
}

void UOpenDriveVisualizerTool::Generate(float offset, float step)
{
	DeleteAllLanes();

	/* Actor Spawn Parameters */
	FActorSpawnParameters spawnParam;
	spawnParam.bHideFromSceneOutliner = true;
	spawnParam.bTemporaryEditorActor = true;
	spawnParam.ObjectFlags = EObjectFlags::RF_Transient;

	/* OpenDrive solver creation */
	UOpenDriveSolver* solver = NewObject<UOpenDriveSolver>();
	TArray<UOpenDriveSolver::LaneRef> laneList = solver->GetAllLanesOfType(/* All */);

	/* Draw roads */
	for (auto lane : laneList)
	{
		AOpenDriveEditorLane* newLane = TargetWorld->SpawnActor<AOpenDriveEditorLane>(spawnParam);
		newLane->Initialize(lane.road, lane.laneSection, lane.lane, offset, step);
	}
}

void UOpenDriveVisualizerTool::HideShowLanes()
{
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(TargetWorld, AOpenDriveEditorLane::StaticClass(), actors);

	for (AActor* actor : actors)
	{
		actor->SetIsTemporarilyHiddenInEditor(!actor->IsHiddenEd());
	}
}

void UOpenDriveVisualizerTool::DeleteAllLanes()
{
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(TargetWorld, AOpenDriveEditorLane::StaticClass(), actors);
	
	for (AActor* actor : actors)
	{
		actor->Destroy();
	}
}

void UOpenDriveVisualizerTool::OnActorSelected(UObject* selectedActor)
{
	AOpenDriveEditorLane* selectedRoad = Cast<AOpenDriveEditorLane>(selectedActor);

	if (IsValid(selectedRoad))
	{
		Properties.Get()->RoadId = selectedRoad->GetRoadId();
		Properties.Get()->JunctionId = selectedRoad->GetJunctionId();
		Properties.Get()->LaneId = selectedRoad->GetLaneId();
		Properties.Get()->LaneType = selectedRoad->GetLaneType();
		Properties.Get()->SuccessorId = selectedRoad->GetSuccessorId();
		Properties.Get()->PredecessorId = selectedRoad->GetPredecessorId();
	}
}
#pragma endregion

#undef LOCTEXT_NAMESPACE
