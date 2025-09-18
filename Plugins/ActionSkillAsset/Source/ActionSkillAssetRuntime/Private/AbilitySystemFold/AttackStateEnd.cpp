// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystemFold/AttackStateEnd.h"

#include "AbilitySystemFold/AbilitySystemCompoentRef/ActionAbilitySystemComponent.h"
#include "GamePlay/Character/ActionPlayerCharacter.h"

void UAttackStateEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                             const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	AActionPlayerCharacter * Character=Cast<AActionPlayerCharacter>(MeshComp->GetOwner());
	if(!Character) return ;
	Character->GetActionAbilitySystemComponent()->SetCurrentInputState(EInputState::NormalInputState);
}
