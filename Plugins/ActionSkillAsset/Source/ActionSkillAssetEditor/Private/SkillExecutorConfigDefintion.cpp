// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillExecutorConfigDefintion.h"

#include "ActionSkillAssetRuntime/Public/ActionAbilityResourceFold/SkillExecutorFold/SkillExecutorDescriptor.h"
#include "SkillExecutorConfigAssetEditorFold/FSkillExecutorConfigEditor.h"
#define LOCTEXT_NAMESPACE "SKillExecutor"
FText USkillExecutorConfigDefintion::GetAssetDescription(const FAssetData& AssetData) const
{
	return LOCTEXT("SkillExecutor","SkillAssetTreeForConfig");
}

FLinearColor USkillExecutorConfigDefintion::GetAssetColor() const
{
	return FLinearColor(1.0f,0.843f,0.0f,1.0f);
}

FText USkillExecutorConfigDefintion::GetAssetDisplayName() const
{
	return LOCTEXT("SkillExecutor","SkillExecutorDescriptorTree");
}

TConstArrayView<FAssetCategoryPath> USkillExecutorConfigDefintion::GetAssetCategories() const
{
	static const auto Categories={EAssetCategoryPaths::Gameplay/LOCTEXT("ActionResource","SkillExecutorDescriptorTree")};
	return Categories;
}

TSoftClassPtr<UObject> USkillExecutorConfigDefintion::GetAssetClass() const
{
	return USkillExecutorDescriptorAsset::StaticClass();
}

EAssetCommandResult USkillExecutorConfigDefintion::OpenAssets(const FAssetOpenArgs& OpenArgs) const
{
	const EToolkitMode::Type Mode = OpenArgs.ToolkitHost.IsValid()
										? EToolkitMode::WorldCentric
										: EToolkitMode::Standalone;
	for (auto &OpenArg:OpenArgs.Assets)
	{
		if ( USkillExecutorDescriptorAsset *SkillExecutorConfig=Cast<USkillExecutorDescriptorAsset>(OpenArg.GetAsset()))
		{
			const TSharedPtr<FSkillExecutorConfigEditor> NewEditor=MakeShareable(new FSkillExecutorConfigEditor());
			NewEditor->InitSkillExecutorConfigEditor(Mode,OpenArgs.ToolkitHost,SkillExecutorConfig);
			return EAssetCommandResult::Handled;
		}
	}
	return EAssetCommandResult::Unhandled;

	return EAssetCommandResult::Handled;
}
#undef LOCTEXT_NAMESPACE

