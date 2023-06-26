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

	if (hasBeenLoaded == false)
	{
		LoadRoads();
	}
	else if(Roads != nullptr)
	{
		Roads->SetIsTemporarilyHiddenInEditor(false);
	}
}

void FOpenDRIVEEditorMode::Exit()
{
	FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
	Toolkit.Reset();
	
	if (Roads != nullptr)
	{
		Roads->SetIsTemporarilyHiddenInEditor(true);
	}

	FEdMode::Exit();
}

void FOpenDRIVEEditorMode::Reset()
{
	if (Roads != nullptr)
	{
		Roads->Destroy();
	}
}

void FOpenDRIVEEditorMode::Generate()
{
	if (hasBeenLoaded == true)
	{
		Reset();
		LoadRoads();
	}
}

void FOpenDRIVEEditorMode::LoadRoads()
{
	AOpenDriveRoadEd* newRoad = GetWorld()->SpawnActor<AOpenDriveRoadEd>(FVector::ZeroVector, FRotator::ZeroRotator);
	newRoad->Initialize(50.f);
	Roads = newRoad;
	hasBeenLoaded = true;
}
