#pragma once 
#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "IOpenDriveModuleInterface.h"

class FOpenDriveEditorModeCommands : public IOpenDriveModuleListenerInterface , public TCommands<FOpenDriveEditorModeCommands>
{
public : 

	FOpenDriveEditorModeCommands();

	// IOpenDriveModuleListener interface implementation start
	virtual void OnStartupModule() override;
	virtual void OnShutdownModule() override;
	// IOpenDriveModuleListener interface implementation end

	/** TCommands interface */
	virtual void RegisterCommands() override;

	static TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> GetCommands();
	
	TSharedPtr<FUICommandInfo> OpenDriveVisualizerTool;
	TSharedPtr<FUICommandInfo> OpenDriveUtilsTool;
	
	TSharedPtr<FUICommandInfo> OpenDriveSwitchToEditorMode;
	TSharedPtr<FUICommandInfo> OpenDriveAutoAlignToLane;
	
protected :

	TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> Commands;
};
