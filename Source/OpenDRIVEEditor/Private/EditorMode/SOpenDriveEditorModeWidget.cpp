#include "Public/EditorMode/SOpenDriveEditorModeWidget.h"
#include "Public/OpenDriveEditor.h"
#include "Public/EditorMode/OpenDriveEditorMode.h"
#include "Public/EditorMode/SOpenDriveEditorModeWidget.h"

void SOpenDRIVEEditorModeWidget::Construct(const FArguments& InArgs)
{
	AssetThumbnailPoolPtr = MakeShareable(new FAssetThumbnailPool(24));

	RoadIdTextPtr = SNew(STextBlock)
		.Text(FText::FromString(TEXT("RoadId : ")));
	
	JunctionIdTextPtr = SNew(STextBlock)
		.Text(FText::FromString(TEXT("JunctionId : ")));

	LaneTypeTextPtr = SNew(STextBlock)
		.Text(FText::FromString(TEXT("Lane type : ")));

	LaneIdTextPtr = SNew(STextBlock)
		.Text(FText::FromString(TEXT("LaneId : ")));

	OpenDRIVEAssetProBoxPtr = SNew(SObjectPropertyEntryBox)
		.DisplayBrowse(true)
		.EnableContentPicker(true)
		.DisplayThumbnail(true)
		.ThumbnailPool(AssetThumbnailPoolPtr)
		.AllowedClass(UOpenDriveAsset::StaticClass())
		.AllowClear(true)
		.OnObjectChanged(FOnSetObject::CreateSP(this, &SOpenDRIVEEditorModeWidget::OnObjectChanged))
		.ObjectPath(this, &SOpenDRIVEEditorModeWidget::GetAssetDataPath);

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

	ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f,5.f,0.f,0.f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
		        .HAlign(HAlign_Left)
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
				.Padding(10, 0, 2, 0)
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
					.HAlign(HAlign_Right)
					.Text(FText::FromString("Generate"))
					.OnClicked(this, &SOpenDRIVEEditorModeWidget::Generate)
					.IsEnabled(this, &SOpenDRIVEEditorModeWidget::CheckIfSimulating)
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 10.f, 0.f, 0.f)
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
				border
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 30.f, 0.f, 0.f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.Padding(2, 0, 0, 0)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("OpenDRIVE Asset")))
				]
			+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Right)
				.Padding(20, 0, 0, 0)
				.VAlign(VAlign_Center)
				[
					OpenDRIVEAssetProBoxPtr.ToSharedRef()
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

