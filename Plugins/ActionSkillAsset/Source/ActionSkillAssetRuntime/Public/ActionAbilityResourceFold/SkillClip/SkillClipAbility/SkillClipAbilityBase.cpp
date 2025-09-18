// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillClipAbilityBase.h"

#include "AbilitySystemFold/AbilitySystemCompoentRef/ActionAbilitySystemComponent.h"
#include "ActionAbilityResourceFold/SkillExecutor.h"
#include "ActionAbilityResourceFold/SkillManager.h"

void USkillClipAbilityBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle,ActorInfo,ActivationInfo))
	{
		Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
		Executor=Cast<UActionAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo())->GetSkillManager()->SelectedSkillExecutorConfig->ExecutorDescriptor.Executor;
		if (Executor)
		{
			AbilityEvent.BindDynamic(Executor,&USkillExecutor::OnClipEnd);
		}
		PreEntryAbility();
		OnEntryAbility();
	}
}
ESkillCLipType USkillClipAbilityBase::GetSkillClipType()
{
	return  ESkillCLipType::Ability;
}
void USkillClipAbilityBase::PreEntryAbility_Implementation()
{
}
void USkillClipAbilityBase::OnEntryAbility_Implementation()
{
	UE_LOG(LogTemp,Warning,TEXT("%s SkillClipMontagePlay"),*GetName());
}
void USkillClipAbilityBase::OnEndAbility_Implementation()
{
	EndAbility(GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetCurrentActivationInfo(),true,false);
}
void USkillClipAbilityBase::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (bIsExiting)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is already exiting, skipping..."), *GetName());
		return;
	}
	bIsExiting = true;
	UE_LOG(LogTemp,Warning,TEXT("%s SkillClipMontageEnd"),*GetName());
	if (!AbilityEvent.ExecuteIfBound())
	{
		UE_LOG(LogTemp,Warning,TEXT("%s don't bind End Event"),*GetName());
	}
	AbilityEvent.Clear();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

FEndAbilityEvent& USkillClipAbilityBase::GetAbilitySignal()
{
	return AbilityEvent;
}

float USkillClipAbilityBase::GetHoldMinTime()
{
	return Executor->HoldSkill.HoldSkillInfo.MinHoldTime;
}

float USkillClipAbilityBase::GetCurrentHoldTime()
{
	return Executor->HoldSkill.HoldSkillInfo.CurrentHoldTime;
}


