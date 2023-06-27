#include "Public/EditorMode/OpenDriveEditorMode.h"
#include "Public/OpenDriveEditor.h"
#include "Toolkits/ToolkitManager.h"
#include "ScopedTransaction.h"
#include "Public/EditorMode/OpenDriveEditorToolkit.h"
#include "RoadManager.hpp"

const FEditorModeID FOpenDRIVEEditorMode::EM_RoadMode(TEXT("EM_RoadMode"));

void FOpenDRIVEEditorMode::Enter()
{
	FEdMode::Enter();

	if (!Toolkit.IsValid())
	{
		Toolkit = MakeShareable(new FOpenDRIVEEditorModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}

	LoadRoads();
}

void FOpenDRIVEEditorMode::Exit()
{
	FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
	Toolkit.Reset();
	Reset();
	FEdMode::Exit();
}

void FOpenDRIVEEditorMode::Reset()
{
	for (AOpenDriveRoadEd* road : Roads)
	{
		if (road != nullptr)
		{
			Roads.Remove(road);
		}
	}
	hasBeenLoaded = false;
}

void FOpenDRIVEEditorMode::Generate()
{
	if (hasBeenLoaded == true)
	{
		Reset();
		LoadRoads();
	}
	else
	{
		LoadRoads();
	}
}

void FOpenDRIVEEditorMode::LoadRoads()
{
	roadmanager::OpenDrive* Odr = roadmanager::Position::GetOpenDrive();
	roadmanager::Road* road = 0;
	size_t nrOfRoads = Odr->GetNumOfRoads();

	for (int i = 0; i < (int)nrOfRoads; i++)
	{
		road = Odr->GetRoadByIdx(i);
		if (!road) continue;

		AOpenDriveRoadEd* newRoad = GetWorld()->SpawnActor<AOpenDriveRoadEd>(FVector::ZeroVector, FRotator::ZeroRotator);
		newRoad->SetFolderPath("/EditorMode");
		newRoad->SetActorLabel(FString("Roads ") + FString::FromInt(i));
		newRoad->Initialize(road, 50.f);
		Roads.Add(newRoad);
	}
	hasBeenLoaded = true;
}
