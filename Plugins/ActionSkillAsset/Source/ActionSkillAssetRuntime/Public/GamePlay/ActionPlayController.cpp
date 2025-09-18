// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionPlayController.h"

#include "EnhancedInputSubsystems.h"
#include "GamePlayTag/GamePlayTags.h"
#include "Character/ActionPlayerCharacter.h"
#include "GamePlayTag/GamePlayTags.h"
#include "InputFold/EnhancedInput/ActionInputComponent.h"


void AActionPlayController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	//注册输入事件
	RegisterInputAction(InPawn);
}

void AActionPlayController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent)
	{
		InputComponent->DestroyComponent();
		InputComponent = nullptr;
	}
	// 创建并注册你自己的组件
	UActionInputComponent* MyInputComponent = NewObject<UActionInputComponent>(this, UActionInputComponent::StaticClass());
	MyInputComponent->RegisterComponent(); // 注册到引擎
	InputComponent = MyInputComponent;
}

UInputDataAsset* AActionPlayController::GetInputDataAsset()
{
	return InputDataAsset;
}

void AActionPlayController::RegisterInputAction(APawn * InPawn)
{
	UEnhancedInputLocalPlayerSubsystem * EnhancedInputSubsystem =ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(this->GetLocalPlayer());
	UActionInputComponent * ActionInputComponent=CastChecked<UActionInputComponent>(InputComponent);
	EnhancedInputSubsystem->AddMappingContext(NormalMappingContext,1);
	AActionPlayerCharacter * RefCharacter=Cast<AActionPlayerCharacter>(InPawn);
	//NativeSkillBind
	ActionInputComponent->BindAction(InputDataAsset->GetNativeInputActionByTag(GamePlayTags::Move),ETriggerEvent::Triggered,RefCharacter,&AActionPlayerCharacter::OnInputMove);
	ActionInputComponent->BindAction(InputDataAsset->GetNativeInputActionByTag(GamePlayTags::Look),ETriggerEvent::Triggered,RefCharacter,&AActionPlayerCharacter::OnInputLook);
	//AbilitySkillBind
	EnhancedInputSubsystem->AddMappingContext(AbilityMappingContext,0);
	ActionInputComponent->BindAbilityByFunction(InputDataAsset->AbilityInputData,RefCharacter,&AActionPlayerCharacter::OnAbilityInputTrigger);
}
