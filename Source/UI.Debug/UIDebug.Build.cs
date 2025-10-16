using UnrealBuildTool;

public class UIDebug : ModuleRules
{
    public UIDebug(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "UMG",
            "Slate",
            "SlateCore",
            "InputCore",
            "WorldGen",
            "SimAstro"
        });
    }
}
