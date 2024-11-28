// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TPSPortfolio : ModuleRules
{
	public TPSPortfolio(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", 
            "UMG", "Niagara", "HeadMountedDisplay", "EnhancedInput", "AIModule", 
            "GameplayTasks", "NavigationSystem", "Paper2D" ,"Chaos", "ChaosSolverEngine", 
            "FieldSystemEngine", "GeometryCollectionEngine", "SlateCore","GeometryCore",
            "GeometryFramework", "DynamicMesh"});
		PrivateIncludePaths.Add("TPSPortfolio");
        PrivateIncludePaths.Add("TPSPortfolio/Public");
        PrivateIncludePaths.Add("TPSPortfolio/FlowfieldPathfinder");
        PrivateIncludePaths.Add("TPSPortfolio/FlowfieldVolume");
        PrivateIncludePaths.Add("TPSPortfolio/Skill");
        PrivateIncludePaths.Add("TPSPortfolio/UI");
        PrivateIncludePaths.Add("TPSPortfolio/Subsystem");

        PrivateDependencyModuleNames.AddRange(new string[] {
			// AI/MassAI Plugin Modules
			"MassAIBehavior",
            "MassAIDebug",

			// Runtime/MassEntity Plugin Modules
			"MassEntity",

			// Runtime/MassGameplay Plugin Modules
			"MassActors",
            "MassCommon",
            "MassGameplayDebug",
            "MassLOD",
            "MassMovement",
            "MassNavigation",
            "MassRepresentation",
            "MassReplication",
            "MassSpawner",
            "MassSimulation",
            "MassSignals",

            "StructUtils",
        });
    }
}
