// Copyright Epic Games, Inc. All Rights Reserved.

#include "OpenDRIVELoader.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FOpenDRIVELoaderModule"

void FOpenDRIVELoaderModule::StartupModule() {
#if PLATFORM_WINDOWS
	FString BaseDir = IPluginManager::Get().FindPlugin("OpenDRIVE")->GetBaseDir();
	FString LibraryPath = FPaths::Combine(
		*BaseDir,
		TEXT("Source/ThirdParty/RoadManager/bin/RelWithDebInfo/RoadManager.dll")
	);

	RoadManagerHandle = FPlatformProcess::GetDllHandle(*LibraryPath);
#else
	RoadManagerHandle = nullptr;
#endif
}

void FOpenDRIVELoaderModule::ShutdownModule() {
#if PLATFORM_WINDOWS
	if (RoadManagerHandle)
	{
		FPlatformProcess::FreeDllHandle(RoadManagerHandle);
	}
#endif
	RoadManagerHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOpenDRIVELoaderModule, OpenDRIVELoader)
