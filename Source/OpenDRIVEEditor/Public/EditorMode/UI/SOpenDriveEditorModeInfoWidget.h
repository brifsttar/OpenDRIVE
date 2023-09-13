#pragma once
#include "PropertyCustomizationHelpers.h"
#include "Framework/Application/SlateApplication.h"

class SOpenDRIVEEditorInfoWidget : public SCompoundWidget
{
public : 

	SLATE_BEGIN_ARGS(SOpenDRIVEEditorInfoWidget) {}
	SLATE_END_ARGS();

	/**
	* Constructs the lane info boxes 
	*/
	void Construct(const FArguments& InArgs);

	/**
	* Called to update information with currently selected lane
	* @param lane_ the selected lane
	*/
	void UpadateInfo(AOpenDriveEditorLane* lane_);

private :

	//Text font
	TSharedPtr<FSlateFontInfo> _fontInfoPtr;

	//Lane informations text blocks
	TSharedPtr<STextBlock> _roadIdTextPtr;
	TSharedPtr<STextBlock> _junctionIdTextPtr;
	TSharedPtr<STextBlock> _laneIdTextPtr;
	TSharedPtr<STextBlock> _laneTypeTextPtr;
	TSharedPtr<STextBlock> _successorIdTextPtr;
	TSharedPtr<STextBlock> _predecessorIdTextPtr;
};