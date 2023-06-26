#pragma once 
#include "EditorModes.h"
#include "EdMode.h"
#include "../OpenDriveRoadEd.h"

class FOpenDRIVEEditorMode : public FEdMode
{
public :
	const static FEditorModeID EM_RoadMode;

	virtual void Enter() override;
	virtual void Exit() override;

	inline bool GetHasBeenLoaded() const { return hasBeenLoaded; };

	void Reset();

	void Generate();

protected :

	bool hasBeenLoaded = false;

	void LoadRoads();
	
	AOpenDriveRoadEd* Roads;

};