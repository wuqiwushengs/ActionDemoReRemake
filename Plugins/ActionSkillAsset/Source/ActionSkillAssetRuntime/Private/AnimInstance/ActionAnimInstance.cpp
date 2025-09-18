// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/ActionAnimInstance.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GamePlay/Character/ActionPlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"

void UActionAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UActionAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);
}

void UActionAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	ActionLocationInfo(DeltaSeconds);
	AcclerationInfo(DeltaSeconds);
	RotationInfo(DeltaSeconds);	
}

UCharacterMovementComponent* UActionAnimInstance::GetCharacterMovementComponent()
{
	USkeletalMeshComponent *SkeletalMeshComponent=GetSkelMeshComponent();
	ACharacter *  Character=Cast<ACharacter>(SkeletalMeshComponent->GetOwner());
	if (Character)
	{
		UCharacterMovementComponent * CharacterMovementComponent=Character->GetCharacterMovement();
		return CharacterMovementComponent;
	}
	return nullptr;
}
FGameplayTagContainer UActionAnimInstance::GetGameplayAbilityTag()
{
	AActionPlayerCharacter * Character=Cast<AActionPlayerCharacter>(TryGetPawnOwner());
	if (Character)
	{
		return Character->GetAbilitySystemComponent()->GetOwnedGameplayTags();
	}
	return  FGameplayTagContainer();
}

void UActionAnimInstance::ActionLocationInfo(float DeltaTime)
{
	LastVelocity=CurrentVelocity;
	if (GetCharacterMovementComponent())
	{
		CurrentVelocity=GetCharacterMovementComponent()->GetLastUpdateVelocity();
	}
	LastActorLocation=CurrentActorLocation;
	if (GetCharacterMovementComponent())
	{
		CurrentActorLocation=GetCharacterMovementComponent()->GetLastUpdateLocation();
	}
	DistanceOffsetInDelta=FVector::DistXY(CurrentActorLocation,LastActorLocation);
	Speed=DistanceOffsetInDelta/DeltaTime;
	
	
}

void UActionAnimInstance::AcclerationInfo(float DeltaTime)
{
	LastAcceleration=CurrentAcceleration;
	if (GetCharacterMovementComponent())
	{
		CurrentAcceleration=GetCharacterMovementComponent()->GetCurrentAcceleration();
	}
	float DotProduct=FVector::DotProduct(CurrentVelocity,CurrentAcceleration);
	float AngleRadians=FMath::Acos(FMath::Clamp(DotProduct,-1.0f,1.0f));
	AccelerationDeltaAngle=FMath::RadiansToDegrees(AngleRadians);
	BisRunningToTheWall=CurrentAcceleration.Length()!=0 && CurrentVelocity.Length()<100 && CurrentAcceleration.GetSafeNormal2D().Dot(CurrentVelocity.GetSafeNormal2D())>=-0.6 &&CurrentAcceleration.GetSafeNormal2D().Dot(CurrentVelocity.GetSafeNormal2D())<=1;
}

void UActionAnimInstance::RotationInfo(float DeltaTime)
{
	LastRotation=CurrentRotation;
	if (GetCharacterMovementComponent())
	{
		CurrentRotation=GetCharacterMovementComponent()->GetLastUpdateRotation();
	}
}
