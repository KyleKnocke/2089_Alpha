using UnrealBuildTool;

public class RPGData : ModuleRules
{
    public RPGData(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "DataRegistry"
        });
    }
}
