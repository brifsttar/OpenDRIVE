#include "Public/EditorMode/SOpenDriveEditorModeWidget.h"
#include "Public/OpenDriveEditor.h"
#include "Public/EditorMode/OpenDriveEditorMode.h"
#include "Public/EditorMode/SOpenDriveEditorModeWidget.h"

void SOpenDRIVEEditorModeWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			.VAlign(VAlign_Top)
			.Padding(5.f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.f,5.f,0.f,0.f)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("OpenDRIVE Editor Mode")))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.f,5.f,0.f,0.f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(2,0,0,0)
					.VAlign(VAlign_Center)
					[
						SNew(SButton)
						.Text(FText::FromString("Reset"))
						.OnClicked(this, &SOpenDRIVEEditorModeWidget::Reset)
						.IsEnabled(this, &SOpenDRIVEEditorModeWidget::IsLoaded)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(0, 0, 2, 0)
					.VAlign(VAlign_Center)
					[
						SNew(SButton)
						.Text(FText::FromString("Generate"))
						.OnClicked(this, &SOpenDRIVEEditorModeWidget::Generate)
					]
				]
			]
		];
}


FOpenDRIVEEditorMode* SOpenDRIVEEditorModeWidget::GetEdMode() const
{
	return (FOpenDRIVEEditorMode*)GLevelEditorModeTools().GetActiveMode(FOpenDRIVEEditorMode::EM_RoadMode);
}

FReply SOpenDRIVEEditorModeWidget::Reset()
{
	GetEdMode()->Reset();
	return FReply::Handled();
}

bool SOpenDRIVEEditorModeWidget::IsLoaded() const
{
	return GetEdMode()->GetHasBeenLoaded();
}

FReply SOpenDRIVEEditorModeWidget::Generate()
{
	GetEdMode()->Generate();
	return FReply::Handled();
}

