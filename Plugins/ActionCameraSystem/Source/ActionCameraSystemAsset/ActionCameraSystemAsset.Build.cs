using UnrealBuildTool;

public class ActionCameraSystemAsset : ModuleRules
{
    public ActionCameraSystemAsset(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CinematicCamera",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "InputCore",
                "EnhancedInput",
                "GameplayTags",
                "GameplayAbilities","GameplayTasks",
                "UMG",
                "DeveloperSettings", "TemplateSequence"
            }
        );
    }
}