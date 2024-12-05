#pragma once
#include "CoreMinimal.h"
#include "IOpenDriveModuleInterface.h"

class FOpenDriveEditorModule : public IOpenDriveModuleInterface
{
public:
	// IModuleInterface implementation start
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// IModuleInterface implementation end

	// IOpenDriveModuleInterface implementation start
	virtual void AddModuleListeners() override;
	// IOpenDriveModuleInterface implementation end

	static inline FOpenDriveEditorModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FOpenDriveEditorModule>("OpenDriveEditor");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("OpenDriveEditor");
	}

protected:
	
	void RegisterMenuExtensions();
	void AddToolbarExtension(FToolBarBuilder& Builder);
	void Toggle();

	TSharedPtr<FUICommandList> OpenDriveCommands;
};
