// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetDefinition.h"
#include "SkillExecutorConfigDefintion.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONSKILLASSETEDITOR_API USkillExecutorConfigDefintion : public UAssetDefinition
{
	GENERATED_BODY()
public:
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override;
	virtual FLinearColor GetAssetColor() const override;
	virtual FText GetAssetDisplayName() const override;
	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
	virtual EAssetCommandResult OpenAssets(const FAssetOpenArgs& OpenArgs) const override;
};
