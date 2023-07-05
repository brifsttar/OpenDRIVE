#include "Public/EditorMode/SOpenDriveEditorModeWidget.h"
#include "Public/OpenDriveEditor.h"
#include "Public/EditorMode/OpenDriveEditorMode.h"
#include "Public/EditorMode/SOpenDriveEditorModeWidget.h"

void SOpenDRIVEEditorModeWidget::Construct(const FArguments& InArgs)
{
	RoadId = SNew(STextBlock)
		.Text(FText::FromString(TEXT("RoadId : ")));
	
	JunctionId = SNew(STextBlock)
		.Text(FText::FromString(TEXT("JunctionId : ")));

	LaneType = SNew(STextBlock)
		.Text(FText::FromString(TEXT("Lane type : ")));

	LaneId = SNew(STextBlock)
		.Text(FText::FromString(TEXT("LaneId : ")));

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
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.f, 20.f, 0.f, 0.f)
				[
					RoadId.ToSharedRef()
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.f, 5.f, 0.f, 0.f)
				[
					LaneId.ToSharedRef()
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.f, 5.f, 0.f, 0.f)
				[
					JunctionId.ToSharedRef()
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.f, 5.f, 0.f, 0.f)
				[
					LaneType.ToSharedRef()
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

void SOpenDRIVEEditorModeWidget::UpdateLaneInfo(AOpenDriveRoadEd* lane)
{
	RoadId.Get()->SetText(FText::FromString("Road Id : " + FString::FromInt(lane->GetRoadId())));

	JunctionId.Get()->SetText(FText::FromString("Junction Id : " + FString::FromInt(lane->GetJunctionId())));

	LaneType.Get()->SetText(FText::FromString("Lane type : " + lane->GetLaneType()));

	LaneId.Get()->SetText(FText::FromString("Lane Id : " + FString::FromInt(lane->GetLaneId())));
}