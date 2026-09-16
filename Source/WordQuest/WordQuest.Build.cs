using UnrealBuildTool;

public class WordQuest : ModuleRules
{
    public WordQuest(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Keep only modules actually used by Word Quest. The project now uses
        // standard input and UMG/Slate artwork, so EnhancedInput and Paper2D are
        // no longer needed in the shipping build.
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "UMG",
            "Slate",
            "SlateCore"
        });
    }
}
