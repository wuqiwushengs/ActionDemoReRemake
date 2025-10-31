using UnrealBuildTool;

public class ActionCameraSystemAsset : ModuleRules
{
    public ActionCameraSystemAsset(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core","CinematicCamera",
                "TemplateSequence","MovieScene","MovieSceneTracks","LevelSequence"
                ,"TimeManagement","CoreUObject",
                "Engine",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                
                "Slate",
                "SlateCore",
                "InputCore",
                "EnhancedInput",
                "GameplayTags",
                "GameplayAbilities","GameplayTasks",
                "UMG",
                "DeveloperSettings", 
            }
        );
    }
}