#pragma once 
#include "Framework/Application/SlateApplication.h"

class SOpenDRIVEEditorModeWidget : public SCompoundWidget
{
public :

	SLATE_BEGIN_ARGS(SOpenDRIVEEditorModeWidget) {}
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs);

	class FOpenDRIVEEditorMode* GetEdMode() const;

	FReply Reset();
	bool IsLoaded() const;

	FReply Generate();

	/*Lane info display*/
	TSharedPtr<STextBlock> RoadId;
	TSharedPtr<STextBlock> JunctionId;
	TSharedPtr<STextBlock> LaneId;
	TSharedPtr<STextBlock> LaneType;

	void UpdateLaneInfo(AOpenDriveRoadEd* lane);
};