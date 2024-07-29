#pragma once 
#include "Framework/Commands/Commands.h"
#include "Styling/AppStyle.h"
#include "IOpenDriveModuleInterface.h"

#define LOCTEXT_NAMESPACE "OpenDriveCommands"


class OpenDriveCommands : public TCommands<OpenDriveCommands>
{

public:

	OpenDriveCommands() : TCommands<OpenDriveCommands>(
		"OpenDriveCommands",
		LOCTEXT("OpenDriveCommandsName", "Open Drive Commands"),
		NAME_None,
		FAppStyle::GetAppStyleSetName()
	)
	{}

	void RegisterCommands() override
	{
		UI_COMMAND
		(
			SetEnableGizmo,
			"Enable/Disable OpenDrive gizmo",
			"Enable or disable the OpenDrive gizmo",
			EUserInterfaceActionType::Button,
			FInputChord()
		);
	}

public : 

	TSharedPtr<FUICommandInfo> SetEnableGizmo;
};

class OpenDriveCommandsModuleListener : public IOpenDRIVEModuleListenerInterface, public TSharedFromThis<OpenDriveCommandsModuleListener>
{
public : 

	virtual ~OpenDriveCommandsModuleListener();

	virtual void OnStartupModule() override;

	virtual void OnShutdownModule() override;

	void MapCommands();
};

#undef LOCTEXT_NAMESPACE
