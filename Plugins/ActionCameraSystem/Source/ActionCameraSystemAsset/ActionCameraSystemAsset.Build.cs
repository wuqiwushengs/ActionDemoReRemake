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
                "CinematicCamera",
                "GameplayTags",
                "GameplayAbilities","GameplayTasks",
                "UMG",
                "DeveloperSettings"
            }
        );
    }
}