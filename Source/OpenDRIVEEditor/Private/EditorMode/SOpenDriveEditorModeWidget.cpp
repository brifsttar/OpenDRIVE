#include "Public/EditorMode/SOpenDriveEditorModeWidget.h"
#include "Public/OpenDriveEditor.h"
#include "Public/EditorMode/OpenDriveEditorMode.h"
#include "Public/EditorMode/SOpenDriveEditorModeWidget.h"

void SOpenDRIVEEditorModeWidget::Construct(const FArguments& InArgs)
{
	_infoWidget = SNew(SOpenDRIVEEditorInfoWidget);

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.f, 30.f, 0.f, 0.f)
		[
			SNew(SOpenDRIVEEditorParamWidget)
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(20.f, 30.f, 20.f, 0.f)
		[
			_infoWidget.ToSharedRef()
		]
	];
}

FOpenDRIVEEditorMode* SOpenDRIVEEditorModeWidget::GetEdMode() const
{
	return (FOpenDRIVEEditorMode*)GLevelEditorModeTools().GetActiveMode(FOpenDRIVEEditorMode::EM_RoadMode);
}

void SOpenDRIVEEditorModeWidget::UpdateLaneInfo(AOpenDriveEditorLane* lane_)
{
	_infoWidget->UpadateInfo(lane_);
}

