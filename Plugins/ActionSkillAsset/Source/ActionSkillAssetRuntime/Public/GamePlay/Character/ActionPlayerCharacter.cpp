// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionPlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "InputActionValue.h"
#include "AbilitySystemFold/AbilitySystemCompoentRef/ActionAbilitySystemComponent.h"
#include "AnimInstance/PostAnimPlayedNotify.h"
#include "GamePlay/ActionPlayController.h"
#include "InputFold/EnhancedInput/ActionInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GamePlayTag/GamePlayTags.h"
#include "Kismet/KismetMathLibrary.h"



AActionPlayerCharacter::AActionPlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ActionAbilitySystemComponent=CreateDefaultSubobject<UActionAbilitySystemComponent>("ActionAbilitySystemComponent");
	AttackCollisionComponent=CreateDefaultSubobject<UAttackCollisionComponent>("AttackCollisionCompoent");
	//TODO::临时使用后面对摄像机实行自定义方法。
}

UAbilitySystemComponent* AActionPlayerCharacter::GetAbilitySystemComponent() const
{
	return Cast<UAbilitySystemComponent>(ActionAbilitySystemComponent);
}

UActionAbilitySystemComponent* AActionPlayerCharacter::GetActionAbilitySystemComponent() const
{
	return ActionAbilitySystemComponent;
}

UInputDataAsset* AActionPlayerCharacter::GetInputDataAsset() const
{
	 
	return PlayController?PlayController->GetInputDataAsset():nullptr;
}

FCharacterNormalInputData AActionPlayerCharacter::GetCharacterNormalInputData()
{
	return CharacterNormalInputData;
}

UAttackCollisionComponent* AActionPlayerCharacter::GetAttackCollisionComponent_Implementation()
{
	return AttackCollisionComponent;
}


// Called when the game starts or when spawned
void AActionPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayController=Cast<AActionPlayController>(GetLocalViewingPlayerController());

}


// Called every frame
void AActionPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
#pragma  region NormalInputFunc
void AActionPlayerCharacter::OnInputMove(const FInputActionValue&  InputActionValue)
{	CharacterNormalInputData.MoveInputValue=InputActionValue.Get<FVector2D>();
	CheckPostAnimPlayAndStop();
	ProcessMove(InputActionValue);
}

void AActionPlayerCharacter::ProcessMove(const FInputActionValue& InputActionValue)
{
	FRotator ControllRotation=GetControlRotation();
	FVector ForwardVector=UKismetMathLibrary::GetForwardVector(ControllRotation);
	ForwardVector.Normalize();
	FVector RightVector=UKismetMathLibrary::GetRightVector(ControllRotation);
	RightVector.Normalize();
	AddMovementInput(ForwardVector,CharacterNormalInputData.MoveInputValue.X);
	AddMovementInput(RightVector,CharacterNormalInputData.MoveInputValue.Y);
}

void AActionPlayerCharacter::OnInputLook(const FInputActionValue&  InputActionValue)
{
	CharacterNormalInputData.LookInputValue=InputActionValue.Get<FVector2d>();
	ProcessLook(InputActionValue);
}

void AActionPlayerCharacter::ProcessLook(const FInputActionValue& InputActionValue)
{
	AddControllerYawInput(CharacterNormalInputData.LookInputValue.X*0.5);
	AddControllerPitchInput(CharacterNormalInputData.LookInputValue.Y*0.5);
}

void AActionPlayerCharacter::CheckPostAnimPlayAndStop()
{
	if(GetActionAbilitySystemComponent()->GetOwnedGameplayTags().HasTag(GamePlayTags::PostAnim))
	{
		GetMesh()->GetAnimInstance()->Montage_StopGroupByName(0,AttackSlotGroupName);
		GetActionAbilitySystemComponent()->RemoveLooseGameplayTag(GamePlayTags::PostAnim);
	}
}

#pragma endregion
// Called to bind functionality to input
void AActionPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void AActionPlayerCharacter::OnAbilityInputTrigger(const FInputActionInstance& InputInfo, FGameplayTag InputData,ETriggerEvent TriggerEvent)
{
	UE_LOG(LogTemp,Warning,TEXT("%s"),*InputData.ToString());
	if (TriggerEvent==ETriggerEvent::Started ||InputInfo.GetTriggerEvent()==ETriggerEvent::Completed)
	{
		ActionAbilitySystemComponent->AbilityInputDataLocalProcessing(InputInfo,InputData,GetInputDataAsset(),TriggerEvent);
	}
	
}

