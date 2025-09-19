// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/PostAnimPlayedNotify.h"

#include "AbilitySystemComponent.h"
#include "GamePlay/Character/ActionPlayerCharacter.h"
#include "GamePlayTag/GamePlayTags.h"

void UPostAnimPlayedNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                        float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	AActionPlayerCharacter * Character=Cast<AActionPlayerCharacter>(MeshComp->GetOwner());
	if(!Character) return;
	Character->GetAbilitySystemComponent()->AddLooseGameplayTag(GamePlayTags::PostAnim);	
}

void UPostAnimPlayedNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	AActionPlayerCharacter * Character=Cast<AActionPlayerCharacter>(MeshComp->GetOwner());
	if(!Character) return;
	Character->GetAbilitySystemComponent()->RemoveLooseGameplayTag(GamePlayTags::PostAnim);
}
	
