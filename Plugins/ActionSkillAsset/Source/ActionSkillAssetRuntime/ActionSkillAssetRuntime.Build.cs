using UnrealBuildTool;

public class ActionSkillAssetRuntime : ModuleRules
{
    public ActionSkillAssetRuntime(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "CoreUObject", "Engine", 
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