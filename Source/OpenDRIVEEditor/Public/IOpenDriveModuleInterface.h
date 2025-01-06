#pragma once 
#include "Modules/ModuleManager.h"

class IOpenDriveModuleListenerInterface
{
public :
	virtual ~IOpenDriveModuleListenerInterface(){}
	virtual void OnStartupModule() {};
	virtual void OnShutdownModule() {};
};

class IOpenDriveModuleInterface : public IModuleInterface
{
public :
	
	virtual void StartupModule() override
	{
		if (!IsRunningCommandlet())
		{
			AddModuleListeners();

			for (int i = 0; i < ModuleListeners.Num(); i++)
			{
				ModuleListeners[i]->OnStartupModule();
			}
		}
	}

	virtual void ShutdownModule() override
	{
		for (int i = 0; i < ModuleListeners.Num(); i++)
		{
			ModuleListeners[i]->OnShutdownModule();
		}
	}

	virtual void AddModuleListeners() {};

protected : 

	TArray<TSharedRef<IOpenDriveModuleListenerInterface>> ModuleListeners;
};
