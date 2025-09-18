// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ExecutorCondition.generated.h"

class USkillManager;
class UActionAbilitySystemComponent;
class AActionPlayerCharacter;
/**
 * 
 */
UCLASS(Blueprintable,EditInlineNew)
class ACTIONSKILLASSETRUNTIME_API UExecutorCondition : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	bool CanTransition(const AActionPlayerCharacter * PLayer,const  UActionAbilitySystemComponent *AbilitySystemComponent,const  USkillManager * Manager);
	bool CanTransition_Implementation(const AActionPlayerCharacter * PLayer,const  UActionAbilitySystemComponent *AbilitySystemComponent,const  USkillManager * Manager);
};
