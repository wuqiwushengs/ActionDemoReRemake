using UnrealBuildTool;

public class ActionSkillAssetEditor : ModuleRules
{
    public ActionSkillAssetEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
               
                "Slate",
                "ToolWidgets",
                "SlateCore",
                "UnrealEd",
                "PropertyEditor",
                "EditorStyle",
                "InputCore",
                "Projects",
                "AssetDefinition",
                "StateTreeModule",
                "StateTreeEditorModule",
                "ApplicationCore",
                "DataValidation", 
                "PropertyEditor",
                "ActionSkillAssetRuntime"
            }
        );
    }
}