#include "Public/EditorMode/OpenDriveEditorMode.h"
#include "Public/OpenDriveEditor.h"
#include "Toolkits/ToolkitManager.h"
#include "ScopedTransaction.h"
#include "Public/EditorMode/OpenDriveEditorToolkit.h"
#include "RoadManager.hpp"

const FEditorModeID FOpenDRIVEEditorMode::EM_RoadMode(TEXT("EM_RoadMode"));

FOpenDRIVEEditorMode::FOpenDRIVEEditorMode()
{
	FEdMode::FEdMode();

	UE_LOG(LogClass, Warning, TEXT("Custom editor mode constructor called"));
	MapOpenedDelegateHandle = FEditorDelegates::OnMapOpened.AddRaw(this, &FOpenDRIVEEditorMode::OnMapOpenedCallback);

	OnActorSelectedHandle = USelection::SelectObjectEvent.AddRaw(this, &FOpenDRIVEEditorMode::OnActorSelected);
}

void FOpenDRIVEEditorMode::Enter()
{
	FEdMode::Enter();

	bIsMapOpening = false;

	UE_LOG(LogClass, Warning, TEXT("Enter"));

	if (!Toolkit.IsValid())
	{
		Toolkit = MakeShareable(new FOpenDRIVEEditorModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}

	if (bHasBeenLoaded == false)
	{
		LoadRoads();
	}
	else
	{
		SetRoadsVisibility(false);
	}
}

void FOpenDRIVEEditorMode::Exit()
{
	FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
	Toolkit.Reset();
	
	if (bIsMapOpening == false)
	{
		SetRoadsVisibility(true);
	}

	FEdMode::Exit();
}

void FOpenDRIVEEditorMode::Reset()
{
	for (AOpenDriveRoadEd* road : Roads)
	{
		if (IsValid(road))
		{
			road->Destroy();
		}
	}
	bHasBeenLoaded = false;
}

void FOpenDRIVEEditorMode::Generate()
{
	if (bHasBeenLoaded == true)
	{
		Reset();
		LoadRoads();
	}
	else
	{
		LoadRoads();
	}
}

FOpenDRIVEEditorMode::~FOpenDRIVEEditorMode()
{
	FEditorDelegates::OnMapOpened.Remove(MapOpenedDelegateHandle);
	USelection::SelectObjectEvent.Remove(OnActorSelectedHandle);
}

void FOpenDRIVEEditorMode::OnMapOpenedCallback(const FString& MapName, bool bLoadAsTemplate)
{
	UE_LOG(LogClass, Warning, TEXT("a new map has been opened"));
	bIsMapOpening = true;
	bHasBeenLoaded = false;
}

void FOpenDRIVEEditorMode::LoadRoads()
{
	/*reset*/
	if (Roads.IsEmpty() == false)
	{
		for (AOpenDriveRoadEd* roads : Roads)
		{
			Roads.Remove(roads);
		}

		Roads.Empty();
	}

	/*roadmanager params*/
	roadmanager::OpenDrive* Odr = roadmanager::Position::GetOpenDrive();
	roadmanager::Road* road = 0;
	roadmanager::LaneSection* laneSection = 0;
	roadmanager::Lane* lane = 0;
	size_t nrOfRoads = Odr->GetNumOfRoads();
	double laneLength = 0.;

	/*Actor spawn params*/
	FActorSpawnParameters spawnParam;
	spawnParam.bHideFromSceneOutliner = true;
	spawnParam.bTemporaryEditorActor = true;

	for (int i = 0; i < (int)nrOfRoads; i++)
	{
		road = Odr->GetRoadByIdx(i);
		if (!road) continue;
		
		for (int j = 0; j < road->GetNumberOfLaneSections(); j++)
		{
			laneSection = road->GetLaneSectionByIdx(j);

			if (!laneSection) continue;

			laneLength = laneSection->GetLength();

			for (int k = 0; k < laneSection->GetNumberOfLanes(); k++)
			{
				lane = laneSection->GetLaneByIdx(k);

				if (!lane || lane->GetId() == 0) continue;

				AOpenDriveRoadEd* newRoad = GetWorld()->SpawnActor<AOpenDriveRoadEd>(FVector::ZeroVector, FRotator::ZeroRotator, spawnParam);
				newRoad->SetActorHiddenInGame(true);
				newRoad->Initialize(road->GetId(), road->GetJunction(), laneSection, lane);
				Roads.Add(newRoad);
			}
		}
	}
	bHasBeenLoaded = true;
}

void FOpenDRIVEEditorMode::SetRoadsVisibility(bool bIsVisible)
{
	for (AOpenDriveRoadEd* road : Roads)
	{
		road->SetIsTemporarilyHiddenInEditor(bIsVisible);
	}
}

void FOpenDRIVEEditorMode::OnActorSelected(UObject* selectedObject)
{
	AOpenDriveRoadEd* selectedRoad = Cast<AOpenDriveRoadEd>(selectedObject);

	if (IsValid(selectedRoad) == true)
	{
		UE_LOG(LogClass, Warning, TEXT("object selected"));
		onLaneSelected.Broadcast(selectedRoad);
		
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
