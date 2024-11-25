#include "OpenDRIVEEditor.h"
#include "LevelEditor.h"
#include "EditorMode/OpenDriveEditorModeCommands.h"
#include "EditorMode/OpenDriveEditorModeStyle.h"
#include "OpenDriveAssetActions.h"
#include "EditorMode/Tools/Gizmo/OpenDriveGizmoSubsystem.h"

#define LOCTEXT_NAMESPACE "FOpenDriveEditorModule"

void FOpenDriveEditorModule::StartupModule() 
{
	IOpenDriveModuleInterface::StartupModule();
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FOpenDriveEditorModule::RegisterMenuExtensions));
}

void FOpenDriveEditorModule::ShutdownModule() 
{
	FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor").GetGlobalLevelEditorActions()->UnmapAction(FOpenDriveEditorModeCommands::Get().OpenDriveGizmoTool);
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
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	const TSharedPtr<FUICommandList> Commands = LevelEditorModule.GetGlobalLevelEditorActions();
	Commands->MapAction(
		FOpenDriveEditorModeCommands::Get().OpenDriveGizmoTool,
		FExecuteAction::CreateRaw(this, &FOpenDriveEditorModule::Toggle),
		FCanExecuteAction()
	);
	
	TSharedPtr<FExtender> NewExtender = MakeShareable(new FExtender());
	NewExtender->AddToolBarExtension("Transform", EExtensionHook::After, Commands, FToolBarExtensionDelegate::CreateRaw(this, &FOpenDriveEditorModule::AddToolbarExtension));
	LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(NewExtender);
}

void FOpenDriveEditorModule::AddToolbarExtension(FToolBarBuilder& builder)
{
	FUIAction ToggleButtonAction;
	ToggleButtonAction.GetActionCheckState.BindLambda([&]
	{
		return bOpenDriveGizmoOn ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	});
	ToggleButtonAction.ExecuteAction.BindRaw(this,&FOpenDriveEditorModule::Toggle);

	builder.AddSeparator();

	builder.AddToolBarButton(
	ToggleButtonAction,
	NAME_Default,
	FText::FromString("Switch to OpenDRIVE translation mode"),
	FText::FromString("Allows to move Actors following lanes described in Odr file (only works in Local Space)"),
	FSlateIcon(FOpenDriveEditorModeStyleSet::GetStyleSetName(), "OpenDriveEditorModeIcon", "OpenDriveEditorModeIcon.Small"),
	EUserInterfaceActionType::ToggleButton
	);
}

void FOpenDriveEditorModule::Toggle()
{
	bOpenDriveGizmoOn = !bOpenDriveGizmoOn;
	GEditor->GetEditorSubsystem<UOpenDriveGizmoSubsystem>()->ToggleOpenDriveGizmoMode(bOpenDriveGizmoOn);
}

IMPLEMENT_MODULE(FOpenDriveEditorModule, OpenDRIVEEditor)

#undef LOCTEXT_NAMESPACE
