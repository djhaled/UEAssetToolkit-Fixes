// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UnrealPSKPSA : ModuleRules
{
	public UnrealPSKPSA(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		bLegacyPublicIncludePaths = false;
        ShadowVariableWarningLevel = WarningLevel.Off;
        PublicDefinitions.Add("WIN32_LEAN_AND_MEAN");
		
		PublicIncludePaths.AddRange(new string[] {
			// ... add public include paths required here ...
		});
				
		
		PrivateIncludePaths.AddRange(new string[] {
			// ... add other private include paths required here ...
		});
			
		
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			// ... add other public dependencies that you statically link with here ...
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore",
			"ProceduralMeshComponent",
			"UnrealEd",
			"Projects",
			"MeshDescription",
            "RawMesh",
			"RenderCore",
			"MeshBuilder",
			"MeshUtilitiesCommon", 
			"EditorScriptingUtilities", 
			"GeometricObjects",
		});
		
		DynamicallyLoadedModuleNames.AddRange(new string[] {
			// ... add any modules that your module loads dynamically here ...
		});
	}
}
