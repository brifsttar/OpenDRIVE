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
				Path.Combine(ModuleDirectory, "Public")
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
				"Engine",
				"CoreUObject",
				"InputCore",
				"LevelEditor",
				"Slate",
				"EditorStyle",
				"AssetTools",
				"EditorWidgets",
				"UnrealEd",
				"BlueprintGraph",
				"AnimGraph",
				"ComponentVisualizers",
				"OpenDRIVE",
				"InputCore",
				"LevelEditor",
				"EditorSubsystem",
				"OpenDRIVE",
				"RoadManager",
                "InteractiveToolsFramework"
            }
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"AppFramework",
				"Slate",
				"SlateCore",
				"ToolMenus",
				"EditorFramework",
				"EditorStyle",
				"AnimGraph",
				"Blutility",
				"UnrealEd",
				"KismetWidgets",
				"MainFrame",
				"PropertyEditor",
				"ComponentVisualizers",
				"Landscape",
				"EditorScriptingUtilities",
				"NavigationSystem",
				"InteractiveToolsFramework",
				"EditorInteractiveToolsFramework",
				"LevelEditor"
			}
		);
    }
}
