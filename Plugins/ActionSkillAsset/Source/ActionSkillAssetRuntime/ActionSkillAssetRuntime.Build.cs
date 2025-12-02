using UnrealBuildTool;

public class ActionSkillAssetRuntime : ModuleRules
{
    public ActionSkillAssetRuntime(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "CoreUObject", "Engine","AIModule","MotionWarping","ActionAttributeSystemRuntime"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Slate",
                "SlateCore","StateTreeModule","GameplayStateTreeModule","InputCore", "EnhancedInput","GameplayAbilities","GameplayTags","GameplayTasks",
                
            }
        );
    }
}