#pragma once 
#include "Framework/Application/SlateApplication.h"

class SOpenDRIVEEditorModeWidget : public SCompoundWidget
{
public :

	SLATE_BEGIN_ARGS(SOpenDRIVEEditorModeWidget) {}
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs);

	/**
	 * Returns the Editor mode 
	 * @return The OpenDRIVE Editor Mode
	 */
	class FOpenDRIVEEditorMode* GetEdMode() const;

	//Link between the buttons and the editor mode functions
	FReply Generate();
	FReply Reset();

	/**
	 * Gets if roads are loaded or not
	 * @return True if loaded, False if not
	 */
	bool IsLoaded() const;

	/**
	 * Gets if editor is in simulation mode or not
	 * @return True if simulating, False if not
	 */
	bool CheckIfSimulating() const;

	//Lane informations text blocks
	TSharedPtr<STextBlock> RoadIdTextPtr;
	TSharedPtr<STextBlock> JunctionIdTextPtr;
	TSharedPtr<STextBlock> LaneIdTextPtr;
	TSharedPtr<STextBlock> LaneTypeTextPtr;

	/**
	 * Displays the last selected lane's informations
	 * @param lane The lane
	 */
	void UpdateLaneInfo(AOpenDriveRoadEd* lane_);

	/**
	 * Sets the road offset 
	 * @param newOffset The new offset
	 */
	void SetOffset(const FText &newOffset);
};