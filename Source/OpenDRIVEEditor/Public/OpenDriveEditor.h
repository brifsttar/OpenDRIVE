// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "UnrealEd.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Editor/LevelEditor/Public/LevelEditor.h"
#include "Editor/PropertyEditor/Public/PropertyEditing.h"
#include "IAssetTypeActions.h"
#include "IOpenDriveModuleInterface.h"

class FOpenDRIVEEditorModule : public IOpenDRIVEModuleInterface
{
public:
	/** IModuleInterface implementation **/
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual void AddModuleListeners() override;

	static inline FOpenDRIVEEditorModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FOpenDRIVEEditorModule>("OpenDriveEditor");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("OpenDriveEditor");
	}

	TSharedRef<FWorkspaceItem> GetMenuRoot() { return MenuRoot; };

protected:

	TSharedPtr<FExtensibilityManager> LevelEditorMenuExtensibilityManager;
	TSharedPtr<FExtender> MenuExtender;

	static TSharedRef<FWorkspaceItem> MenuRoot;

private:

	TSharedPtr<IAssetTypeActions> OpenDRIVEAssetTypeActions;
};
