#pragma once 
#include "EditorModes.h"
#include "EdMode.h"


class FOpenDRIVEEditorMode : public FEdMode
{
public :
	const static FEditorModeID EM_RoadMode;

	virtual void Enter() override;
	virtual void Exit() override;
};