// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ActionAnimInstance.generated.h"

struct FGameplayTagContainer;
class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class ACTIONSKILLASSETRUNTIME_API UActionAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable,meta=(BlueprintThreadSafe),BlueprintPure)
	UCharacterMovementComponent * GetCharacterMovementComponent();
	UFUNCTION(BlueprintCallable,meta=(BlueprintThreadSafe),BlueprintPure)
	FGameplayTagContainer GetGameplayAbilityTag();
	UFUNCTION(BlueprintCallable,meta=(BlueprintThreadSafe))
	void ActionLocationInfo(float DeltaTime);
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void AcclerationInfo(float DeltaTime);
	UFUNCTION(BlueprintCallable,meta=(BlueprintThreadSafe))
	void RotationInfo(float DeltaTime);
	/*VelocityType*/
	UPROPERTY(BlueprintReadOnly)
	FVector CurrentVelocity;
	UPROPERTY(BlueprintReadOnly)
	FVector LastVelocity;
	/*LocationType*/
	UPROPERTY(BlueprintReadOnly)
	FVector CurrentActorLocation;
	UPROPERTY(BlueprintReadOnly)
	FVector LastActorLocation;
	UPROPERTY(BlueprintReadOnly)
	float DistanceOffsetInDelta;
	UPROPERTY(BlueprintReadOnly)
	float Speed;
	/*Acceleration Type*/
	UPROPERTY(BlueprintReadOnly)
	FVector CurrentAcceleration;
	UPROPERTY(BlueprintReadOnly)
	FVector LastAcceleration;
	float AccelerationDeltaAngle;
	/*Rotation Type*/
	UPROPERTY(BlueprintReadOnly)
	FRotator CurrentRotation;
	UPROPERTY(BlueprintReadOnly)
	FRotator LastRotation;
	UPROPERTY(BlueprintReadOnly)
	bool BisRunningToTheWall;
};
