// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class OpenDRIVEEditor : ModuleRules
{
	public OpenDRIVEEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				ModuleDirectory,
				Path.Combine(ModuleDirectory, "Public"),
			}
		);
		
		PrivateIncludePaths.AddRange(
			new string[] {
				Path.Combine(ModuleDirectory, "Private"),
				System.IO.Path.GetFullPath(Target.RelativeEnginePath) + "Source/Editor/Blutility/Private"
			}
		);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"OpenDRIVE",
				"RoadManager",
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Blutility",
				"UnrealEd",
				"Landscape",
				"EditorScriptingUtilities",
			}
		);
    }
}
