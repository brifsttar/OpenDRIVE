#pragma once 
#include "Modules/ModuleManager.h"

class IOpenDRIVEModuleListenerInterface
{
public :
	virtual void OnStartupModule() {};
	virtual void OnShutdownModule() {};
};

class IOpenDRIVEModuleInterface : public IModuleInterface
{
public :

	void StartupModule() override
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

	void ShutdownModule() override
	{
		for (int i = 0; i < ModuleListeners.Num(); i++)
		{
			ModuleListeners[i]->OnShutdownModule();
		}
	}

	virtual void AddModuleListeners() {};

protected : 

	TArray<TSharedRef<IOpenDRIVEModuleListenerInterface>> ModuleListeners;
};
