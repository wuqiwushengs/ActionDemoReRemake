// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAbilityResourceFold/SkillExecutorFold/ExecutorCondition.h"

bool UExecutorCondition::CanTransition_Implementation(const AActionPlayerCharacter* PLayer,
	const UActionAbilitySystemComponent* AbilitySystemComponent, const USkillManager* Manager)
{
	return true;
}
