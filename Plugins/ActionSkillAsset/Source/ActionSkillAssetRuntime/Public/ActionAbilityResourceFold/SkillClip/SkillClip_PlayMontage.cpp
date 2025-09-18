// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillClip_PlayMontage.h"

#include "AbilitySystemFold/AbilitySystemCompoentRef/ActionAbilitySystemComponent.h"
#include "ActionAbilityResourceFold/AsyncTask/AsyncTask_PlayMontage.h"
#include "GameFramework/Character.h"

void USkillClip_PlayMontage::OnEnterThisClip_Implementation()
{
	Super::OnEnterThisClip_Implementation();
	AnimPlayedRules.MontageRule=[this]()->UAnimMontage*{return MontagePlayedRule();};
	if(UAnimMontage* PlayMontage= AnimPlayedRules.MontageRule();PlayMontage)
	{
		USkeletalMeshComponent * Mesh=Cast<ACharacter>(GetAbilitySystemComponent()->GetOwnerActor())->GetMesh();
		if (Mesh)
		{
			MontagePlayTask=UAsyncTask_PlayMontage::ActionCreateNewMontage(Mesh,PlayMontage,1,NAME_None,0.0f,true);
			MontagePlayTask->OnMontageBlendOut.AddDynamic(this,&USkillClip_PlayMontage::OnMontageBlendOut);
			MontagePlayTask->OnMontageInterrupt.AddDynamic(this,&USkillClip_PlayMontage::OnMontageInterrupt);
			if (bEndAfterMontageOver)
			{
				MontagePlayTask->OnMontageComplete.AddDynamic(this,&USkillClip_PlayMontage::OnExitThisClip);
			}
			MontagePlayTask->Activate();
		}
	
	}
}

void USkillClip_PlayMontage::OnExitThisClip_Implementation()
{
	Super::OnExitThisClip_Implementation();
}

UAnimMontage* USkillClip_PlayMontage::MontagePlayedRule_Implementation()
{
	return AnimPlayedRules.PlayedMontages[0];
}

void USkillClip_PlayMontage::OnMontageBlendOut_Implementation()
{
	OnExitThisClip_Implementation();
}

void USkillClip_PlayMontage::OnMontageInterrupt_Implementation()
{
	OnExitThisClip_Implementation();
}
