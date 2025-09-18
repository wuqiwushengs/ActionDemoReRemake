// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillClipBase.h"
#include "SkillClip_PlayMontage.generated.h"

class UAsyncTask_PlayMontage;
/**
 * 
 */
UCLASS(Blueprintable)
class ACTIONSKILLASSETRUNTIME_API USkillClip_PlayMontage : public USkillClipBase
{
	GENERATED_BODY()
public:

	virtual void OnEnterThisClip_Implementation() override;
	virtual void OnExitThisClip_Implementation() override;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FMultiAnimPlayed AnimPlayedRules;
	UPROPERTY(EditAnywhere)
	bool bEndAfterMontageOver=true;
	UPROPERTY()
	UAsyncTask_PlayMontage * MontagePlayTask;
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	UAnimMontage * MontagePlayedRule();
	virtual UAnimMontage * MontagePlayedRule_Implementation();
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void OnMontageBlendOut();
	virtual void OnMontageBlendOut_Implementation();
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void OnMontageInterrupt();
	virtual void OnMontageInterrupt_Implementation();
	
};
