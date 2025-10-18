// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillClipAbility_MultiTipSkill.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GamePlayTag/GamePlayTags.h"

USkillClipAbility_MultiTipSkill::USkillClipAbility_MultiTipSkill()
{
	AnimPlayedRules.MontageRule=[this]()->UAnimMontage* {return MultiTipMontagePlayedRule();};
}

void USkillClipAbility_MultiTipSkill::PreEntryAbility_Implementation()
{
	Super::PreEntryAbility_Implementation();
	if(PlayMontage= AnimPlayedRules.MontageRule();PlayMontage)
	{
		HoldMontagePlayTask=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,PlayMontage);
		HoldMontagePlayTask->OnCompleted.AddDynamic(this,&USkillClipAbility_MultiTipSkill::OnEndAbility);
		HoldMontagePlayTask->Activate();
		CurrenPlayTime++;
		MultiTipEvent=UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,GamePlayTags::MultiTipSign);
		MultiTipEvent->EventReceived.AddDynamic(this,&USkillClipAbility_MultiTipSkill::AddTipCount);
		MultiTipEvent->Activate();
		
	}
}
UAnimMontage* USkillClipAbility_MultiTipSkill::MultiTipMontagePlayedRule_Implementation()
{
	//TODO::MultiTip规则的内部实现
	return AnimPlayedRules.PlayedMontages[0];
}

UAnimMontage* USkillClipAbility_MultiTipSkill::GetSelectedAnimMontage()
{
	return  PlayMontage;
}
int32 USkillClipAbility_MultiTipSkill::GetSelectedAnimMontageIndex()
{
	return AnimPlayedRules.PlayedMontages.Find(PlayMontage);
}

void USkillClipAbility_MultiTipSkill::AddTipCount(FGameplayEventData  Payload)
{
	TipCount++;
}
bool USkillClipAbility_MultiTipSkill::ChoosePlayAgain_orEndAbility()
{
	if(PlayMontage= AnimPlayedRules.MontageRule();TipCount>=MinTipIndex && PlayMontage && CurrenPlayTime<MaxPlayTime)
	{
		// 清理旧绑定
		HoldMontagePlayTask->OnCompleted.Clear(); 
		HoldMontagePlayTask->EndTask();
		HoldMontagePlayTask = nullptr;
		//建立新绑定
		HoldMontagePlayTask=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,PlayMontage);
		HoldMontagePlayTask->OnCompleted.AddDynamic(this,&USkillClipAbility_MultiTipSkill::OnEndAbility);
		HoldMontagePlayTask->Activate();
		TipCount=0;
		CurrenPlayTime++;
		return true;
	}
	return  false;
}

void USkillClipAbility_MultiTipSkill::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if(ChoosePlayAgain_orEndAbility()) return;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	//清理事件绑定避免内存泄漏
	if(MultiTipEvent)
	{
		MultiTipEvent->EventReceived.Clear();
		MultiTipEvent->EndTask();
		MultiTipEvent = nullptr;
	}
	
}
