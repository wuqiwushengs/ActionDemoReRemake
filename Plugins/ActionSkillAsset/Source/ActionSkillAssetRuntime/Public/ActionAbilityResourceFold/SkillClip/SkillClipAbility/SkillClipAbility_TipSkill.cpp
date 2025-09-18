// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillClipAbility_TipSkill.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

void USkillClipAbility_TipSkill::PreEntryAbility_Implementation()
{
	TipMontage=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,TipAnimMontage);
	TipMontage->OnCompleted.AddDynamic(this,&USkillClipAbility_TipSkill::OnEndAbility);
	TipMontage->OnInterrupted.AddDynamic(this,&USkillClipAbility_TipSkill::OnEndAbility);
	TipMontage->OnBlendOut.AddDynamic(this,&USkillClipAbility_TipSkill::OnEndAbility);
	TipMontage->Activate();
}
