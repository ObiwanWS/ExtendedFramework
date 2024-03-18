// Copyright (c) 2024 Obiwan Medeiros.

using UnrealBuildTool;

public class ExtendedGameFramework : ModuleRules
{
	public ExtendedGameFramework(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] 
			{
                "ExtendedGameFramework",
            }
		);
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
		);
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
                "NetCore",
            }
		);
    }
}
