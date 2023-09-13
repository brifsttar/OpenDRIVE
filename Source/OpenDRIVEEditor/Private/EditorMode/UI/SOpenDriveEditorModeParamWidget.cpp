#include "Public/EditorMode/UI/SOpenDriveEditorModeParamWidget.h"
#include "Public/OpenDriveEditor.h"
#include "Public/EditorMode/OpenDriveEditorMode.h"

void SOpenDRIVEEditorParamWidget::Construct(const FArguments& InArgs)
{
	_fontInfoPtr = MakeShareable(new FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Light.ttf"), 12));

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
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
	];
}

TSharedRef<SHorizontalBox> SOpenDRIVEEditorParamWidget::ConstructButtons(const FArguments& InArgs)
{
	TSharedPtr<SButton> resetButton = SNew(SButton).Text(FText::FromString("Reset"))
		.OnClicked(this, &SOpenDRIVEEditorParamWidget::Reset)
		.IsEnabled(this, &SOpenDRIVEEditorParamWidget::IsLoaded)
		.ToolTipText(FText::FromString(TEXT("Resets currently drawn roads.")));

	StaticCast<STextBlock&>(resetButton.ToSharedRef().Get().GetContent().Get()).SetJustification(ETextJustify::Center);

	TSharedPtr<SButton> generateButton = SNew(SButton).Text(FText::FromString("Generate"))
		.OnClicked(this, &SOpenDRIVEEditorParamWidget::Generate)
		.IsEnabled(this, &SOpenDRIVEEditorParamWidget::CheckIfInEditorMode)
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

TSharedRef<SBorder> SOpenDRIVEEditorParamWidget::ConstructRoadGenerationParameters(const FArguments& InArgs)
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
		.OnValueChanged(this, &SOpenDRIVEEditorParamWidget::OnOffsetValueChanged);

	TSharedRef<SSlider> StepSlider = SNew(SSlider).MinValue(1.f).MaxValue(10.f)
		.Value(GetEdMode()->GetStep())
		.OnValueChanged(this, &SOpenDRIVEEditorParamWidget::OnStepValueChanged);

	_showArrowsCheckBox = SNew(SCheckBox)
		.IsEnabled(this, &SOpenDRIVEEditorParamWidget::CheckIfInEditorMode)
		.OnCheckStateChanged(this, &SOpenDRIVEEditorParamWidget::OnCheckStateChanged);

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

bool SOpenDRIVEEditorParamWidget::IsLoaded() const
{
	return (GetEdMode()->GetHasBeenLoaded() && CheckIfInEditorMode());
}

FReply SOpenDRIVEEditorParamWidget::Generate()
{
	GetEdMode()->Generate();
	_showArrowsCheckBox.Get()->SetIsChecked(ECheckBoxState::Unchecked);
	return FReply::Handled();
}

FReply SOpenDRIVEEditorParamWidget::Reset()
{
	GetEdMode()->ResetLanesArray();
	_showArrowsCheckBox.Get()->SetIsChecked(ECheckBoxState::Unchecked);
	return FReply::Handled();
}

bool SOpenDRIVEEditorParamWidget::CheckIfInEditorMode() const
{
	return !(GEditor->IsPlayingSessionInEditor());
}

void SOpenDRIVEEditorParamWidget::SetOffset(const FText& newOffset)
{
	FString string = newOffset.ToString();

	float offset = string.IsNumeric() ? FCString::Atof(*string) : 10.f;

	GetEdMode()->SetRoadOffset(offset);
}

void SOpenDRIVEEditorParamWidget::OnCheckStateChanged(ECheckBoxState state)
{
	GetEdMode()->SetRoadsArrowsVisibilityInEditor(state == ECheckBoxState::Checked);
}

void SOpenDRIVEEditorParamWidget::OnOffsetValueChanged(float value)
{
	_offsetTextPtr->SetText(FText::FromString("Road's Zoffset : " + FString::FormatAsNumber(value)));
	GetEdMode()->SetRoadOffset(value);
}

void SOpenDRIVEEditorParamWidget::OnStepValueChanged(float value)
{
	_stepTextPtr->SetText(FText::FromString("Step : " + FString::FormatAsNumber(value)));
	GetEdMode()->SetStep(value);
}

FOpenDRIVEEditorMode* SOpenDRIVEEditorParamWidget::GetEdMode() const
{
	return (FOpenDRIVEEditorMode*)GLevelEditorModeTools().GetActiveMode(FOpenDRIVEEditorMode::EM_RoadMode); 
}
