#include "EditorMode/OpenDriveEditorModeToolkit.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "OpenDriveEditorModeToolkit"

void FOpenDriveEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);
}

void FOpenDriveEditorModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(NAME_Default);
}

FName FOpenDriveEditorModeToolkit::GetToolkitFName() const
{
	return FName("OpenDriveEditorMode");
}

FText FOpenDriveEditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "OpenDriveEditorMode Toolkit");
}

#undef LOCTEXT_NAMESPACE
