using UnrealBuildTool;

public class DevTools : ModuleRules
{
    public DevTools(ReadOnlyTargetRules Target) : base(Target)
    {
        bool bIsShipping = Target.Configuration == UnrealTargetConfiguration.Shipping;
        PrivateDefinitions.Add(bIsShipping ? "WITH_DEV_TOOLS=0" : "WITH_DEV_TOOLS=1");

        if (bIsShipping)
        {
            bBuild = false;
            return;
        }

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "UMG",
            "Slate",
            "SlateCore",
            "DeveloperSettings",
            "Projects",
            "WorldGen",
            "SimAstro",
            "RPGData",
            "UIDebug"
        });

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "ApplicationCore",
            "Json",
            "JsonUtilities"
        });
    }
}
