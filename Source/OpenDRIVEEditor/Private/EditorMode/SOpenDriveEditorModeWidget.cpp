#include "Public/EditorMode/SOpenDriveEditorModeWidget.h"
#include "Public/OpenDriveEditor.h"
#include "Public/EditorMode/OpenDriveEditorMode.h"
#include "Public/EditorMode/SOpenDriveEditorModeWidget.h"

void SOpenDRIVEEditorModeWidget::Construct(const FArguments& InArgs)
{
	AssetThumbnailPoolPtr = MakeShareable(new FAssetThumbnailPool(24));

	OpenDRIVEAssetProBoxPtr = SNew(SObjectPropertyEntryBox)
		.DisplayBrowse(true)
		.EnableContentPicker(true)
		.DisplayThumbnail(true)
		.ThumbnailPool(AssetThumbnailPoolPtr)
		.AllowedClass(UOpenDriveAsset::StaticClass())
		.AllowClear(true)
		.OnObjectChanged(FOnSetObject::CreateSP(this, &SOpenDRIVEEditorModeWidget::OnObjectChanged))
		.ObjectPath(this, &SOpenDRIVEEditorModeWidget::GetAssetDataPath);

	ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f,5.f,0.f,0.f)
			[
				ConstructButtons(InArgs)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(20.f, 10.f, 20.f, 0.f)
			[
				SNew(SEditableTextBox)
				.Justification(ETextJustify::Center)
				.HintText(FText::FromString(TEXT("Road Offset")))
				.OnTextChanged(this, &SOpenDRIVEEditorModeWidget::SetOffset)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 30.f, 0.f, 0.f)
			[
				ConstructLaneInfoBox(InArgs)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 30.f, 0.f, 0.f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(0.5f)
				.Padding(20, 10, 0, 0)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("OpenDRIVE Asset")))
				]
				+ SHorizontalBox::Slot()
				.FillWidth(0.5f)
				.Padding(10, 0, 20, 0)
				[
					OpenDRIVEAssetProBoxPtr.ToSharedRef()
				]
			]
		];
}

TSharedRef<SBorder> SOpenDRIVEEditorModeWidget::ConstructLaneInfoBox(const FArguments& InArgs)
{
	RoadIdTextPtr = SNew(STextBlock)
		.Text(FText::FromString(TEXT("RoadId : ")));

	JunctionIdTextPtr = SNew(STextBlock)
		.Text(FText::FromString(TEXT("JunctionId : ")));

	LaneTypeTextPtr = SNew(STextBlock)
		.Text(FText::FromString(TEXT("Lane type : ")));

	LaneIdTextPtr = SNew(STextBlock)
		.Text(FText::FromString(TEXT("LaneId : ")));

	TSharedRef<SBorder> border =
		SNew(SBorder)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5.f, 10.f, 0.f, 0.f)
			[
				RoadIdTextPtr.ToSharedRef()
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5.f, 5.f, 0.f, 0.f)
			[
				LaneIdTextPtr.ToSharedRef()
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5.f, 5.f, 0.f, 0.f)
			[
				JunctionIdTextPtr.ToSharedRef()
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5.f, 5.f, 0.f, 10.f)
			[
				LaneTypeTextPtr.ToSharedRef()
			]
		];

	return border;
}

TSharedRef<SHorizontalBox> SOpenDRIVEEditorModeWidget::ConstructButtons(const FArguments& InArgs)
{
	TSharedPtr<SButton> resetButton = SNew(SButton).Text(FText::FromString("Reset"))
		.OnClicked(this, &SOpenDRIVEEditorModeWidget::Reset).IsEnabled(this, &SOpenDRIVEEditorModeWidget::IsLoaded);

	StaticCast<STextBlock&>(resetButton.ToSharedRef().Get().GetContent().Get()).SetJustification(ETextJustify::Center);

	TSharedPtr<SButton> generateButton = SNew(SButton).Text(FText::FromString("Generate"))
		.OnClicked(this, &SOpenDRIVEEditorModeWidget::Generate).IsEnabled(this, &SOpenDRIVEEditorModeWidget::CheckIfSimulating);

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
	RoadIdTextPtr.Get()->SetText(FText::FromString("Road Id : " + FString::FromInt(lane_->GetRoadId())));

	JunctionIdTextPtr.Get()->SetText(FText::FromString("Junction Id : " + FString::FromInt(lane_->GetJunctionId())));

	LaneTypeTextPtr.Get()->SetText(FText::FromString("Lane type : " + lane_->GetLaneType()));

	LaneIdTextPtr.Get()->SetText(FText::FromString("Lane Id : " + FString::FromInt(lane_->GetLaneId())));
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
		_assetData = assetData_;
		GetEdMode()->SetOpenDRIVEAsset(openDRIVEAsset);
	}
}

