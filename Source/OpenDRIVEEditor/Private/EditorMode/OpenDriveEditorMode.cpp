#include "EditorMode/OpenDriveEditorMode.h"
#include "OpenDriveEditor.h"
#include "Toolkits/ToolkitManager.h"
#include "ScopedTransaction.h"
#include "EditorMode/OpenDriveEditorToolkit.h"
#include "RoadManager.hpp"
#include <NavigationSystem.h>
#include "Kismet/GameplayStatics.h"
#include <OpenDriveSolver.h>

const FEditorModeID FOpenDRIVEEditorMode::EM_RoadMode(TEXT("EM_RoadMode"));

FOpenDRIVEEditorMode::FOpenDRIVEEditorMode()
{
	UE_LOG(LogClass, Warning, TEXT("Custom editor mode constructor called"));

	FEdMode::FEdMode();
	MapOpenedDelegateHandle = FEditorDelegates::MapChange.AddRaw(this, &FOpenDRIVEEditorMode::OnMapOpenedCallback);
	OnActorSelectedHandle = USelection::SelectObjectEvent.AddRaw(this, &FOpenDRIVEEditorMode::OnActorSelected);
}

void FOpenDRIVEEditorMode::Enter()
{
	FEdMode::Enter();

	bIsMapOpening = false;

	if (!Toolkit.IsValid())
	{
		Toolkit = MakeShareable(new FOpenDRIVEEditorModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}
	
	if (bHasBeenLoaded == false	&& (GEditor->IsSimulateInEditorInProgress() == false && GEditor->IsPlaySessionInProgress() == false))
	{
		LoadRoadsNetwork();
	}
	else
	{
		SetRoadsVisibilityInEditor(false);
	}
}

void FOpenDRIVEEditorMode::Exit()
{
	FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
	Toolkit.Reset();
	
	if (bIsMapOpening == false) //prevents the function's call in case of level change 
	{
		SetRoadsVisibilityInEditor(true);
		SetRoadsArrowsVisibilityInEditor(false);
	}

	FEdMode::Exit();
}

void FOpenDRIVEEditorMode::ResetRoadsArray()
{
	for (auto road : roadsArray)
	{
		if (IsValid(road) == true)
		{
			road->Destroy();
		}
	}
	roadsArray.Reset();
	bHasBeenLoaded = false;
}

void FOpenDRIVEEditorMode::ResetNavMeshArray()
{
	ARoadBorderLane.Reset();
}

void FOpenDRIVEEditorMode::Generate()
{
	LoadRoadsNetwork();
}

FOpenDRIVEEditorMode::~FOpenDRIVEEditorMode()
{
	FEditorDelegates::OnMapOpened.Remove(MapOpenedDelegateHandle);
	USelection::SelectObjectEvent.Remove(OnActorSelectedHandle);
}

void FOpenDRIVEEditorMode::OnMapOpenedCallback(uint32 type)
{
	if (type == MapChangeEventFlags::NewMap)
	{
		UE_LOG(LogClass, Warning, TEXT("a new map has been opened"));

		roadsArray.Reset();
		bIsMapOpening = true;
		bHasBeenLoaded = false;
	}
}

void FOpenDRIVEEditorMode::LoadRoadsNetwork()
{
	// Actor spawn params
	FActorSpawnParameters spawnParam;
	spawnParam.bHideFromSceneOutliner = true;
	spawnParam.bTemporaryEditorActor = true;
	spawnParam.ObjectFlags = EObjectFlags::RF_Transient;

	// empty the array if needed

	if (roadsArray.IsEmpty() == false)
	{
		ResetRoadsArray();
	}

	//Extract all the road from the opendrive file
	UOpenDriveSolver* Solver = NewObject<UOpenDriveSolver>();
	TArray<UOpenDriveSolver::LaneRef> laneList = Solver->GetAllLanesOfType(/*All*/);

	//Draw them
	for(UOpenDriveSolver::LaneRef lane : laneList ) {
		AOpenDriveEditorLane* newRoad = GetWorld()->SpawnActor<AOpenDriveEditorLane>(FVector::ZeroVector, FRotator::ZeroRotator, spawnParam);
		newRoad->SetActorHiddenInGame(true);
		newRoad->Initialize(lane.road, lane.laneSection, lane.lane, _roadOffset, _step);
		roadsArray.Add(newRoad);
	}
	bHasBeenLoaded = true;
}

void FOpenDRIVEEditorMode::SetRoadsVisibilityInEditor(bool bIsVisible)
{
	if (roadsArray.IsEmpty() == false)
	{
		for (AOpenDriveEditorLane* road : roadsArray)
		{
			road->SetIsTemporarilyHiddenInEditor(bIsVisible);
		}
	}
}

void FOpenDRIVEEditorMode::SetRoadsArrowsVisibilityInEditor(bool bIsVisible)
{
	if (roadsArray.IsEmpty() == false)
	{
		for (AOpenDriveEditorLane* road : roadsArray)
		{
			road->SetArrowVisibility(bIsVisible);
		}
	}
}

void FOpenDRIVEEditorMode::OnActorSelected(UObject* selectedObject)
{
	AOpenDriveEditorLane* selectedRoad = Cast<AOpenDriveEditorLane>(selectedObject);

	if (IsValid(selectedRoad) == true)
	{
		UE_LOG(LogClass, Warning, TEXT("road selected"));
	
		TSharedPtr<FOpenDRIVEEditorModeToolkit> openDRIVEEdToolkit = StaticCastSharedPtr<FOpenDRIVEEditorModeToolkit>(Toolkit);

		if (openDRIVEEdToolkit.IsValid())
		{
			TSharedPtr<SOpenDRIVEEditorModeWidget> openDRIVEEdWidget = StaticCastSharedPtr<SOpenDRIVEEditorModeWidget>(openDRIVEEdToolkit->GetInlineContent());

			if (openDRIVEEdWidget.IsValid())
			{
				openDRIVEEdWidget->UpdateLaneInfo(selectedRoad);
			}
		}
	}
}

void FOpenDRIVEEditorMode::OutlinerFolder_Clear(FString folder) {
	
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		AActor* Actor = *ActorItr;
		if (Actor && Actor->GetFolderPath().ToString() == folder) {
			Actor->Destroy();
		}
	}
}

TArray<AActor*> FOpenDRIVEEditorMode::OutlinerFolder_GetAll(FString folder) {
	TArray<AActor*> actorList;
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		AActor* Actor = *ActorItr;
		if (Actor && Actor->GetFolderPath().ToString() == folder) {
			actorList.Add(Actor);
		}
	}
	return actorList;
}
