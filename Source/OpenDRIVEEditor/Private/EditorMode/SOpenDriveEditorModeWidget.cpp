#include "EditorMode/SOpenDriveEditorModeWidget.h"
#include "OpenDriveEditor.h"
#include "EditorMode/OpenDriveEditorMode.h"

void SOpenDriveEditorModeWidget::Construct(const FArguments& InArgs)
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
