#include "Public/EditorMode/SOpenDriveEditorModeWidget.h"
#include "Public/OpenDriveEditor.h"
#include "Public/EditorMode/OpenDriveEditorMode.h"

void SOpenDRIVEEditorModeWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
		.VAlign(VAlign_Top)
		.Padding(5.f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("This is a editor mode example.")))
		]
		];
}


FOpenDRIVEEditorMode* SOpenDRIVEEditorModeWidget::GetEdMode() const
{
	return (FOpenDRIVEEditorMode*)GLevelEditorModeTools().GetActiveMode(FOpenDRIVEEditorMode::EM_RoadMode);
}

