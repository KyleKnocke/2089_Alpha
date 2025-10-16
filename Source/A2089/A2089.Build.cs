using UnrealBuildTool;

public class A2089 : ModuleRules
{
    public A2089(ReadOnlyTargetRules Target) : base(Target)
    {
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
            "SimAstro",
            "WorldGen",
            "RPGData",
            "UIDebug"
        });
    }
}
