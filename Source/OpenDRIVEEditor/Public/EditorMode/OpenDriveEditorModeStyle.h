#pragma once 

#include "CoreMinimal.h"
#include "IOpenDriveModuleInterface.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

/* This class is used to register a custom OpenDRIVE slate style for later uses */
class FOpenDriveEditorModeStyleSet : public IOpenDRIVEModuleListenerInterface
{
public : 

	/** IOpenDriveModuleListener Interface */
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
