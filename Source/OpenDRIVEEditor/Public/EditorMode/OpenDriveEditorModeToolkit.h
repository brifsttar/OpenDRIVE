#pragma once 
#include "CoreMinimal.h"
#include "Toolkits/BaseToolkit.h"

#include "EditorMode/OpenDriveEditorMode.h"

class FOpenDriveEditorModeToolkit : public FModeToolkit
{
public : 

	FOpenDriveEditorModeToolkit() {}

	/** FModeToolkit */
	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode) override;
	virtual void GetToolPaletteNames(TArray<FName>& PaletteNames) const override;
	virtual TSharedPtr<SWidget> GetInlineContent() const override;
	virtual void BuildToolPalette(FName PaletteName, class FToolBarBuilder& ToolbarBuilder) override;

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;

private : ;

	TSharedPtr<SWidget> ToolkitWidget;
};
