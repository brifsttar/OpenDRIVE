#pragma once 
#include "CoreMinimal.h"
#include "Toolkits/BaseToolkit.h"
#include "EditorMode/OpenDriveEditorMode.h"

class FOpenDriveEditorModeToolkit : public FModeToolkit
{
public : 

	FOpenDriveEditorModeToolkit() {}
	
	// FModeToolkit implementation start
	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode) override;
	virtual void GetToolPaletteNames(TArray<FName>& PaletteNames) const override;
	// FModeToolkit implementation end

	// IToolkit interface implementation start
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	// IToolkit interface implementation end

private : 

	TSharedPtr<SWidget> ToolkitWidget;
};
