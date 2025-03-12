#include "OpenDriveEditor.h"
#include "EditorModes.h"
#include "EditorModeManager.h"
#include "LevelEditor.h"
#include "EditorMode/OpenDriveEditorModeCommands.h"
#include "EditorMode/OpenDriveEditorModeStyle.h"
#include "OpenDriveAssetActions.h"
#include "EditorMode/OpenDriveEditorMode.h"

#define LOCTEXT_NAMESPACE "FOpenDriveEditorModule"

void FOpenDriveEditorModule::StartupModule() 
{
	IOpenDriveModuleInterface::StartupModule();
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FOpenDriveEditorModule::RegisterMenuExtensions));
}

void FOpenDriveEditorModule::ShutdownModule() 
{
	if (UObjectInitialized()) {
		FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor").GetGlobalLevelEditorActions()->UnmapAction(FOpenDriveEditorModeCommands::Get().OpenDriveSwitchToEditorMode);
		FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor").GetGlobalLevelEditorActions()->UnmapAction(FOpenDriveEditorModeCommands::Get().OpenDriveAutoAlignToLane);
		FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor").GetGlobalLevelEditorActions()->UnmapAction(FOpenDriveEditorModeCommands::Get().OpenDriveOverrideActorHeight);
		OpenDriveCommands.Reset();
	}
	IOpenDriveModuleInterface::ShutdownModule();
}

void FOpenDriveEditorModule::AddModuleListeners()
{
	ModuleListeners.Add(MakeShareable(new FOpenDriveEditorModeStyleSet));
	ModuleListeners.Add(MakeShareable(new FOpenDriveEditorModeCommands));
	ModuleListeners.Add(MakeShareable(new FOpenDriveAssetActions));
}

void FOpenDriveEditorModule::RegisterMenuExtensions()
{
	OpenDriveCommands = MakeShareable(new FUICommandList());
	
	OpenDriveCommands->MapAction(
		FOpenDriveEditorModeCommands::Get().OpenDriveSwitchToEditorMode,
		FExecuteAction::CreateRaw(this, &FOpenDriveEditorModule::ToggleEditorMode),
		FCanExecuteAction(),
		FGetActionCheckState::CreateRaw(this, &FOpenDriveEditorModule::IsOpenDriveModeActive_CheckState)
	);

	OpenDriveCommands->MapAction(
		FOpenDriveEditorModeCommands::Get().OpenDriveAutoAlignToLane,
		FExecuteAction::CreateRaw(this, &FOpenDriveEditorModule::ToggleAutoAlignWithLane),
		FCanExecuteAction::CreateRaw(this, &FOpenDriveEditorModule::IsOpenDriveEditorModeActive),
		FGetActionCheckState::CreateRaw(this, &FOpenDriveEditorModule::IsAutoWithLaneChecked)
	);

	OpenDriveCommands->MapAction(
		FOpenDriveEditorModeCommands::Get().OpenDriveOverrideActorHeight,
		FExecuteAction::CreateRaw(this, &FOpenDriveEditorModule::ToggleOverrideHeight),
		FCanExecuteAction::CreateRaw(this, &FOpenDriveEditorModule::IsOpenDriveEditorModeActive),
		FGetActionCheckState::CreateRaw(this, &FOpenDriveEditorModule::IsOverrideHeightChecked)
	);
	
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	const TSharedPtr<FUICommandList> Commands = LevelEditorModule.GetGlobalLevelEditorActions();
	Commands->Append(OpenDriveCommands.ToSharedRef());

	TSharedPtr<FExtender> NewExtender = MakeShareable(new FExtender());
	NewExtender->AddToolBarExtension("Transform", EExtensionHook::After, Commands, FToolBarExtensionDelegate::CreateRaw(this, &FOpenDriveEditorModule::AddToolbarExtension));
	LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(NewExtender);
}

void FOpenDriveEditorModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddSeparator();
	Builder.BeginSection("OpenDriveEditor");
	Builder.BeginBlockGroup();
	Builder.AddToolBarButton(
		FOpenDriveEditorModeCommands::Get().OpenDriveSwitchToEditorMode,
		NAME_Default,
		FText::FromString("Open Drive Mode"),
		FText::FromString("Toggles Open Drive Mode")
	);
	Builder.AddToolBarButton(
		FOpenDriveEditorModeCommands::Get().OpenDriveAutoAlignToLane,
		NAME_Default,
		FText::FromString("Auto align with lane"),
		FText::FromString("Toggles auto lane alignment with OpenDrive gizmo S and T translation"),
		TAttribute<FSlateIcon>::CreateLambda([]() -> FSlateIcon
		{
			if (const UOpenDriveEditorMode* CastEditorMode = Cast<UOpenDriveEditorMode>(GLevelEditorModeTools().GetActiveScriptableMode(UOpenDriveEditorMode::EM_OpenDriveEditorModeId)))
			{
				return CastEditorMode->bAutoAlignWithLane ?
					FSlateIcon(FAppStyle::GetAppStyleSetName(), "Sequencer.LockSequence"):
					FSlateIcon(FAppStyle::GetAppStyleSetName(), "Sequencer.UnlockSequence");
			}
			return FSlateIcon(FAppStyle::GetAppStyleSetName(), "Sequencer.UnlockSequence");
		})
	);
	Builder.AddToolBarButton(
		FOpenDriveEditorModeCommands::Get().OpenDriveOverrideActorHeight,
		NAME_Default,
		FText::FromString("Override Actor height"),
		FText::FromString("Enables this to override Actor's height with OpenDrive's height while translating with Gizmo"),
		TAttribute<FSlateIcon>::CreateLambda([]() -> FSlateIcon
		{
			if (const UOpenDriveEditorMode* CastEditorMode = Cast<UOpenDriveEditorMode>(GLevelEditorModeTools().GetActiveScriptableMode(UOpenDriveEditorMode::EM_OpenDriveEditorModeId)))
			{
				return CastEditorMode->bOverrideHeight ?
					FSlateIcon(FAppStyle::GetAppStyleSetName(), "EditableComboBox.Accept"):
					FSlateIcon(FAppStyle::GetAppStyleSetName(), "EditableComboBox.Delete");
			}
			return FSlateIcon(FAppStyle::GetAppStyleSetName(), "EditableComboBox.Delete");
		})
	);
	Builder.EndBlockGroup();
	Builder.EndSection();
}  

void FOpenDriveEditorModule::ToggleEditorMode()
{
	if (GLevelEditorModeTools().IsModeActive(UOpenDriveEditorMode::EM_OpenDriveEditorModeId))
	{
		GLevelEditorModeTools().ActivateMode(FBuiltinEditorModes::EM_Default);
	}
	else
	{
		GLevelEditorModeTools().ActivateMode(UOpenDriveEditorMode::EM_OpenDriveEditorModeId);
	}
}

ECheckBoxState FOpenDriveEditorModule::IsOpenDriveModeActive_CheckState()
{
	return GLevelEditorModeTools().IsModeActive(UOpenDriveEditorMode::EM_OpenDriveEditorModeId) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void FOpenDriveEditorModule::ToggleAutoAlignWithLane()
{
	if (UOpenDriveEditorMode* CastEditorMode = Cast<UOpenDriveEditorMode>(GLevelEditorModeTools().GetActiveScriptableMode(UOpenDriveEditorMode::EM_OpenDriveEditorModeId)))
	{
		CastEditorMode->ToggleAutoAlignWithLane();
	}
}

bool FOpenDriveEditorModule::IsOpenDriveEditorModeActive()
{
	return GLevelEditorModeTools().IsModeActive(UOpenDriveEditorMode::EM_OpenDriveEditorModeId);
}

ECheckBoxState FOpenDriveEditorModule::IsAutoWithLaneChecked()
{
	if (const UOpenDriveEditorMode* CastEditorMode = Cast<UOpenDriveEditorMode>(GLevelEditorModeTools().GetActiveScriptableMode(UOpenDriveEditorMode::EM_OpenDriveEditorModeId)))
	{
		return CastEditorMode->bAutoAlignWithLane ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}
	return ECheckBoxState::Unchecked;
}

void FOpenDriveEditorModule::ToggleOverrideHeight()
{
	if (UOpenDriveEditorMode* CastEditorMode = Cast<UOpenDriveEditorMode>(GLevelEditorModeTools().GetActiveScriptableMode(UOpenDriveEditorMode::EM_OpenDriveEditorModeId)))
	{
		CastEditorMode->ToggleOverrideHeight();
	}
}

ECheckBoxState FOpenDriveEditorModule::IsOverrideHeightChecked()
{
	if (const UOpenDriveEditorMode* CastEditorMode = Cast<UOpenDriveEditorMode>(GLevelEditorModeTools().GetActiveScriptableMode(UOpenDriveEditorMode::EM_OpenDriveEditorModeId)))
	{
		return CastEditorMode->bOverrideHeight ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}
	return ECheckBoxState::Unchecked;
}

IMPLEMENT_MODULE(FOpenDriveEditorModule, OpenDRIVEEditor)

#undef LOCTEXT_NAMESPACE
