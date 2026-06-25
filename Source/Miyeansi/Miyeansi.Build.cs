// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Miyeansi : ModuleRules
{
	public Miyeansi(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
                "CommonLibrary",
				"CustomUI",
				"StoryFlow",
				"SaveGame",
				"ItemCore",
				"VisualNovel",
				// "InteractionSystem" 쓸때 넣기
			}
			);	
	}
}
