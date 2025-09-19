// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillClipAbility_TipSkill.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

USkillClipAbility_TipSkill::USkillClipAbility_TipSkill()
{
	AnimPlayedRules.MontageRule=[this]()->UAnimMontage* {return TipMontagePlayedRule();};
}

void USkillClipAbility_TipSkill::PreEntryAbility_Implementation()
{
	if(PlayAnimMontage=AnimPlayedRules.MontageRule();PlayAnimMontage)
	{
		TipMontage=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,PlayAnimMontage);
		TipMontage->OnCompleted.AddDynamic(this,&USkillClipAbility_TipSkill::OnEndAbility);
		TipMontage->OnInterrupted.AddDynamic(this,&USkillClipAbility_TipSkill::OnEndAbility);
		TipMontage->OnBlendOut.AddDynamic(this,&USkillClipAbility_TipSkill::OnEndAbility);
		TipMontage->Activate();
		return;
	}
	OnEndAbility_Implementation();
}

UAnimMontage* USkillClipAbility_TipSkill::TipMontagePlayedRule_Implementation()
{
	if(AnimPlayedRules.PlayedMontages.Num()>0) return AnimPlayedRules.PlayedMontages[0]; 
	return nullptr;
}
