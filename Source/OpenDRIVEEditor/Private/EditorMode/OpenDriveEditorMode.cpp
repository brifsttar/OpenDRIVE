#include "Public/EditorMode/OpenDriveEditorMode.h"
#include "Public/OpenDriveEditor.h"
#include "Toolkits/ToolkitManager.h"
#include "ScopedTransaction.h"
#include "Public/EditorMode/OpenDriveEditorToolkit.h"
#include "RoadManager.hpp"

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

	if (bHasBeenLoaded == false)
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
	}

	FEdMode::Exit();
}

void FOpenDRIVEEditorMode::Reset()
{
	if (FRoadsArray.IsEmpty() == false)
	{
		for (AOpenDriveEditorLane* road : FRoadsArray)
		{
			if (IsValid(road))
			{
				road->Destroy();
			}
		}
		FRoadsArray.Empty();
		bHasBeenLoaded = false;
	}
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

		bIsMapOpening = true;
		bHasBeenLoaded = false;
	}
}

void FOpenDRIVEEditorMode::LoadRoadsNetwork()
{
	// empty the array if needed
	if (FRoadsArray.IsEmpty() == false)
	{
		Reset();
	}

	// roadmanager params
	roadmanager::OpenDrive* Odr = roadmanager::Position::GetOpenDrive();
	roadmanager::Road* road = 0;
	roadmanager::LaneSection* laneSection = 0;
	roadmanager::Lane* lane = 0;
	roadmanager::RoadLink* predecessorLink;
	roadmanager::RoadLink* successorLink;
	size_t nrOfRoads = Odr->GetNumOfRoads();
	
	// Actor spawn params
	FActorSpawnParameters spawnParam;
	spawnParam.bHideFromSceneOutliner = true;
	spawnParam.bTemporaryEditorActor = true;

	for (int i = 0; i < (int)nrOfRoads; i++)
	{
		road = Odr->GetRoadByIdx(i);
		if (!road) continue;

		predecessorLink = road->GetLink(roadmanager::LinkType::PREDECESSOR);
		successorLink = road->GetLink(roadmanager::LinkType::SUCCESSOR);
		int predId = predecessorLink != nullptr ? predecessorLink->GetElementId() : 0;
		int succId = successorLink != nullptr ? successorLink->GetElementId() : 0;

		for (int j = 0; j < road->GetNumberOfLaneSections(); j++)
		{
			laneSection = road->GetLaneSectionByIdx(j);

			if (!laneSection) continue;

			for (int k = 0; k < laneSection->GetNumberOfLanes(); k++)
			{
				lane = laneSection->GetLaneByIdx(k);

				if (!lane || lane->GetId() == 0) continue;

				AOpenDriveEditorLane* newRoad = GetWorld()->SpawnActor<AOpenDriveEditorLane>(FVector::ZeroVector, FRotator::ZeroRotator, spawnParam);
				newRoad->SetActorHiddenInGame(true);
				newRoad->Initialize(road->GetId(), road->GetJunction(), succId, predId, laneSection, lane, _roadOffset, _step);
				FRoadsArray.Add(newRoad);
			}
		}
	}
	bHasBeenLoaded = true;
}

void FOpenDRIVEEditorMode::SetRoadsVisibilityInEditor(bool bIsVisible)
{
	if (FRoadsArray.IsEmpty() == false)
	{
		for (AOpenDriveEditorLane* road : FRoadsArray)
		{
			road->SetIsTemporarilyHiddenInEditor(bIsVisible);
		}
	}
}

void FOpenDRIVEEditorMode::SetRoadsArrowsVisibilityInEditor(bool bIsVisible)
{
	if (FRoadsArray.IsEmpty() == false)
	{
		for (AOpenDriveEditorLane* road : FRoadsArray)
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
