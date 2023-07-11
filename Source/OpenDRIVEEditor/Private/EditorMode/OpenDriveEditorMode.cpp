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

		// If if we already selected an OpenDRIVE asset, send it to the widget to update its render 
		// (so it doesn't show "none" instead of the actual selected asset)
		if (IsValid(OpenDRIVEAsset))
		{
			FAssetData openDriveAssetData(OpenDRIVEAsset);

			TSharedPtr<FOpenDRIVEEditorModeToolkit> openDRIVEEdToolkit = StaticCastSharedPtr<FOpenDRIVEEditorModeToolkit>(Toolkit);

			if (openDRIVEEdToolkit.IsValid())
			{
				TSharedPtr<SOpenDRIVEEditorModeWidget> openDRIVEEdWidget = StaticCastSharedPtr<SOpenDRIVEEditorModeWidget>(openDRIVEEdToolkit->GetInlineContent());

				if (openDRIVEEdWidget.IsValid())
				{
					openDRIVEEdWidget->SetAssetData(openDriveAssetData);
				}
			}
		}
	}

	if (bHasBeenLoaded == false)
	{
		LoadRoads();
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
	
	// When we load a new map, it will call the function Exit(). 
	// The issue here to not hide the actors bellow. 
	// (because they doesn't exist in the new opened level, so it will just crash when it will try to set their visibility
	if (bIsMapOpening == false)
	{
		SetRoadsVisibilityInEditor(true);
	}

	FEdMode::Exit();
}

void FOpenDRIVEEditorMode::Reset()
{
	for (AOpenDriveRoadEd* road : FRoadsArray)
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

void FOpenDRIVEEditorMode::SetOpenDRIVEAsset(UOpenDriveAsset* newAsset_)
{
	OpenDRIVEAsset = newAsset_;
}

void FOpenDRIVEEditorMode::LoadRoads()
{
	// reset old roads pointers after a change of level
	if (FRoadsArray.IsEmpty() == false)
	{
		for (AOpenDriveRoadEd* roads : FRoadsArray)
		{
			FRoadsArray.Remove(roads);
		}
		FRoadsArray.Empty();
	}

	// roadmanager params
	roadmanager::OpenDrive* Odr = roadmanager::Position::GetOpenDrive();
	roadmanager::Road* road = 0;
	roadmanager::LaneSection* laneSection = 0;
	roadmanager::Lane* lane = 0;
	size_t nrOfRoads = Odr->GetNumOfRoads();
	double laneLength = 0.;

	// Actor spawn params
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
				newRoad->Initialize(road->GetId(), road->GetJunction(), laneSection, lane, _roadOffset);
				FRoadsArray.Add(newRoad);
			}
		}
	}
	bHasBeenLoaded = true;
}

void FOpenDRIVEEditorMode::SetRoadsVisibilityInEditor(bool bIsVisible)
{
	for (AOpenDriveRoadEd* road : FRoadsArray)
	{
		road->SetIsTemporarilyHiddenInEditor(bIsVisible);
	}
}

void FOpenDRIVEEditorMode::OnActorSelected(UObject* selectedObject)
{
	AOpenDriveRoadEd* selectedRoad = Cast<AOpenDriveRoadEd>(selectedObject);

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
