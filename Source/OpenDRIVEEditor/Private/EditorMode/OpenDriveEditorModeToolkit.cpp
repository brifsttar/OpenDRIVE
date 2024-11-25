#include "EditorMode/OpenDriveEditorModeToolkit.h"
#include "EditorMode/OpenDriveEditorModeStyle.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "OpenDriveEditorModeToolkit"

void FOpenDriveEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	/* Create tool widget */
	TSharedPtr<SVerticalBox> ToolkitWidgetVBox = SNew(SVerticalBox);
	SAssignNew(ToolkitWidget, SBorder)
		.HAlign(HAlign_Fill)
		.Padding(4)
		[
			ToolkitWidgetVBox->AsShared()
		];

	/* Default init (for DetailsView creation) */
	FModeToolkit::Init(InitToolkitHost, InOwningMode); //bar vertical button

	/* Add DetailsView to toolkit widget */
	ToolkitWidgetVBox->AddSlot().HAlign(HAlign_Fill).FillHeight(1.f) //properties set
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

void FOpenDriveEditorModeToolkit::BuildToolPalette(FName PaletteName, FToolBarBuilder& ToolbarBuilder)
{
	if (!GetScriptableEditorMode().IsValid())
	{
		return;
	}

	/* Add icons for each tools */
	TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> CommandsList = GetScriptableEditorMode()->GetModeCommands();

	if (TArray<TSharedPtr<FUICommandInfo>>* CurrentCommandListPtr = CommandsList.Find(PaletteName))
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

#undef LOCTEXT_NAMESPACE
