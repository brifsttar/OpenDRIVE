#include "EditorMode/Tools/OpenDriveVisualizerTool.h"
#include "OpenDriveEditorLane.h"
#include "OpenDriveSolver.h"
#include "InteractiveToolManager.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"

#define LOCTEXT_NAMESPACE "OpenDriveVisualizerTool"

UInteractiveTool* UOpenDriveVisualizerToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UOpenDriveVisualizerTool* NewTool = NewObject<UOpenDriveVisualizerTool>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}

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

void UOpenDriveVisualizerTool::Generate(const float Offset, const float Step) const
{
	DeleteAllLanes();
	
	/* OpenDrive solver creation */
	UOpenDriveSolver* Solver = NewObject<UOpenDriveSolver>();
	FActorSpawnParameters SpawnParam;
	SpawnParam.bHideFromSceneOutliner = true;

	/* Draw roads */
	for (TArray<UOpenDriveSolver::LaneRef> LaneList = Solver->GetAllLanesOfType(/* All */);
		const auto [Road, LaneSection, Lane] : LaneList)
	{
		AOpenDriveEditorLane* NewLane = TargetWorld->SpawnActor<AOpenDriveEditorLane>(SpawnParam);
		NewLane->Initialize(Road, LaneSection, Lane, Offset, Step);
	}
}

void UOpenDriveVisualizerTool::HideShowLanes() const
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(TargetWorld, AOpenDriveEditorLane::StaticClass(), Actors);

	for (AActor* Actor : Actors)
	{
		Actor->SetIsTemporarilyHiddenInEditor(!Actor->IsHiddenEd());
	}
}

void UOpenDriveVisualizerTool::DeleteAllLanes() const
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(TargetWorld, AOpenDriveEditorLane::StaticClass(), Actors);
	
	for (AActor* Actor : Actors)
	{
		Actor->Destroy();
	}

	Actors.SetNum(0);
}

void UOpenDriveVisualizerTool::OnActorSelected(UObject* SelectedActor) const
{
	if (const AOpenDriveEditorLane* SelectedRoad = Cast<AOpenDriveEditorLane>(SelectedActor); IsValid(SelectedRoad))
	{
		Properties.Get()->RoadId = SelectedRoad->GetRoadId();
		Properties.Get()->JunctionId = SelectedRoad->GetJunctionId();
		Properties.Get()->LaneId = SelectedRoad->GetLaneId();
		Properties.Get()->LaneType = SelectedRoad->GetLaneType();
		Properties.Get()->SuccessorId = SelectedRoad->GetSuccessorId();
		Properties.Get()->PredecessorId = SelectedRoad->GetPredecessorId();
	}
}

#undef LOCTEXT_NAMESPACE
