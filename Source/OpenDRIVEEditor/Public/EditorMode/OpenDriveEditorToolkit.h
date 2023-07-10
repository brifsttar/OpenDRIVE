#pragma once 
#include "Toolkits/BaseToolkit.h"
#include "Public/EditorMode/OpenDriveEditorMode.h"
#include "Public/EditorMode/SOpenDriveEditorModeWidget.h"

class FOpenDRIVEEditorModeToolkit : public FModeToolkit
{
public : 

	FOpenDRIVEEditorModeToolkit()
	{
		SAssignNew(OpenDriveEditorModeWidget, SOpenDRIVEEditorModeWidget);
	}

	// IToolkit interface 
	virtual FName GetToolkitFName() const override { return FName("OpenDRIVEEditorMode"); }
	virtual FText GetBaseToolkitName() const override { return NSLOCTEXT("BuilderModeToolkit", "DisplayName", "Builder"); }
	virtual class FEdMode* GetEditorMode() const override { return GLevelEditorModeTools().GetActiveMode(FOpenDRIVEEditorMode::EM_RoadMode); }
	virtual TSharedPtr<class SWidget> GetInlineContent() const override { return OpenDriveEditorModeWidget; }

private :

	TSharedPtr<SOpenDRIVEEditorModeWidget> OpenDriveEditorModeWidget;

};