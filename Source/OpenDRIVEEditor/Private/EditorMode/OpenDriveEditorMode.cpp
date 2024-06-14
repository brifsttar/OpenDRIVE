#include "Public/EditorMode/OpenDriveEditorMode.h"
#include "Public/OpenDriveEditor.h"
#include "Toolkits/ToolkitManager.h"
#include "ScopedTransaction.h"
#include "Public/EditorMode/OpenDriveEditorToolkit.h"
#include "RoadManager.hpp"
#include <NavigationSystem.h>
#include "Kismet/GameplayStatics.h"
#include <OpenDriveEditorNavMeshModifier.h>
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
	UE_LOG(LogClass, Warning, TEXT("Enter"));

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

TSubclassOf<UNavArea> FOpenDRIVEEditorMode::getNavArea(roadmanager::Lane::LaneType laneType){
	return LoadClass<UNavArea>(nullptr, TEXT("//D:/git/vhcd/Content/VHCD/Test/Forbidden"));

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

void FOpenDRIVEEditorMode::CreateNavemeshObject()
{
	//Get the manager in the scene
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "NavMeshManager", OutActors);
	AActor* NavMeshManager = nullptr;
	UFunction* GenerateFromOpenDrive = nullptr;
	for (AActor* Actor : OutActors)
	{
		GenerateFromOpenDrive = Actor->GetClass()->FindFunctionByName(FName("GenerateFromOpenDrive"));
		if (GenerateFromOpenDrive == nullptr)
		{
			continue;
		}
		NavMeshManager = Actor;
		break;
	}



	// Actor spawn params
	FActorSpawnParameters spawnParam;
	spawnParam.bHideFromSceneOutliner = false;
	spawnParam.bTemporaryEditorActor = true;
	TArray<ULevel*> levels = GetWorld()->GetLevels();
	spawnParam.OverrideLevel = levels[0];

	OutlinerFolder_Clear("/NavMeshObject");

	UOpenDriveSolver* Solver = NewObject<UOpenDriveSolver>();
	//TArray<UOpenDriveSolver::LaneRef> laneList = Solver->GetAllLanesOfType(roadmanager::Lane::LANE_TYPE_BORDER);
	TArray<UOpenDriveSolver::LaneRef> laneList = Solver->GetAllLanesOfType();
	for (UOpenDriveSolver::LaneRef lane : laneList) {

		AOpenDriveEditorNavMeshModifier* newRoad = GetWorld()->SpawnActor<AOpenDriveEditorNavMeshModifier>(FVector::ZeroVector, FRotator::ZeroRotator, spawnParam);
		newRoad->Initialize(lane.road, lane.laneSection, lane.lane, _roadOffset, _step);
		newRoad->CreateSpline();
		TSubclassOf<UNavArea> area = getNavArea(lane.lane->GetLaneType());
		newRoad->SetNavType(area);
		newRoad->SetFolderPath("/NavMeshObject");
		newRoad->Rebuild();

		OutActors.Add(newRoad);
	}

	/*
	struct FDynamicArgs
	{
		TArray<FTransform> Arg01;
		TSubclassOf<UNavArea> NewAreaClass;
	};
	FDynamicArgs Arg;
	
	
	// Actor spawn params
	FActorSpawnParameters spawnParam;
	spawnParam.bHideFromSceneOutliner = false;
	//Call the manager to spawn all the line
	UOpenDriveSolver* Solver = NewObject<UOpenDriveSolver>();




	TArray<UOpenDriveSolver::RoadData> A_road = Solver->getAllRoad(roadmanager::Lane::LANE_TYPE_SHOULDER);
	
	if (NavMeshManager)
	Arg.Arg01 = Solver->extractRoadTransform(A_road);

	NavMeshManager->ProcessEvent(GenerateFromOpenDrive, &Arg);

	bHasBeenLoaded = true;
	*/

}
