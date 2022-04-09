// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OpenDRIVE : ModuleRules
{
	public OpenDRIVE(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"OpenDRIVELoader",
				"RoadManager",
				"Projects"
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"ChaosVehicles",
				"Engine",
			}
		);

		if (Target.bBuildEditor) {
			PrivateDependencyModuleNames.Add("UnrealEd");
		}
	}
}
