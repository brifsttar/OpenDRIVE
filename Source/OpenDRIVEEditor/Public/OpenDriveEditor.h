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
	
	TSharedPtr<FUICommandList> OpenDriveCommands;

	// switch to EdMode
	void ToggleEditorMode();
	ECheckBoxState GetOpenDriveModeStatus();

	// auto align with lane
	void ToggleAutoAlignWithLane();
	bool CanToggleAutoAlignWithLane();
	ECheckBoxState GetActionCheckState();
};
