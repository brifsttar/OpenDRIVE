#include "OpenDriveCommands.h"
#include "OpenDriveEditor.h"
#include "Gizmo/OpenDriveGizmoSubsystem.h"

OpenDriveCommandsModuleListener::~OpenDriveCommandsModuleListener()
{

}

void OpenDriveCommandsModuleListener::OnStartupModule()
{
	OpenDriveCommands::Register();
	MapCommands();
}

void OpenDriveCommandsModuleListener::OnShutdownModule()
{
	OpenDriveCommands::Unregister();
}

void OpenDriveCommandsModuleListener::MapCommands()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	const auto& Commands = OpenDriveCommands::Get();
	const auto& CommandsList = LevelEditorModule.GetGlobalLevelEditorActions();

	CommandsList->MapAction(
		Commands.SetEnableGizmo,
		FExecuteAction::CreateLambda([]()
			{
				if (GEditor)
				{
					UOpenDriveGizmoSubsystem* subsystem = GEditor->GetEditorSubsystem<UOpenDriveGizmoSubsystem>();

					if (subsystem)
					{
						subsystem->SetEnableGizmo();
					}
				}
			})
	);
}
