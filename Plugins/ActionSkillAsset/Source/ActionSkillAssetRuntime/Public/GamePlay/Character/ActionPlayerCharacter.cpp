// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionPlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "InputActionValue.h"
#include "AbilitySystemFold/AbilitySystemCompoentRef/ActionAbilitySystemComponent.h"
#include "GamePlay/ActionPlayController.h"
#include "InputFold/EnhancedInput/ActionInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"



AActionPlayerCharacter::AActionPlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ActionAbilitySystemComponent=CreateDefaultSubobject<UActionAbilitySystemComponent>("ActionAbilitySystemComponent");
	SpringArmComponent=CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	CameraComponent=CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	//TODO::临时使用后面对摄像机实行自定义方法。
	SpringArmComponent->SetupAttachment(GetRootComponent());
	CameraComponent->SetupAttachment(SpringArmComponent);
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
{
	MovementInputValue=InputActionValue.Get<FVector2D>();
	FRotator ControllRotation=GetControlRotation();
	FVector ForwardVector=UKismetMathLibrary::GetForwardVector(ControllRotation);
	ForwardVector.Normalize();
	FVector RightVector=UKismetMathLibrary::GetRightVector(ControllRotation);
	RightVector.Normalize();
	AddMovementInput(ForwardVector,MovementInputValue.X);
	AddMovementInput(RightVector,MovementInputValue.Y);
}

void AActionPlayerCharacter::OnInputLook(const FInputActionValue&  InputActionValue)
{
	FVector2d InputValue=InputActionValue.Get<FVector2d>();
	AddControllerYawInput(InputValue.X*0.5);
	AddControllerPitchInput(InputValue.Y*0.5);
}

void AActionPlayerCharacter::PrintHello(const FInputActionInstance&  InputActionValue)
{
	UEnum * Enum=StaticEnum<ETriggerEvent>();
	FString DebugName=Enum->GetNameStringByValue(static_cast<int64>(InputActionValue.GetTriggerEvent()));
	GEngine->AddOnScreenDebugMessage(-1,1,FColor::Red,DebugName);
	
	
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

