// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillClipBase.h"
#include "ActionAbilityResourceFold/SkillExecutor.h"

USkillClipBase::USkillClipBase()
{
}

void USkillClipBase::Initialize(UActionAbilitySystemComponent* InAbilitySystemComponent, USkillExecutor* InExecutor)
{
	this->AbilitySystemComponent=InAbilitySystemComponent;
	this->Executor=InExecutor;
}

UActionAbilitySystemComponent* USkillClipBase::GetAbilitySystemComponent()
{
	return AbilitySystemComponent;
}

ESkillCLipType USkillClipBase::GetSkillClipType()
{
	return  ESkillCLipType::NormalSkillClip;
}

FEndAbilityEvent & USkillClipBase::GetAbilitySignal()
{
	return AbilityEvent;
}

void USkillClipBase::OnEnterThisClip_Implementation()
{
}
void  USkillClipBase::OnExitThisClip_Implementation()
{
	if (bIsExiting)
	{
		return;
	}
	bIsExiting = true;
	if (!AbilityEvent.ExecuteIfBound())
	{
		AbilityEvent.Clear();
	}

}
