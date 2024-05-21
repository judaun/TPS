// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TPSPortfolio : ModuleRules
{
	public TPSPortfolio(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "Niagara", "HeadMountedDisplay", "EnhancedInput", "AIModule", "GameplayTasks", "NavigationSystem" });
		PrivateIncludePaths.Add("TPSPortfolio");
        PrivateIncludePaths.Add("TPSPortfolio/Public");
    }
}
