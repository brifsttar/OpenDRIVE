// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class RoadManager : ModuleRules
{
	public RoadManager(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		string RoadManagerDir = Path.Combine(ModuleDirectory, "modules", "esmini");
		string RoadManagerSrcDir = Path.Combine(RoadManagerDir, "EnvironmentSimulator", "Modules");

		PublicIncludePaths.AddRange(
			new string[] {
				Path.Combine(RoadManagerSrcDir, "RoadManager"),
				Path.Combine(RoadManagerSrcDir, "CommonMini"),
				Path.Combine(RoadManagerDir, "externals/pugixml"),
			}
		);

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib", "RelWithDebInfo", "RoadManager.lib"));
            PublicDelayLoadDLLs.Add("RoadManager.dll");
            RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "bin", "RelWithDebInfo", "RoadManager.dll"));
        }
	}
}
