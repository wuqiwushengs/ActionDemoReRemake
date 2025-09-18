// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncTask_PlayMontage.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAnimMontageDelegate);
UCLASS()
class ACTIONSKILLASSETRUNTIME_API UAsyncTask_PlayMontage : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static  UAsyncTask_PlayMontage* ActionCreateNewMontage(USkeletalMeshComponent * SkeletalMeshComponent, UAnimMontage *MontageToPlay, float Rate, FName StartSection, float StartTimeSeconds, bool bAllowInterruptAfterBlendOut);
	virtual void Activate() override;
	UPROPERTY(BlueprintAssignable)
	FAnimMontageDelegate OnMontageBlendOut;
	UPROPERTY(BlueprintAssignable)
	FAnimMontageDelegate OnMontageInterrupt;
	UPROPERTY(BlueprintAssignable)
	FAnimMontageDelegate OnMontageComplete;
	
private:
	UPROPERTY()
	USkeletalMeshComponent * Mesh;
	UPROPERTY()
	UAnimMontage * Montage;
	float Rate;
	FName StartSection;
	float StartTimeSeconds;
	bool bAllowInterruptAfterBlendOut;
	FOnMontageEnded MontageEnded;
	UFUNCTION()
	void OnMontageEnded(UAnimMontage * PlayedMontage,bool binterrupt);
	FOnMontageBlendingOutStarted MontageBlendOut;
	UFUNCTION()
	void OnMontageBlendingOut(UAnimMontage * PlayedMontage, bool binterrupt);
};
