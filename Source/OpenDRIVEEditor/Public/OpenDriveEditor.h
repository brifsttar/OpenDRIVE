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

	// IOpenDriveModuleInterface implementation
	virtual void AddModuleListeners() override;

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
	void AddToolbarExtension(FToolBarBuilder& builder);
	bool bOpenDriveGizmoOn = false;
	void Toggle();

	TSharedPtr<FUICommandList> CommandList;
};
