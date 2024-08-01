#pragma once 

#include "CoreMinimal.h"
#include "IOpenDriveModuleInterface.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

class FOpenDriveEditorModeStyleSet : public IOpenDRIVEModuleListenerInterface
{
public : 

	virtual void OnStartupModule() override;
	virtual void OnShutdownModule() override;

	void Initialize();
	void Shutdown();
	static const class ISlateStyle& Get(); 
	static FName GetStyleSetName();

private : 

	void Create();

	void CreateIcons();

	void CreateButtonsStyle();

	static TSharedPtr<class FSlateStyleSet> OpenDriveStyleInstance;
};
