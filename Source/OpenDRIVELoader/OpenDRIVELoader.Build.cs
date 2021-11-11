// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OpenDRIVELoader : ModuleRules
{
	public OpenDRIVELoader(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"RoadManager",
				"Projects"
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
			}
		);
	}
}
