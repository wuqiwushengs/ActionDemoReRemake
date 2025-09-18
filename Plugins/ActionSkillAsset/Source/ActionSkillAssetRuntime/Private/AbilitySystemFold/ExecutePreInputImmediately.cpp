// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystemFold/ExecutePreInputImmediately.h"
#include "AbilitySystemFold/AbilitySystemCompoentRef/ActionAbilitySystemComponent.h"
#include "GamePlay/Character/ActionPlayerCharacter.h"
#include "GamePlayTag/GamePlayTags.h"

void UExecutePreInputImmediately::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                              float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	AActionPlayerCharacter * Character=Cast<AActionPlayerCharacter>(MeshComp->GetOwner());
	if(!Character) return;
	Character->GetActionAbilitySystemComponent()->AddLooseGameplayTag(GamePlayTags::ExecutePreInputImmediately);
}

void UExecutePreInputImmediately::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	AActionPlayerCharacter * Character=Cast<AActionPlayerCharacter>(MeshComp->GetOwner());
	if(!Character) return;
	Character->GetActionAbilitySystemComponent()->RemoveLooseGameplayTag(GamePlayTags::ExecutePreInputImmediately);
}
