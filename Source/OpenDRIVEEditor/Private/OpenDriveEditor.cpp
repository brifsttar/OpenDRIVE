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
	FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor").GetGlobalLevelEditorActions()->UnmapAction(FOpenDriveEditorModeCommands::Get().OpenDriveGizmoTool);
	OpenDriveCommands.Reset();
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
		FOpenDriveEditorModeCommands::Get().OpenDriveGizmoTool,
		FExecuteAction::CreateRaw(this, &FOpenDriveEditorModule::Toggle),
		FCanExecuteAction()
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
	FUIAction ToggleButtonAction;
	ToggleButtonAction.GetActionCheckState.BindLambda([&]
	{
		return GLevelEditorModeTools().IsModeActive(UOpenDriveEditorMode::EM_OpenDriveEditorModeId) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	});
	ToggleButtonAction.ExecuteAction.BindRaw(this,&FOpenDriveEditorModule::Toggle);

	Builder.AddSeparator();

	Builder.AddToolBarButton(
	ToggleButtonAction,
	NAME_Default,
	FText::FromString("Switch to OpenDRIVE Editor Mode"),
	FText::FromString("Shortcut to OpenDRIVE Editor Mode (visualization, Actors manipulation)"),
	FSlateIcon(FOpenDriveEditorModeStyleSet::GetStyleSetName(), "OpenDriveEditorModeIcon", "OpenDriveEditorModeIcon.Small"),
	EUserInterfaceActionType::ToggleButton
	);
}

void FOpenDriveEditorModule::Toggle()
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

IMPLEMENT_MODULE(FOpenDriveEditorModule, OpenDRIVEEditor)

#undef LOCTEXT_NAMESPACE
