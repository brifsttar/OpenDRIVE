// Copyright Epic Games, Inc. All Rights Reserved.

#include "Public/OpenDRIVEEditor.h"
#include "OpenDriveAssetActions.h"
#include "AssetToolsModule.h"
#include "Public/MenuTool.h"
#include "Public/EditorMode/OpenDriveEdModeTool.h"

IMPLEMENT_MODULE(FOpenDRIVEEditorModule, OpenDRIVEEditor)

#define LOCTEXT_NAMESPACE "FOpenDRIVEEditorModule"

TSharedRef<FWorkspaceItem> FOpenDRIVEEditorModule::MenuRoot =
FWorkspaceItem::NewGroup(FText::FromString("Menu Root"));

void FOpenDRIVEEditorModule::StartupModule() {

	if (!IsRunningCommandlet())
	{
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

		OpenDRIVEAssetTypeActions = MakeShareable(new FOpenDRIVEAssetActions);
		AssetTools.RegisterAssetTypeActions(OpenDRIVEAssetTypeActions.ToSharedRef());

		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		LevelEditorMenuExtensibilityManager = LevelEditorModule.GetMenuExtensibilityManager();
		MenuExtender = MakeShareable(new FExtender);
		MenuExtender->AddMenuBarExtension("Window", EExtensionHook::After, NULL, FMenuBarExtensionDelegate::CreateRaw(this, &FOpenDRIVEEditorModule::MakePulldownMenu));
		LevelEditorMenuExtensibilityManager->AddExtender(MenuExtender);
	}

	IOpenDRIVEModuleInterface::StartupModule();
}

void FOpenDRIVEEditorModule::ShutdownModule() {
	if (FModuleManager::Get().IsModuleLoaded("AssetTools")) {
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		if (OpenDRIVEAssetTypeActions.IsValid()) {
			AssetTools.UnregisterAssetTypeActions(OpenDRIVEAssetTypeActions.ToSharedRef());
		}
	}

	IOpenDRIVEModuleInterface::ShutdownModule();
}

void FOpenDRIVEEditorModule::AddModuleListeners()
{
	ModuleListeners.Add(MakeShareable(new MenuTool));
	ModuleListeners.Add(MakeShareable(new OpenDRIVEEdModeTool));
}

void FOpenDRIVEEditorModule::AddMenuExtension(const FMenuExtensionDelegate& extensionDelegate, FName extensionHook, const TSharedPtr<FUICommandList>& CommandList, EExtensionHook::Position position)
{
	MenuExtender->AddMenuExtension(extensionHook, position, CommandList, extensionDelegate);
}

void FOpenDRIVEEditorModule::MakePulldownMenu(FMenuBarBuilder& menuBuilder)
{
	menuBuilder.AddPullDownMenu(
		FText::FromString("OpenDRIVE"),
		FText::FromString("open the OpenDRIVE menu"),
		FNewMenuDelegate::CreateRaw(this, &FOpenDRIVEEditorModule::FillPulldownMenu),
		"OpenDRIVE",
		FName(TEXT("OpenDRIVE menu"))
	);
}

void FOpenDRIVEEditorModule::FillPulldownMenu(FMenuBuilder& menuBuilder)
{
	menuBuilder.BeginSection("ExampleSection", FText::FromString("Section 1"));
	menuBuilder.AddMenuSeparator(FName("Section_1"));
	menuBuilder.EndSection();

	menuBuilder.BeginSection("ExampleSection", FText::FromString("Section 2"));
	menuBuilder.AddMenuSeparator(FName("Section_2"));
	menuBuilder.EndSection();
}

#undef LOCTEXT_NAMESPACE
	
