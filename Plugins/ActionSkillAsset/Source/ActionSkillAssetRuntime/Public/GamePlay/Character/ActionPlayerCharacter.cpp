// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionPlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "ActionAttackCollisionComponent.h"
#include "ActionGlobalAttributeSet.h"
#include "InputActionValue.h"
#include "AbilitySystemFold/AbilitySystemCompoentRef/ActionAbilitySystemComponent.h"
#include "AnimInstance/PostAnimPlayedNotify.h"
#include "GamePlay/ActionPlayController.h"
#include "InputFold/EnhancedInput/ActionInputComponent.h"
#include "EnemyFollow/Component/ActionEnemyFollowComponent.h"
#include "MotionWarpingComponent.h"
#include "GamePlayTag/GamePlayTags.h"
#include "Kismet/KismetMathLibrary.h"



AActionPlayerCharacter::AActionPlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ActionAbilitySystemComponent=CreateDefaultSubobject<UActionAbilitySystemComponent>("ActionAbilitySystemComponent");
	AttackCollisionComponent=CreateDefaultSubobject<UActionAttackCollisionComponent>("AttackCollisionComponent");
	EnemyFollowComponent=CreateDefaultSubobject<UActionEnemyFollowComponent>("ActionEnemyFollowComponent");
	MotionWarpingComponent=CreateDefaultSubobject<UMotionWarpingComponent>("MotionWarpingComponent");
	CharacterNormalAttribute=CreateDefaultSubobject<UActionGlobalAttributeSet>("CharacterAttribute");
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

void AActionPlayerCharacter::OnBeAttackRecallInternal_Implementation(FAttackedResult AttackedResult)
{
	UE_LOG(LogTemp,Warning,TEXT("I Was Attacked"));
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
	//TODO:: 摄像机限制了但是没有限制控制器导致pitch轴会有时候对不齐。导致摄像机正常但是人走不动
	AddControllerYawInput(CharacterNormalInputData.LookInputValue.X*0.5);
	AddControllerPitchInput(CharacterNormalInputData.LookInputValue.Y*0.5);
}

void AActionPlayerCharacter::CheckPostAnimPlayAndStop()
{
	if(GetActionAbilitySystemComponent()->GetOwnedGameplayTags().HasTag(GamePlayTags::PostAnim))
	{
		GetMesh()->GetAnimInstance()->Montage_StopGroupByName(0,AttackSlotGroupName);
		GetActionAbilitySystemComponent()->RemoveLooseGameplayTag(GamePlayTags::PostAnim);
		UE_LOG(LogTemp,Warning,TEXT("StopMontageUseNotify"))
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
	if (TriggerEvent==ETriggerEvent::Started ||InputInfo.GetTriggerEvent()==ETriggerEvent::Completed)
	{
		ActionAbilitySystemComponent->AbilityInputDataLocalProcessing(InputInfo,InputData,GetInputDataAsset(),TriggerEvent);
	}
	
}

