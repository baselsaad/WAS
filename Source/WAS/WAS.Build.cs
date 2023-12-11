// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WAS : ModuleRules
{
    public WAS(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp17;

        string[] includePaths = new string[]
        {
                "WAS",
                "WAS/GameMode",
                "WAS/Manager",
                "WAS/Items",
                "WAS/Player",
                "WAS/Tools",
                "WAS/Menu",
                "WAS/Puzzles",
                "WAS/AudioSys",
        };

        string[] DependencyModule = new string[]
        {
               "Core",
               "CoreUObject",
               "Engine",
               "InputCore",
               "HeadMountedDisplay",
               "UMG",
               "RHI",
               "PhysicsCore",
               "GameplayCameras",
               "Slate",
               "SlateCore",
               "MediaAssets",
               "AudioMixer",
               "NavigationSystem"
        };

        PublicIncludePaths.AddRange(includePaths);
        PublicDependencyModuleNames.AddRange(DependencyModule);
    }
}
