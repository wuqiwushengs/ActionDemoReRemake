// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SkillExecutorEditorData.generated.h"

class USkillExecutorConfig;
/**
 * 
 */
UCLASS()
class ACTIONSKILLASSETEDITOR_API USkillExecutorEditorData : public UObject
{
	GENERATED_BODY()
public:
	USkillExecutorConfig & AddSubTree(const FName Name);
	UPROPERTY()
	TArray<TObjectPtr<USkillExecutorConfig>> SubTrees;
};
