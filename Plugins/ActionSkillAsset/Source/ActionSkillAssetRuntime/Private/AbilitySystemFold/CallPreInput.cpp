// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystemFold/CallPreInput.h"
#include "AbilitySystemFold/AbilitySystemCompoentRef/ActionAbilitySystemComponent.h"
#include "GamePlay/Character/ActionPlayerCharacter.h"

void UCallPreInput::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	AActionPlayerCharacter * Character=Cast<AActionPlayerCharacter>(MeshComp->GetOwner());
	if (!Character)return;
	Character->GetActionAbilitySystemComponent()->SetCurrentInputState(EInputState::PreInputState);
	Character->GetActionAbilitySystemComponent()->SetPreInputDisable(DisablePreTag);
	
}

void UCallPreInput::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	AActionPlayerCharacter * Character=Cast<AActionPlayerCharacter>(MeshComp->GetOwner());
	if(!Character) return;
	Character->GetActionAbilitySystemComponent()->TurnPreInputToDefault();
	
}
