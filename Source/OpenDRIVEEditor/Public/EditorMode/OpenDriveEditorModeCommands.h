#pragma once 

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "IOpenDriveModuleInterface.h"

class FOpenDriveEditorModeCommands : public IOpenDRIVEModuleListenerInterface , public TCommands<FOpenDriveEditorModeCommands>
{
public : 

	FOpenDriveEditorModeCommands();

	/** IOpenDriveModuleListener Interface */
	virtual void OnStartupModule() override;
	virtual void OnShutdownModule() override;

	/** TCommands interface */
	virtual void RegisterCommands() override;

	static TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> GetCommands();

	/* Each command corresponds to one tool */
	TSharedPtr<FUICommandInfo> OpenDriveVisualizerTool;
	TSharedPtr<FUICommandInfo> OpenDriveUtilsTool;

protected :

	TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> Commands;
};
