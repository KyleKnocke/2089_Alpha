using UnrealBuildTool;

public class WorldGen : ModuleRules
{
    public WorldGen(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "DeveloperSettings",
            "SimAstro",
            "RPGData"
        });
    }
}
