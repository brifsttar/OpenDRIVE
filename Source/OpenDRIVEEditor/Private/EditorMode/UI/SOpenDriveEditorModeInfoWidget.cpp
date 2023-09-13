#include "Public/EditorMode/UI/SOpenDriveEditorModeInfoWidget.h"
#include "Public/EditorMode/SOpenDriveEditorModeWidget.h"
#include "Public/OpenDriveEditor.h"
#include "Public/EditorMode/OpenDriveEditorMode.h"

void SOpenDRIVEEditorInfoWidget::Construct(const FArguments& InArgs)
{
	_fontInfoPtr = MakeShareable(new FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Light.ttf"), 12));

	_roadIdTextPtr = SNew(STextBlock)
		.Text(FText::FromString(TEXT("RoadId : ")))
		.Font(*_fontInfoPtr)
		.ToolTipText(FText::FromString(TEXT("The selected road's Id.")));

	_junctionIdTextPtr = SNew(STextBlock)
		.Text(FText::FromString(TEXT("JunctionId : ")))
		.Font(*_fontInfoPtr)
		.ToolTipText(FText::FromString(TEXT("The selected road's junction's Id.")));

	_laneTypeTextPtr = SNew(STextBlock)
		.Text(FText::FromString(TEXT("Lane type : ")))
		.Font(*_fontInfoPtr)
		.ToolTipText(FText::FromString(TEXT("The selected lane's type.")));

	_laneIdTextPtr = SNew(STextBlock)
		.Text(FText::FromString(TEXT("LaneId : ")))
		.Font(*_fontInfoPtr)
		.ToolTipText(FText::FromString(TEXT("The selected lane's Id.")));

	_successorIdTextPtr = SNew(STextBlock)
		.Text(FText::FromString(TEXT("Successor Id : ")))
		.Font(*_fontInfoPtr)
		.ToolTipText(FText::FromString(TEXT("The road's successorId.")));

	_predecessorIdTextPtr = SNew(STextBlock)
		.Text(FText::FromString(TEXT("Predecessor Id : ")))
		.Font(*_fontInfoPtr)
		.ToolTipText(FText::FromString(TEXT("The road's predecessorId.")));

	ChildSlot
		[
			SNew(SBorder)
				[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(10.f, 10.f, 10.f, 0.f)
				[
					SNew(SBorder)
						[
							SNew(SVerticalBox)
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(10.f, 10.f, 0.f, 0.f)
							[
								_roadIdTextPtr.ToSharedRef()
							]
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(10.f, 5.f, 0.f, 0.f)
							[
								_successorIdTextPtr.ToSharedRef()
							]
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(10.f, 5.f, 0.f, 0.f)
							[
								_predecessorIdTextPtr.ToSharedRef()
							]
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(10.f, 5.f, 0.f, 10.f)
							[
								_junctionIdTextPtr.ToSharedRef()
							]
						]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(10.f, 20.f, 10.f, 10.f)
				[
					SNew(SBorder)
						[
							SNew(SVerticalBox)
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(10.f, 10.f, 0.f, 0.f)
							[
								_laneIdTextPtr.ToSharedRef()
							]

							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(10.f, 5.f, 0.f, 10.f)
							[
								_laneTypeTextPtr.ToSharedRef()
							]
						]
				]
			]
		];
}

void SOpenDRIVEEditorInfoWidget::UpadateInfo(AOpenDriveEditorLane* lane_)
{
	_roadIdTextPtr.Get()->SetText(FText::FromString("Road Id : " + FString::FromInt(lane_->GetRoadId())));

	_junctionIdTextPtr.Get()->SetText(FText::FromString("Junction Id : " + FString::FromInt(lane_->GetJunctionId())));

	_laneTypeTextPtr.Get()->SetText(FText::FromString("Lane type : " + lane_->GetLaneType()));

	_laneIdTextPtr.Get()->SetText(FText::FromString("Lane Id : " + FString::FromInt(lane_->GetLaneId())));

	_successorIdTextPtr.Get()->SetText(FText::FromString("Successor Id : " + FString::FromInt(lane_->GetSuccessorId())));

	_predecessorIdTextPtr.Get()->SetText(FText::FromString("Predecessor Id : " + FString::FromInt(lane_->GetPredecessorId())));
}
