// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionDemoReRemake/Public/Gameplay/XActionPlayController.h"

#include "ActionDemoReRemake/Public/ActionGameplayTags.h"
#include "GamePlay/Character/ActionPlayerCharacter.h"
#include "InputFold/EnhancedInput/ActionInputComponent.h"

void AXActionPlayController::CustomNormalInputBinding(UEnhancedInputLocalPlayerSubsystem *InputSubsystem,UActionInputComponent * ActionInputComponent ,AActionPlayerCharacter* RefCharacter)
{
	//NativeSkillBind
	ActionInputComponent->BindAction(InputDataAsset->GetNativeInputActionByTag(ActionGameplayTags::Move),ETriggerEvent::Triggered,RefCharacter,&AActionPlayerCharacter::OnInputMove);
	ActionInputComponent->BindAction(InputDataAsset->GetNativeInputActionByTag(ActionGameplayTags::Look),ETriggerEvent::Triggered,RefCharacter,&AActionPlayerCharacter::OnInputLook);
}
