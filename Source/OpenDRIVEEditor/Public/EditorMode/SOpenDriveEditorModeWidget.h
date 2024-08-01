#pragma once 
#include "OpenDriveAsset.h"
#include "PropertyCustomizationHelpers.h"
#include "Framework/Application/SlateApplication.h"

class SOpenDriveEditorModeWidget : public SCompoundWidget
{
public :

	SLATE_BEGIN_ARGS(SOpenDriveEditorModeWidget) {}
	SLATE_END_ARGS();

	/**
	 * Constructs the window
	 */
	void Construct(const FArguments& InArgs);
};
