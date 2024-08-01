#pragma once 
#include "OpenDriveEditor.h"
#include "IOpenDriveModuleInterface.h"
#include "Framework/Docking/TabManager.h"
#include "Widgets/Docking/SDockTab.h"

class FTabToolBase : public IOpenDRIVEModuleListenerInterface, public TSharedFromThis< FTabToolBase >
{
public :

	virtual void OnStartupModule() override
	{
		Initialize();

		FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TabName, FOnSpawnTab::CreateRaw(this, &FTabToolBase::SpawnTab))
			.SetGroup(FOpenDriveEditorModule::Get().GetMenuRoot())
			.SetDisplayName(TabDisplayName)
			.SetTooltipText(ToolTipText);
	}

	virtual void OnShutdownModule() override
	{
		FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TabName);
	};

	virtual void Initialize() {};

	virtual TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& TabSpawnArgs) { return SNew(SDockTab); };

	virtual void MakeMenuEntry(FMenuBuilder& menuBuilder)
	{
		FGlobalTabmanager::Get()->PopulateTabSpawnerMenu(menuBuilder, TabName);
	};

	virtual ~FTabToolBase() {};

protected : 

	FName TabName;
	FText TabDisplayName;
	FText ToolTipText;
};
