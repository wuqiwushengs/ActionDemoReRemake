using UnrealBuildTool;

public class ActionCameraSystemEditor : ModuleRules
{
    public ActionCameraSystemEditor(ReadOnlyTargetRules Target) : base(Target)
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
                "UMG",
                "DeveloperSettings"
            }
        );
    }
}