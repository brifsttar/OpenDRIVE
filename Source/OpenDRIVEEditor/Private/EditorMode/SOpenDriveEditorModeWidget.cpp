#include "Public/EditorMode/SOpenDriveEditorModeWidget.h"
#include "Public/OpenDriveEditor.h"
#include "Public/EditorMode/OpenDriveEditorMode.h"
#include "Public/EditorMode/SOpenDriveEditorModeWidget.h"

void SOpenDRIVEEditorModeWidget::Construct(const FArguments& InArgs)
{
	_fontInfoPtr = MakeShareable(new FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Light.ttf"), 12));

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.f, 30.f, 0.f, 0.f)
		[
			ConstructButtons(InArgs)
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(20.f, 30.f, 20.f, 0.f)
		[
			ConstructRoadGenerationParameters(InArgs)
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(20.f, 30.f, 20.f, 0.f)
		[
			ConstructLaneInfoBox(InArgs)
		]
	];
}

TSharedRef<SBorder> SOpenDRIVEEditorModeWidget::ConstructLaneInfoBox(const FArguments& InArgs)
{
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

	TSharedRef<SBorder> border =
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

		];
	return border;
}

TSharedRef<SHorizontalBox> SOpenDRIVEEditorModeWidget::ConstructButtons(const FArguments& InArgs)
{
	TSharedPtr<SButton> resetButton = SNew(SButton).Text(FText::FromString("Reset"))
		.OnClicked(this, &SOpenDRIVEEditorModeWidget::Reset).IsEnabled(this, &SOpenDRIVEEditorModeWidget::IsLoaded)
		.ToolTipText(FText::FromString(TEXT("Resets currently drawn roads.")));

	StaticCast<STextBlock&>(resetButton.ToSharedRef().Get().GetContent().Get()).SetJustification(ETextJustify::Center);

	TSharedPtr<SButton> generateButton = SNew(SButton).Text(FText::FromString("Generate"))
		.OnClicked(this, &SOpenDRIVEEditorModeWidget::Generate).IsEnabled(this, &SOpenDRIVEEditorModeWidget::CheckIfSimulating)
		.ToolTipText(FText::FromString(TEXT("Draws roads (will reset currently drawn roads).")));

	StaticCast<STextBlock&>(generateButton.ToSharedRef().Get().GetContent().Get()).SetJustification(ETextJustify::Center);

	TSharedRef<SHorizontalBox> horBox = 
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().Padding(20, 0, 0, 0).FillWidth(0.5f)
		[
			resetButton.ToSharedRef()
		]
		+ SHorizontalBox::Slot().Padding(10, 0, 20, 0).FillWidth(0.5f)
		[
			generateButton.ToSharedRef()
		];

	return horBox;
}

TSharedRef<SBorder> SOpenDRIVEEditorModeWidget::ConstructRoadGenerationParameters(const FArguments& InArgs)
{
	_offsetTextPtr = SNew(STextBlock).Justification(ETextJustify::Center)
		.Text(FText::FromString("ZOffset : " + FString::FormatAsNumber(GetEdMode()->GetRoadOffset())))
		.Font(*_fontInfoPtr)
		.ToolTipText(FText::FromString(TEXT("Roads' ZOffset : by defaults, the road's network will be drawn at Z=20.\nSo if you already have static meshes to represent your roads, higher that value to avoid texture flickering.")));

	_stepTextPtr = SNew(STextBlock).Justification(ETextJustify::Center)
		.Text(FText::FromString("Step : " + FString::FormatAsNumber(GetEdMode()->GetStep())))
		.Font(*_fontInfoPtr)
		.ToolTipText(FText::FromString(TEXT("Lower this value for a more precise draw (and less performances !).")));

	TSharedRef<SSlider> OffsetSlider = SNew(SSlider).MinValue(0.f).MaxValue(80.f)
		.Value(GetEdMode()->GetRoadOffset())
		.OnValueChanged(this, &SOpenDRIVEEditorModeWidget::OnOffsetValueChanged);
	TSharedRef<SSlider> StepSlider = SNew(SSlider).MinValue(1.f).MaxValue(10.f)
		.Value(GetEdMode()->GetStep())
		.OnValueChanged(this, &SOpenDRIVEEditorModeWidget::OnStepValueChanged);

	_showArrowsCheckBox = SNew(SCheckBox)
		.OnCheckStateChanged(this, &SOpenDRIVEEditorModeWidget::OnCheckStateChanged);

	TSharedRef<SBorder> border = SNew(SBorder)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5.f, 10.f, 0.f, 0.f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Show arrows")))
				.Font(*_fontInfoPtr)
				.Justification(ETextJustify::Center)
				.ToolTipText(FText::FromString(TEXT("Tick the checkbox to see the roads' directions.")))
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5.f, 5.f, 0.f, 0.f)
			.HAlign(HAlign_Center)
			[
				_showArrowsCheckBox.ToSharedRef()
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5.f, 10.f, 0.f, 0.f)
			[
				_offsetTextPtr.ToSharedRef()
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5.f, 5.f, 5.f, 0.f)
			[
				OffsetSlider
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5.f, 5.f, 0.f, 0.f)
			[
				_stepTextPtr.ToSharedRef()
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5.f, 5.f, 5.f, 10.f)
			[
				StepSlider
			]
			
		];

	return border;
}

FOpenDRIVEEditorMode* SOpenDRIVEEditorModeWidget::GetEdMode() const
{
	return (FOpenDRIVEEditorMode*)GLevelEditorModeTools().GetActiveMode(FOpenDRIVEEditorMode::EM_RoadMode);
}

FReply SOpenDRIVEEditorModeWidget::Reset()
{
	GetEdMode()->Reset();
	_showArrowsCheckBox.Get()->SetIsChecked(ECheckBoxState::Unchecked);
	return FReply::Handled();
}

bool SOpenDRIVEEditorModeWidget::IsLoaded() const
{
	return (GetEdMode()->GetHasBeenLoaded() && CheckIfSimulating());
}

bool SOpenDRIVEEditorModeWidget::CheckIfSimulating() const
{
	return !(GEditor->IsSimulateInEditorInProgress());
}

FReply SOpenDRIVEEditorModeWidget::Generate()
{
	GetEdMode()->Generate();
	return FReply::Handled();
}

void SOpenDRIVEEditorModeWidget::UpdateLaneInfo(AOpenDriveRoadEd* lane_)
{
	_roadIdTextPtr.Get()->SetText(FText::FromString("Road Id : " + FString::FromInt(lane_->GetRoadId())));

	_junctionIdTextPtr.Get()->SetText(FText::FromString("Junction Id : " + FString::FromInt(lane_->GetJunctionId())));

	_laneTypeTextPtr.Get()->SetText(FText::FromString("Lane type : " + lane_->GetLaneType()));

	_laneIdTextPtr.Get()->SetText(FText::FromString("Lane Id : " + FString::FromInt(lane_->GetLaneId())));

	_successorIdTextPtr.Get()->SetText(FText::FromString("Successor Id : " + FString::FromInt(lane_->GetSuccessorId())));

	_predecessorIdTextPtr.Get()->SetText(FText::FromString("Predecessor Id : " + FString::FromInt(lane_->GetPredecessorId())));
}

void SOpenDRIVEEditorModeWidget::SetOffset(const FText &newOffset_)
{
	FString string = newOffset_.ToString();

	float offset = string.IsNumeric() ? FCString::Atof(*string) : 10.f;

	GetEdMode()->SetRoadOffset(offset);
}

void SOpenDRIVEEditorModeWidget::OnObjectChanged(const FAssetData& assetData_)
{
	UOpenDriveAsset* openDRIVEAsset = Cast<UOpenDriveAsset>(assetData_.GetAsset());

	if (IsValid(openDRIVEAsset))
	{
		_openDRIVEAssetData = assetData_;
	}
}

void SOpenDRIVEEditorModeWidget::OnCheckStateChanged(ECheckBoxState state)
{
	switch (state)
	{
	case(ECheckBoxState::Checked):
		GetEdMode()->SetRoadsArrowsVisibilityInEditor(true);
		break;
	case(ECheckBoxState::Unchecked):
		GetEdMode()->SetRoadsArrowsVisibilityInEditor(false);
		break;
	case(ECheckBoxState::Undetermined):
		GetEdMode()->SetRoadsArrowsVisibilityInEditor(false);
		break;
	default:
		GetEdMode()->SetRoadsArrowsVisibilityInEditor(false);
		break;
	}
}

void SOpenDRIVEEditorModeWidget::OnOffsetValueChanged(float value)
{
	_offsetTextPtr->SetText(FText::FromString("Road's Zoffset : " + FString::FormatAsNumber(value)));
	GetEdMode()->SetRoadOffset(value);
}

void SOpenDRIVEEditorModeWidget::OnStepValueChanged(float value)
{
	_stepTextPtr->SetText(FText::FromString("Step : " + FString::FormatAsNumber(value)));
	GetEdMode()->SetStep(value);
}

