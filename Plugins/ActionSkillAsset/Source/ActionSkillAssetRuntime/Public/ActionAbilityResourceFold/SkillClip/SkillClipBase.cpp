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
	UE_LOG(LogTemp,Warning,TEXT("%s SkillClipMontagePlay"),*GetName());
}
void  USkillClipBase::OnExitThisClip_Implementation()
{
	if (bIsExiting)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is already exiting, skipping..."), *GetName());
		return;
	}
	bIsExiting = true;
	if (!AbilityEvent.ExecuteIfBound())
	{
		UE_LOG(LogTemp,Warning,TEXT("%s don't bind End Event"),*GetName());
		AbilityEvent.Clear();
	}

}
