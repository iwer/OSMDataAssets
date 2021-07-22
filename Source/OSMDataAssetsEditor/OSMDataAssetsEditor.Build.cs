// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OSMDataAssetsEditor : ModuleRules
{
	public OSMDataAssetsEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
                // ... add public include paths required here ...
            }
            );


        PrivateIncludePaths.AddRange(
            new string[] {
                // ... add other private include paths required here ...
                "OSMDataAssetsEditor/Private",
                "OSMDataAssetsEditor/Private/Factories",
                "OSMDataAssetsEditor/Private/Helpers"
            }
            );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "ContentBrowser",
				"Core",
				"CoreUObject",
				"DesktopWidgets",
				"EditorStyle",
				"Engine",
				"InputCore",
				"Projects",
                "RenderCore",
				"Slate",
				"SlateCore",
				"UnrealEd",
                "XmlParser",
                // ... add private dependencies that you statically link with here ...
                "GDAL",
				"UnrealGDAL",
                "GeoReference",
                "OSMDataAssets"
            }
            );


            DynamicallyLoadedModuleNames.AddRange(
    			new string[]
    			{
    				// ... add any modules that your module loads dynamically here ...
                    "AssetTools",
    				"MainFrame"
    			});

    	    PrivateIncludePathModuleNames.AddRange(
    			new string[] {
    				"AssetTools",
    				"UnrealEd"
    			});
	}
}
