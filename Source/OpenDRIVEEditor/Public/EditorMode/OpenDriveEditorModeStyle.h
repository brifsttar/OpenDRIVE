#pragma once 
#include "CoreMinimal.h"
#include "IOpenDriveModuleInterface.h"
#include "Styling/SlateStyle.h"

/**
 * A Custom OpenDriveEditor Slate Style Set to load OpenDrive's related icons and more
 */
class FOpenDriveEditorModeStyleSet : public IOpenDriveModuleListenerInterface
{
public : 

	// IOpenDriveModuleListener interface implementation start 
	virtual void OnStartupModule() override;
	virtual void OnShutdownModule() override;
	// IOpenDriveModuleListener interface implementation end
	
	static const class ISlateStyle& Get(); 
	static FName GetStyleSetName();

private : 

	void Create();
	void CreateIcons();

	static TSharedPtr<class FSlateStyleSet> OpenDriveStyleInstance;
};
