#include "EditorMode/OpenDriveEditorModeToolkit.h"
#include "EditorMode/SOpenDriveEditorModeWidget.h"
#include "EditorMode/OpenDriveEditorModeStyle.h"

#include "Engine/Selection.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "OpenDriveEditorModeToolkit"

FOpenDriveEditorModeToolkit::FOpenDriveEditorModeToolkit()
{
	//Custom UI here ? 
}

void FOpenDriveEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	TSharedPtr<SVerticalBox> ToolkitWidgetVBox = SNew(SVerticalBox);
	SAssignNew(ToolkitWidget, SBorder)
		.HAlign(HAlign_Fill)
		.Padding(4)
		[
			ToolkitWidgetVBox->AsShared()
		];

	FModeToolkit::Init(InitToolkitHost, InOwningMode);

	ToolkitWidgetVBox->AddSlot().HAlign(HAlign_Fill).FillHeight(1.f)
		[
			DetailsView->AsShared()
		];
}

void FOpenDriveEditorModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(NAME_Default);
}

TSharedPtr<SWidget> FOpenDriveEditorModeToolkit::GetInlineContent() const
{
	return ToolkitWidget;
}

TSharedRef<SWidget> FOpenDriveEditorModeToolkit::CreatePaletteWidget(TSharedPtr<FUICommandList> InCommandList, FName InToolbarCustomizationName, FName InPaletteName)
{
	FUniformToolBarBuilder ModeToolbarBuilder(InCommandList, FMultiBoxCustomization(InToolbarCustomizationName));
	//ModeToolbarBuilder.SetStyle(&FOpenDriveEditorModeStyleSet::Get(), "OpenDriveEditor.ToolbarButton");
	ModeToolbarBuilder.SetStyle(&FAppStyle::Get(), "PaletteToolBar");

	BuildToolPalette(InPaletteName, ModeToolbarBuilder);

	return ModeToolbarBuilder.MakeWidget();
}

void FOpenDriveEditorModeToolkit::BuildToolPalette(FName PaletteName, FToolBarBuilder& ToolbarBuilder)
{
	if (!GetScriptableEditorMode().IsValid())
	{
		return;
	}

	TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> CommandsList = GetScriptableEditorMode()->GetModeCommands();
	TArray<TSharedPtr<FUICommandInfo>>* CurrentCommandListPtr = CommandsList.Find(PaletteName);

	if (CurrentCommandListPtr)
	{
		TArray<TSharedPtr<FUICommandInfo>> CurrentCommandList = *CurrentCommandListPtr;
		for (TSharedPtr<FUICommandInfo> Command : CurrentCommandList)
		{
			FString commandName = Command->GetCommandName().ToString();
			FString commandIconName = commandName + FString("Icon.Small");

			ToolbarBuilder.AddToolBarButton(
				Command,
				NAME_None,
				TAttribute<FText>(),
				TAttribute<FText>(),
				TAttribute<FSlateIcon>::Create
				(
					TAttribute<FSlateIcon>::FGetter::CreateLambda
					(
						[commandIconName]() { return FSlateIcon(FOpenDriveEditorModeStyleSet::GetStyleSetName(), *commandIconName); }
					)
				),
				NAME_None, 
				FNewMenuDelegate()); 
		}
	}
}

FName FOpenDriveEditorModeToolkit::GetToolkitFName() const
{
	return FName("OpenDriveEditorMode");
}

FText FOpenDriveEditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "OpenDrive Editor Mode Toolkit");
}

void FOpenDriveEditorModeToolkit::OnToolStarted(UInteractiveToolManager* Manager, UInteractiveTool* Tool)
{
	FModeToolkit::OnToolStarted(Manager, Tool); // update details
}

#undef LOCTEXT_NAMESPACE
