// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillClipAbility_HoldSkill.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "ActionAbilityResourceFold/SkillExecutor.h"

USkillClipAbility_HoldSkill::USkillClipAbility_HoldSkill()
{	//绑定播放规则
	AnimPlayedRules.MontageRule=[this]()->UAnimMontage* {return HoldMontagePlayedRule();};
}

UAnimMontage* USkillClipAbility_HoldSkill::HoldMontagePlayedRule_Implementation()
{
	//TODO::Hold规则的内部实现
	
	return AnimPlayedRules.PlayedMontages[0];
}

void USkillClipAbility_HoldSkill::PreEntryAbility_Implementation()
{
	Super::PreEntryAbility_Implementation();
	HoldTime=Executor->HoldSkill.HoldSkillInfo.CurrentHoldTime;
	if(PlayMontage= AnimPlayedRules.MontageRule();PlayMontage)
	{
		HoldMontagePlayTask=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,PlayMontage);
		HoldMontagePlayTask->OnCompleted.AddDynamic(this,&USkillClipAbility_HoldSkill::OnEndAbility);
		HoldMontagePlayTask->OnInterrupted.AddDynamic(this,&USkillClipAbility_HoldSkill::OnEndAbility);
		HoldMontagePlayTask->OnBlendOut.AddDynamic(this,&USkillClipAbility_HoldSkill::OnEndAbility);
		HoldMontagePlayTask->Activate();
	}
	else UE_LOG(LogTemp,Warning,TEXT("Don't find a correct Anim"));
}

UAnimMontage* USkillClipAbility_HoldSkill::GetSelectedAnimMontage()
{
	return  PlayMontage;
}

int32 USkillClipAbility_HoldSkill::GetSelectedAnimMontageIndex()
{
	return AnimPlayedRules.PlayedMontages.Find(PlayMontage);
}
