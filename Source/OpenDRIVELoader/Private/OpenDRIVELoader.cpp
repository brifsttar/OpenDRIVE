// Copyright Epic Games, Inc. All Rights Reserved.

#include "OpenDRIVELoader.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FOpenDRIVELoaderModule"

void FOpenDRIVELoaderModule::StartupModule() {
	FString BaseDir = IPluginManager::Get().FindPlugin("OpenDRIVE")->GetBaseDir();
	FString LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/RoadManager/modules/esmini/buildVS2019_64_v141/EnvironmentSimulator/Modules/RoadManager/RelWithDebInfo/RoadManager.dll"));
	RoadManagerHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;
}

void FOpenDRIVELoaderModule::ShutdownModule() {
	FPlatformProcess::FreeDllHandle(RoadManagerHandle);
	RoadManagerHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOpenDRIVELoaderModule, OpenDRIVELoader)
