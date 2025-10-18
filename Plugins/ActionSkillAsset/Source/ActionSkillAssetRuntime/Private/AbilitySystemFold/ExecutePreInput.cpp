// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystemFold/ExecutePreInput.h"

#include "AbilitySystemFold/AbilitySystemCompoentRef/ActionAbilitySystemComponent.h"
#include "GamePlay/Character/ActionPlayerCharacter.h"
#include "GamePlayTag/GamePlayTags.h"

void UExecutePreInput::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                   const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	AActionPlayerCharacter * Character=Cast<AActionPlayerCharacter>(MeshComp->GetOwner());
	if(!Character) return;
 	Character->GetActionAbilitySystemComponent()->AddLooseGameplayTag(GamePlayTags::ExecutePreInput);
	
}

void UExecutePreInput::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	AActionPlayerCharacter * Character=Cast<AActionPlayerCharacter>(MeshComp->GetOwner());
	if(!Character) return;
	Character->GetActionAbilitySystemComponent()->RemoveLooseGameplayTag(GamePlayTags::ExecutePreInput);
}
