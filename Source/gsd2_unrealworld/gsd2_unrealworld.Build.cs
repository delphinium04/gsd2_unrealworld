// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class gsd2_unrealworld : ModuleRules
{
	public gsd2_unrealworld(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
			"Niagara",
			"AIModule",
			"GameplayTasks",
			"NavigationSystem",
			"UMG",
			"Slate",
			"SlateCore"
		});

	}
}
