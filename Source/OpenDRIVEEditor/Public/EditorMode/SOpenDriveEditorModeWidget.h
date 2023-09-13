#pragma once 
#include "../../../OpenDRIVE/Public/OpenDriveAsset.h"
#include "PropertyCustomizationHelpers.h"
#include "Framework/Application/SlateApplication.h"
#include "UI/SOpenDriveEditorModeInfoWidget.h"
#include "UI/SOpenDriveEditorModeParamWidget.h"

class SOpenDRIVEEditorModeWidget : public SCompoundWidget
{
public :

	SLATE_BEGIN_ARGS(SOpenDRIVEEditorModeWidget) {}
	SLATE_END_ARGS();

	/**
	 * Constructs the widget
	 */
	void Construct(const FArguments& InArgs);

	/**
	 * Returns the Editor mode 
	 * @return The OpenDRIVE Editor Mode
	 */
	class FOpenDRIVEEditorMode* GetEdMode() const;

	/**
	 * Displays the last selected lane's informations
	 * @param lane The lane
	 */
	void UpdateLaneInfo(AOpenDriveEditorLane* lane_);

private : 

	//Info widget
	TSharedPtr<SOpenDRIVEEditorInfoWidget> _infoWidget;
};
